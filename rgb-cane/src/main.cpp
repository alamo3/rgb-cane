#include <Arduino.h>
#include <WS2812FX.h>
#include <Audio.h>
#include <WiFiMulti.h>
#define LED_PIN 22

#define X_OUT A13
#define Y_OUT A12
#define Z_OUT A11

#define NUMPIXELS 20

WS2812FX ws2812fx = WS2812FX(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

const float zeroG_voltage_Y = 1.52; // Typically 1.65V for a 3.3V supply
const float zeroG_voltage_X = 1.52;
const float zeroG_voltage_Z = 2.62; // Typically 1.65V for a 3.3V supply
const float sensitivity = 0.33;  // Typical sensitivity is 330mV per g

bool bMeasureAcceleration = false;
bool bTestAudio = true;

Audio audio(true, I2S_DAC_CHANNEL_BOTH_EN);
WiFiMulti wifiMulti;
String ssid =     "FastExtended";
String password = "canada2003";


void setup()
{
  Serial.begin(115200);
  ws2812fx.init();
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(1000);
  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();

  WiFi.mode(WIFI_STA);
    wifiMulti.addAP(ssid.c_str(), password.c_str());
    wifiMulti.run();
    if(WiFi.status() != WL_CONNECTED){
        WiFi.disconnect(true);
        wifiMulti.run();
    }

  audio.setVolume(10);

  audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3

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

    delay(1000);
  }

  audio.loop();


}

void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}