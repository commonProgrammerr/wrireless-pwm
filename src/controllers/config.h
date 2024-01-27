#ifndef __config_controller_h__
#define __config_controller_h__

#include <Arduino.h>

template <typename T>
class ConfigController
{
public:
  virtual bool load(const char *path, T *data) = 0;
  virtual bool save(const char *path, T *data) = 0;
  virtual void apply(T data) = 0;
  virtual void setup(T data) = 0;
};

#endif