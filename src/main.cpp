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
void handleEnable(AsyncWebServerRequest *request);

byte chanels[2] = {0, 0};

AsyncWebServer server(80);
pwm_config config = {
    .enable = false,
    .pin = 4,
    .frequency = 1000,
    .duty = 512,
};

void handleUpdate(AsyncWebServerRequest *request);

void setup()
{
  SETUP_RGB(50);
  Serial.begin(115200);
  SPIFFS.begin();

  WiFi.softAP("wireless-pwm", "pwm12345");
  while (!Serial && millis() <= 3e4)
    delay(100);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  RGB_COLOR(RGB_RED);

  server.serveStatic("/", SPIFFS, "/", NULL)
      .setDefaultFile("index.html");
  server.on("/load", HTTP_ANY, [](AsyncWebServerRequest *request)
            {
    auto res = request->beginResponseStream("application/json");

    JsonDocument body;

     body["frequency"] = config.frequency;
     body["pin"] = config.pin;
     body["duty"] = config.duty;

    serializeJson(body, *res); 
    
    request->send(res); });

  server.on("/update-pwm", HTTP_POST, handleUpdate)
      .onBody(handleBody);

  server.on("/enable", HTTP_POST, handleEnable);

  server.begin();

  pinMode(0, INPUT_PULLUP);

  Serial.println("Getting last config");

  File config_file = SPIFFS.open("/pwm_config");
  if (!(config_file && config_file.read((byte *)&config, sizeof(pwm_config))))
    Serial.println("Fail to open config file");

  config.enable = false;

  ledcSetup(0, config.frequency, 10);
  ledcAttachPin(config.pin, 0);
  chanels[0] = config.pin;
}

void loop()
{
  if (!digitalRead(0))
  {
    config.enable = !config.enable;

    if (config.enable)
      ledcWrite(0, config.duty);
    else
      ledcWrite(0, 0);

    while (!digitalRead(0))
      delay(100);

    return;
  }

  if (config.enable)
  {
    RGB_COLOR(RGB_RED);
  }
  else if (WiFi.softAPgetStationNum())
  {
    RGB_COLOR(RGB_BLUE);
  }
  else
  {
    RGB_COLOR(RGB_GREEN);
  }
}

void handleEnable(AsyncWebServerRequest *request)
{

  Serial.print("New request on ");
  Serial.println(request->url());

  config.enable = !config.enable;

  if (chanels[0])
  {
    if (config.enable)
      ledcWrite(0, config.duty);
    else
      ledcWrite(0, 0);
  }

  request->send(200);
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

  JsonDocument body;
  deserializeJson(body, body_file);

  serializeJson(body, Serial);
  Serial.println();

  config.frequency = body["frequency"];
  config.pin = body["pin"];
  config.duty = body["duty"];

  if (config.pin != chanels[0])
  {
    ledcSetup(0, config.frequency, 10);
    ledcAttachPin(config.pin, 0);
    chanels[0] = config.pin;
  }
  else
    ledcChangeFrequency(0, config.frequency, 10);

  if (config.enable)
    ledcWrite(0, config.duty);
  else
    ledcWrite(0, 0);

  File config_file = SPIFFS.open("/pwm_config", "w", true);
  if (!(config_file && config_file.write((byte *)&config, sizeof(pwm_config))))
    Serial.println("Fail to save config file");

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