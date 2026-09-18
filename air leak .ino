#define BLYNK_PRINT Serial

#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "secrets.h"

// ================= BLYNK =================
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

// ================= I2C =================
#define SDA_PIN 21
#define SCL_PIN 22

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ================= BMP280 =================
Adafruit_BMP280 bmp1;
Adafruit_BMP280 bmp2;

// ================= FLOW SENSORS =================
#define FLOW1_PIN 34
#define FLOW2_PIN 35

// ================= BUZZER =================
#define BUZZER_PIN 25

// ================= VARIABLES =================
volatile int pulse1 = 0;
volatile int pulse2 = 0;

float flow1_rate = 0;
float flow2_rate = 0;

unsigned long lastTime = 0;
unsigned long buzzerTimer = 0;

String leakStatus = "";

// ================= INTERRUPTS =================
void IRAM_ATTR flow1_ISR()
{
  pulse1++;
}

void IRAM_ATTR flow2_ISR()
{
  pulse2++;
}

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // ================= OLED =================
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED ERROR");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED OK");
  display.display();

  delay(2000);

  // ================= BMP280 =================
  if (!bmp1.begin(0x76))
  {
    Serial.println("BMP1 ERROR");
    while (1);
  }

  if (!bmp2.begin(0x77))
  {
    Serial.println("BMP2 ERROR");
    while (1);
  }

  // ================= FLOW SENSOR =================
  pinMode(FLOW1_PIN, INPUT);
  pinMode(FLOW2_PIN, INPUT);

  // ================= BUZZER =================
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ================= INTERRUPTS =================
  attachInterrupt(
    digitalPinToInterrupt(FLOW1_PIN),
    flow1_ISR,
    RISING
  );

  attachInterrupt(
    digitalPinToInterrupt(FLOW2_PIN),
    flow2_ISR,
    RISING
  );

  // ================= BLYNK =================
  Blynk.begin(
    BLYNK_AUTH_TOKEN,
    ssid,
    pass
  );

  lastTime = millis();

  Serial.println("System Started");
}

// ================= LOOP =================
void loop()
{
  Blynk.run();

  if (millis() - lastTime >= 1000)
  {
    // Disable interrupts while calculating pulses
    detachInterrupt(digitalPinToInterrupt(FLOW1_PIN));
    detachInterrupt(digitalPinToInterrupt(FLOW2_PIN));

    unsigned long duration = millis() - lastTime;

    // Flow calculation
    flow1_rate =
      (pulse1 / 7.5) *
      (1000.0 / duration);

    flow2_rate =
      (pulse2 / 7.5) *
      (1000.0 / duration);

    // Reset pulse counters
    pulse1 = 0;
    pulse2 = 0;

    // Re-enable interrupts
    attachInterrupt(
      digitalPinToInterrupt(FLOW1_PIN),
      flow1_ISR,
      RISING
    );

    attachInterrupt(
      digitalPinToInterrupt(FLOW2_PIN),
      flow2_ISR,
      RISING
    );

    // ================= PRESSURE =================
    float rawP1 = bmp1.readPressure() / 100.0;
    float rawP2 = bmp2.readPressure() / 100.0;

    float displayP1 = 0;
    float displayP2 = 0;

    // ================= LEAK LOGIC =================

    // System OFF
    if (
      flow1_rate < 0.2 &&
      flow2_rate < 0.2 &&
      rawP1 < 100 &&
      rawP2 < 100
    )
    {
      leakStatus = "SYSTEM OFF";

      displayP1 = 0;
      displayP2 = 0;
    }

    // Leak in Stage 2
    else if (
      rawP1 >= 800 &&
      rawP2 <= 400
    )
    {
      leakStatus = "LEAK IN STAGE 2";

      displayP1 = rawP1 / 1000.0;
      displayP2 = rawP2 / 1000.0;
    }

    // Leak in Stage 1
    else if (
      rawP1 < 1000 &&
      rawP2 <= 700
    )
    {
      leakStatus = "LEAK IN STAGE 1";

      displayP1 = rawP1 / 1000.0;
      displayP2 = rawP2 / 1000.0;
    }

    // Normal condition
    else
    {
      leakStatus = "NORMAL";

      displayP1 = rawP1 / 1000.0;
      displayP2 = rawP2 / 1000.0;
    }

    // ================= BUZZER =================
    if (
      leakStatus == "LEAK IN STAGE 1" ||
      leakStatus == "LEAK IN STAGE 2"
    )
    {
      if (millis() - buzzerTimer > 200)
      {
        digitalWrite(
          BUZZER_PIN,
          !digitalRead(BUZZER_PIN)
        );

        buzzerTimer = millis();
      }
    }
    else
    {
      digitalWrite(BUZZER_PIN, LOW);
    }

    // ================= SERIAL MONITOR =================
    Serial.print("P1: ");
    Serial.print(displayP1);
    Serial.print(" bar | ");

    Serial.print("P2: ");
    Serial.print(displayP2);
    Serial.print(" bar | ");

    Serial.print("F1: ");
    Serial.print(flow1_rate);
    Serial.print(" L/min | ");

    Serial.print("F2: ");
    Serial.print(flow2_rate);
    Serial.print(" L/min | ");

    Serial.println(leakStatus);

    // ================= OLED =================
    display.clearDisplay();

    display.setCursor(0, 0);
    display.print("P1: ");
    display.print(displayP1, 2);
    display.print(" bar");

    display.setCursor(0, 12);
    display.print("P2: ");
    display.print(displayP2, 2);
    display.print(" bar");

    display.setCursor(0, 26);
    display.print("F1: ");
    display.print(flow1_rate, 2);
    display.print(" L/min");

    display.setCursor(0, 38);
    display.print("F2: ");
    display.print(flow2_rate, 2);
    display.print(" L/min");

    display.setCursor(0, 52);
    display.print(leakStatus);

    display.display();

    // ================= BLYNK =================
    Blynk.virtualWrite(V0, displayP1);
    Blynk.virtualWrite(V1, displayP2);
    Blynk.virtualWrite(V2, flow1_rate);
    Blynk.virtualWrite(V3, flow2_rate);
    Blynk.virtualWrite(V4, leakStatus);

    // Update timer
    lastTime = millis();
  }
}
