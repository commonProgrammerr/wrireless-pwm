#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <Freenove_WS2812_Lib_for_ESP32.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "controllers/PWM.h"

/* ESP32s2 PWM ports 1~17, 19~21, 26, 33~45 */

#define LEDS_COUNT 8
#define LEDS_PIN 18
#define CHANNEL 0

#define RGB_RED 255, 0, 0
#define RGB_BLACK 0, 0, 0
#define RGB_BLUE 0, 0, 255
#define RGB_GREEN 0, 255, 0
#define RGB_YELLOW 255, 255, 0
#define RGB_WHITE 255, 255, 255
#define RGB_PINK 255, 0, 132

Freenove_ESP32_WS2812 RGB = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

#define SETUP_RGB(Brightness) \
  RGB.begin();                \
  RGB.setBrightness(Brightness)

#define RGB_COLOR(COLOR)         \
  delayMicroseconds(300);        \
  RGB.setLedColorData(0, COLOR); \
  RGB.show();                    \
  delayMicroseconds(300)

// Freenove_ESP32_WS2812 RGB = Freenove_ESP32_WS2812(1, 18, 0, TYPE_GRB);

AsyncWebServer server(80);

pwm_config PWMOutPuts[2] = {
    {.enable = false,
     .pin = 1,
     .frequency = 0,
     .value = 0},
    {.enable = false,
     .pin = 1,
     .frequency = 0,
     .value = 0},
};

void handleUpdate(AsyncWebServerRequest *request);

void setup()
{
  SETUP_RGB(50);
  Serial.begin(115200);
  SPIFFS.begin();

  WiFi.softAP("wireless-pwm");
  while (!Serial && millis() <= 3e4)
    delay(100);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  RGB_COLOR(RGB_GREEN);

  AsyncStaticWebHandler *static_handler = new AsyncStaticWebHandler("/", SPIFFS, "/", NULL);
  static_handler->setDefaultFile("index.html");
  server.addHandler(static_handler);
  server.on("/update-speed", HTTP_POST, handleUpdate);
  server.begin();

  analogWriteResolution(14);
}

void loop()
{
  delay(100);
  if (WiFi.softAPgetStationNum())
  {
    RGB_COLOR(RGB_BLUE);
  }
  else
  {
    RGB_COLOR(RGB_GREEN);
  }
}

JsonObject getParamsObj(AsyncWebServerRequest *request)
{
  JsonObject params;
  AsyncWebParameter *p;
  int total = request->params();
  for (int i = 0; i < total; i++)
  {
    p = request->getParam(i);
    params[p->name().c_str()] = p->value();
  }

  return params;
}

void handleUpdate(AsyncWebServerRequest *request)
{
  // if (!request->hasParam("index"))
  // {
  //   request->send(400, "text/plain", "Invalid request");
  //   return;
  // }

  JsonObject params = getParamsObj(request);
  Serial.println("Received params:");
  serializeJson(params, Serial);

  int index = params["index"].as<String>().toInt();

  PWMOutPuts[index].pin = params["pin"].as<int>();
  PWMOutPuts[index].enable = params["enable"] == "true";
  PWMOutPuts[index].frequency = params["frequency"].as<String>().toInt();
  PWMOutPuts[index].value = params["value"].as<String>().toInt();

  // String frequency = params["frequency"];
  // String value = params["value"];

  request->send(200, "text/plain", "");
}
