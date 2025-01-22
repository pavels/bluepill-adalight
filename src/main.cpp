#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 128
#define BRIGHTNESS 255
#define DATA_PIN PA7

CRGB leds[NUM_LEDS];
uint8_t *ledsRaw = (uint8_t *)leds;

const uint8_t magic[] = {'A', 'd', 'a'};
#define MAGICSIZE sizeof(magic)

// Check values are header byte # - 1, as they are indexed from 0
#define HICHECK    (MAGICSIZE)
#define LOCHECK    (MAGICSIZE + 1)
#define CHECKSUM   (MAGICSIZE + 2)

enum processModes_t { Header, Data } mode;

int16_t c;                                  // current byte, must support -1 if no data available
uint16_t outPos;                            // current byte index in the LED array
uint32_t bytesRemaining;                    // count of bytes yet received, set by checksum
unsigned long t, lastByteTime, lastAckTime; // millisecond timestamps

const uint16_t SerialTimeout = 60;          // time before LEDs are shut off if no data (in seconds), 0 to disable

void headerMode();
void dataMode();
void timeouts();

void setup() {
  pinMode(PC13, OUTPUT);

  mode = Header;

  SerialUSB.begin();

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();

  SerialUSB.print("Ada\n");
  lastByteTime = lastAckTime = millis(); // Set initial counters

  digitalWrite(PC13, LOW);
}

void loop() {
  t = millis(); // Save current time

  // If there is new serial data
  if ((c = Serial.read()) >= 0) {
    lastByteTime = lastAckTime = t; // Reset timeout counters

    switch (mode) {
    case Header:
      headerMode();
      break;
    case Data:
      dataMode();
      break;
    }
  } else {
    // No new data
    timeouts();
  }
}

void headerMode() {
  static uint8_t
      headPos,
      hi, lo, chk;

  if (headPos < MAGICSIZE) {
    // Check if magic word matches
    if (c == magic[headPos]) {
      headPos++;
    } else {
      headPos = 0;
    }
  } else {
    // Magic word matches! Now verify checksum
    switch (headPos) {
    case HICHECK:
      hi = c;
      headPos++;
      break;
    case LOCHECK:
      lo = c;
      headPos++;
      break;
    case CHECKSUM:
      chk = c;
      if (chk == (hi ^ lo ^ 0x55)) {
        // Checksum looks valid. Get 16-bit LED count, add 1
        // (# LEDs is always > 0) and multiply by 3 for R,G,B.
        bytesRemaining = 3L * (256L * (long)hi + (long)lo + 1L);
        outPos = 0;
        memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
        mode = Data; // Proceed to latch wait mode
      }
      headPos = 0; // Reset header position regardless of checksum result
      break;
    }
  }
}

void dataMode() {
  // If LED data is not full
  if (outPos < sizeof(leds)) {
    ledsRaw[outPos++] = c; // Issue next byte
  }
  bytesRemaining--;

  if (bytesRemaining == 0) {
    // End of data -- issue latch:
    mode = Header; // Begin next header search
    FastLED.show();
  }
}

void timeouts() {
  // No data received. If this persists, send an ACK packet
  // to host once every second to alert it to our presence.
  if ((t - lastAckTime) >= 1000) {
    Serial.print("Ada\n"); // Send ACK string to host
    lastAckTime = t;       // Reset counter

    // If no data received for an extended time, turn off all LEDs.
    if (SerialTimeout != 0 && (t - lastByteTime) >= (uint32_t)SerialTimeout * 1000) {
      memset(leds, 0, NUM_LEDS * sizeof(struct CRGB)); // filling Led array by zeroes
      FastLED.show();
      mode = Header;
      lastByteTime = t; // Reset counter
    }
  }
}