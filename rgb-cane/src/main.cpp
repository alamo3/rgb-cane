#include <Arduino.h>
#include <WS2812FX.h>
#include <AudioFileSourceSD.h>
#include <AudioFileSourceBuffer.h>
#include <AudioFileSourceHTTPStream.h>
#include <AudioGeneratorMP3.h>
#include <WiFiMulti.h>

#include <SD.h>
#include <SPI.h>
#define LED_PIN 22

#define X_OUT A13
#define Y_OUT A12
#define Z_OUT A11

#define DAC_PIN 25
#define SD_CS_PIN 5
#define NUMPIXELS 20

WS2812FX ws2812fx = WS2812FX(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500



const float zeroG_voltage_Y = 1.52; // Typically 1.65V for a 3.3V supply
const float zeroG_voltage_X = 1.52;
const float zeroG_voltage_Z = 2.62; // Typically 1.65V for a 3.3V supply
const float sensitivity = 0.33;  // Typical sensitivity is 330mV per g

bool bMeasureAcceleration = true;
bool bTestAudio = true;



WiFiMulti wifiMulti;
String ssid =     "Fa";
String password = "hact6104";

AudioFileSourceSD *file;

AudioGeneratorMP3 *mp3;
AudioOutputI2S *out;
File dir;

void setup()
{
  Serial.begin(115200);
  ws2812fx.init();
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(1000);
  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();

  // WiFi.mode(WIFI_STA);
  //   wifiMulti.addAP(ssid.c_str(), password.c_str());
  //   wifiMulti.run();
  //   if(WiFi.status() != WL_CONNECTED){
  //       WiFi.disconnect(true);
  //       wifiMulti.run();
  //   }

    file = new AudioFileSourceSD();
    mp3 = new AudioGeneratorMP3();
    out = new AudioOutputI2S(0, AudioOutputI2S::INTERNAL_DAC);
    out->SetOutputModeMono(true);
    out->SetGain(0.9);
    out->stop();



  if(!SD.begin(SD_CS_PIN)){
    Serial.println("SD Card Mount Failed!");
    while(1);
  }

  if(!file->open("/morse.mp3"))
  {
    Serial.println("Failed to open file on sd card!");
  }

  mp3->begin(file,out);


}

void loop() {
  ws2812fx.service();

  if(bMeasureAcceleration)
  {
    int x_adc_value, y_adc_value, z_adc_value; 

    x_adc_value = analogRead(X_OUT);
    y_adc_value = analogRead(Y_OUT);
    z_adc_value = analogRead(Z_OUT);

    // Convert raw ADC values to voltages
    float xVoltage = x_adc_value * (3.3 / 4095.0);
    float yVoltage = y_adc_value * (3.3 / 4095.0);
    float zVoltage = z_adc_value * (3.3 / 4095.0);

    // Calculate acceleration in g
    float xAccel = (xVoltage - zeroG_voltage_X) / sensitivity;
    float yAccel = (yVoltage - zeroG_voltage_Y) / sensitivity;
    float zAccel = (zVoltage - zeroG_voltage_Z) / sensitivity;


    double roll, pitch, yaw;


    // Serial.print("Roll = ");
    // Serial.print(roll);
    // Serial.print("\t");
    // Serial.print("Pitch = ");
    // Serial.print(pitch);
    // Serial.print("\t");
    // Serial.print("Yaw = ");
    // Serial.print(yaw);
    // Serial.print("\n\n");

    Serial.print("X = ");
    Serial.print(xAccel);
    Serial.print("\t");
    Serial.print("Y = ");
    Serial.print(yAccel);
    Serial.print("\t");
    Serial.print("Z = ");
    Serial.print(zAccel);
    Serial.print("\n\n");

    //delay(1000);
  }

  if(mp3->isRunning())
  {
    //Serial.println("MP3 is running");
    if (!mp3->loop()){
       mp3->stop(); 
       out->stop();
    }
  } else {
    Serial.printf("MP3 done\n");
    delay(1000);
    file->close();
    file->open("/morse.mp3");

    mp3->begin(file,out);
  }


}

void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}