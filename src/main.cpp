#include <Arduino.h>

#define PIN_CLOCK 2
#define PIN_EN 3
#define PIN_OUTPUT 4
#define PIN_INPUT 5

#define TEXT_BUF 64
#define TEXT_UPDATE_INTERVAL 2500

// TODO: Make sure the bitfield order is correct under AVR GCC

typedef union twinkle {
    struct {
      unsigned int first : 4;
      unsigned int second : 4;
    } slider;

    struct {
      unsigned int p1_start : 1;
      unsigned int p2_start : 1;
      unsigned int vefx : 1;
      unsigned int effect : 1;
      unsigned int credit_counter : 1;
      unsigned int filler : 3;
    } buttons;

    struct {
      unsigned int lamp3 : 1;
      unsigned int lamp2 : 1;
      unsigned int lamp1 : 1;
      unsigned int lamp0 : 1;
      unsigned int lamp4 : 1;
      unsigned int lamp5 : 1;
      unsigned int lamp6 : 1;
      unsigned int lamp7 : 1;
    } spotlights;

    struct {
      unsigned int neon_on : 1;
      unsigned int filler : 7;
    } neon;

    uint8_t raw;
} twinkle;

enum known_addresses {
  // Inputs
  INPUT_BUTTONS = 0x07, // effector / sys button status
  INPUT_TT_P1 = 0x0f, // p1 tt analog
  INPUT_TT_P2 = 0x17, // p2 tt analog
  INPUT_VOL_1_2 = 0x1f, // vol1 (lsb 4) / vol2 (msb 4)
  INPUT_VOL_3_4 = 0x27, // vol3 (lsb 4) // vol4 (msb 4)
  INPUT_VOL_5 = 0x2f, // vol 5
  // Outputs
  OUTPUT_BUTTON_LAMP = 0x37, // button lamps
  OUTPUT_16SEG_1 = 0x3f, // 16seg 1
  OUTPUT_16SEG_2 = 0x47, // 16seg 2
  OUTPUT_16SEG_3 = 0x4f, // 16seg 3
  OUTPUT_16SEG_4 = 0x57, // 16seg 4
  OUTPUT_16SEG_5 = 0x5f, // 16seg 5
  OUTPUT_16SEG_6 = 0x67, // 16seg 6
  OUTPUT_16SEG_7 = 0x6f, // 16seg 7
  OUTPUT_16SEG_8 = 0x77, // 16seg 8
  OUTPUT_16SEG_9 = 0x7f, // 16seg 9
  OUTPUT_SPOTLIGHTS = 0x87, // Spotlights (76540123)
  OUTPUT_NEON = 0x8f // Neon (only first bit?)
};

uint8_t addr_16seg[9] = {
  OUTPUT_16SEG_1,
  OUTPUT_16SEG_2,
  OUTPUT_16SEG_3,
  OUTPUT_16SEG_4,
  OUTPUT_16SEG_5,
  OUTPUT_16SEG_6,
  OUTPUT_16SEG_7,
  OUTPUT_16SEG_8,
  OUTPUT_16SEG_9,
};

// TODO: lamp output format might not match button input format
twinkle button_input;
uint8_t turntable_p1;
uint8_t turntable_p2;
twinkle slider_1_2;
twinkle slider_3_4;
twinkle slider_5;

char text_buffer[TEXT_BUF];
uint8_t text_length;
uint8_t text_index;

bool send_address(uint8_t address) {
  int i;
  bool is_ack = false;

  address &= 7;

  for (i=7;i>=0; i--) {
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_OUTPUT, (address >> i) & 1);
    delayMicroseconds(1);
    digitalWrite(PIN_CLOCK, HIGH);
    delayMicroseconds(1);
  }

  is_ack = !digitalRead(PIN_INPUT);
  digitalWrite(PIN_CLOCK, LOW);
  return is_ack;
}

uint8_t exchange_data(uint8_t send_data) {
  uint8_t recv_data = 0;
  int i;

  for (i=7; i>=0; i--) {
    digitalWrite(PIN_OUTPUT, (send_data >> i) & 1);
    delayMicroseconds(1);
    digitalWrite(PIN_CLOCK, HIGH);
    recv_data |= digitalRead(PIN_INPUT) << i;
    delayMicroseconds(1);
    digitalWrite(PIN_CLOCK, LOW);
  }

  return recv_data;
}

uint8_t transfer(uint8_t address, uint8_t send_data) {
  uint8_t recv_data;
  
  digitalWrite(PIN_EN, LOW);
  digitalWrite(PIN_CLOCK, LOW);
  send_address(address);
  recv_data = exchange_data(send_data);
  digitalWrite(PIN_CLOCK, HIGH);
  digitalWrite(PIN_EN, HIGH);
  delayMicroseconds(1);

  return recv_data;
}

void text_update() {
  static unsigned long text_last_update = 0;

  int i;

  if (millis() - text_last_update < TEXT_UPDATE_INTERVAL) {
    return;
  }
  text_last_update = millis();

  if (text_index == text_length) {
    text_index == 0;
    return;
  }

  for (i=0; i<9; i++) {
    transfer(
      addr_16seg[i],
      (i + text_index) >= text_length ? 0x20 : text_buffer[i + text_index]
    );
  }
}

void setup() {
  twinkle spotlights = {.spotlights={1, 1, 1, 1, 1, 1, 1, 1}};
  twinkle neon = {.neon={1, 0x7f}};
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_OUTPUT, OUTPUT);
  pinMode(PIN_INPUT, INPUT);

  digitalWrite(PIN_CLOCK, HIGH);
  digitalWrite(PIN_EN, HIGH);

  transfer(
    OUTPUT_SPOTLIGHTS,
    spotlights.raw
  );

  transfer(
    OUTPUT_NEON,
    neon.raw
  );

  strcpy(text_buffer, "HELLO - WORLD !");
  text_length = 15;
  text_index = 0;
}

void loop() {
  button_input.raw = transfer(INPUT_BUTTONS, 0xff);
  turntable_p1 = transfer(INPUT_TT_P1, 0xff);
  turntable_p2 = transfer(INPUT_TT_P2, 0xff);
  slider_1_2.raw = transfer(INPUT_VOL_1_2, 0xff);
  slider_3_4.raw = transfer(INPUT_VOL_3_4, 0xff);
  slider_5.raw = transfer(INPUT_VOL_5, 0xff);

  text_update();
}
