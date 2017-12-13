#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#include <Eina.h>
#include <Ecore.h>
#include "Ecore_Input.h"
#include <Ecore_Con.h>
#include <Elementary.h>

#define CAR_PORT 1111

extern Eina_Bool is_test;

void common_init(void);

Eina_Bool GPIOExport(int pin);

Eina_Bool GPIOUnexport(int pin);

Eina_Bool GPIOExists(int pin);

Eina_Bool GPIODirection(int pin, const char *dir);

Eina_Bool GPIORead(int pin, int *value);

Eina_Bool GPIOWrite(int pin, int value);

#endif
