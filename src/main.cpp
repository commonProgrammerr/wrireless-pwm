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
PWMController PWM = PWMController();

#define SETUP_RGB(Brightness) \
  RGB.begin();                \
  RGB.setBrightness(Brightness)

#define RGB_COLOR(COLOR)         \
  delayMicroseconds(300);        \
  RGB.setLedColorData(0, COLOR); \
  RGB.show();                    \
  delayMicroseconds(300)

void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

AsyncWebServer server(80);

pwm_config config = {
    .enable = false,
    .pin = 1,
    .frequency = 0,
    .duty = 0,
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

  server.serveStatic("/", SPIFFS, "/", NULL)
      .setDefaultFile("index.html");
  server.on("/update-pwm", HTTP_POST, handleUpdate)
      .onBody(handleBody);

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
  if (request->client()->disconnected())
  {
    Serial.println("Client disconnected");
    return;
  }

  if (request->contentType() != "application/json")
    request->send(400);

  Serial.print("New request on ");
  Serial.println(request->url());

  File body_file = SPIFFS.open(request->url() + "/body.json");
  if (!body_file)
  {
    Serial.println("Fail to open body file");
    return request->send(500);
  }

  auto stream = request->beginResponseStream("application/json");

  JsonDocument body;
  deserializeJson(body, body_file);

  config.enable = body["enable"];
  config.frequency = body["frequency"];
  config.pin = body["pin"];
  config.duty = body["duty"];

  PWM.setup(config);
  PWM.apply(config);
  PWM.save("/output.config", &config);
  request->send(200);
}

void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{

  File body_file = SPIFFS.open(request->url() + "/body.json", index ? "a" : "w", true);

  if (!body_file)
  {
    Serial.println("Fail to open body file");
    request->send(500);
    return delay(10);
  }

  body_file.write(data, len);
}