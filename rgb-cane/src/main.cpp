#include <Arduino.h>
#include <WS2812FX.h>
#include <WiFiMulti.h>

#include <SD.h>
#include <SPI.h>
#include "Audio.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define SD_CS_PIN    10
#define SD_MOSI_PIN  11
#define SD_CLK_PIN   12
#define SD_MISO_PIN  13

SPIClass sdSPI(FSPI); // Create a custom SPI instance

#define I2S_LRC  4  // Word Select / Left-Right Clock
#define I2S_BCLK 5  // Bit Clock
#define I2S_DOUT 6  // Data Out

#define I2C_SDA 1
#define I2C_SCL 2

Audio audio;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup()
{

  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);

  sdSPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

  if (!SD.begin(SD_CS_PIN, sdSPI)) {
    Serial.println("SD Card Mount Failed!");
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(18);

  audio.connecttoFS(SD, "/morse.mp3");

  if(!accel.begin(0x53))
  {
    Serial.println("Unable to connect to adxl345");
  }

  accel.setRange(ADXL345_RANGE_16_G);

}

void loop() {
  audio.loop();
  //Serial.println("Test1");

  sensors_event_t event; 
  accel.getEvent(&event);
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  


}

