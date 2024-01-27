#ifndef __pwm_controller_h__
#define __pwm_controller_h__

#include <Arduino.h>
#include <SPIFFS.h>
#include "./config.h"

typedef struct
{
  bool enable;
  uint8_t pin;
  uint32_t frequency;
  int duty;
} pwm_config;

struct PWMController : ConfigController<pwm_config>
{
  PWMController(){};
  bool load(const char *path, pwm_config *data)
  {
    File config_file = SPIFFS.open(path);

    return config_file && config_file.read((byte *)data, sizeof(pwm_config)) == sizeof(pwm_config);
  }

  bool save(const char *path, pwm_config *data)
  {
    File config_file = SPIFFS.open(path, "w", true);

    return config_file && config_file.write((byte *)data, sizeof(pwm_config)) == sizeof(pwm_config);
  }

  virtual void apply(pwm_config data)
  {

    analogWriteResolution(13U);
    analogWriteFrequency(data.frequency);
    analogWrite(data.pin, data.duty);
    // int8_t channel = analogGetChannel(data.pin);

    // log_v("GPIO %d - Using Channel %d, Value = %d", data.pin, channel, data.duty);
    // if (ledcSetup(channel, data.frequency, 14U) == 0)
    // {
    //   log_e("analogWrite setup failed (freq = %u, resolution = %u). Try setting different resolution or frequency");
    //   return;
    // }
    // ledcAttachPin(data.pin, channel);

    // if (data.enable)
    //   ledcWrite(channel, data.duty);
    // else
    //   ledcWrite(channel, 0U);
  }

  void setup(pwm_config data)
  {
    pinMode(data.pin, OUTPUT);
  }
};

#endif