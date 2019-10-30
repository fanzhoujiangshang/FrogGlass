#pragma once

#include "sys.h"
#include "stabilizer_types.h"

extern Axis3i16 magADC;

bool compassInit(void);
void compassSetCalibrationStart(void);
bool compassIsReady(void);
bool compassIsHealthy(void);
