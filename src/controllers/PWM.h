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

  void apply(pwm_config data)
  {
    // pwm_config last_config;

    // this->load()
    }

  void setup(pwm_config data)
  {
    ledcSetup(0, data.frequency, 10);
    ledcAttachPin(4, 0);
  }
};

#endif