#ifndef CODECRAFTSDK_NAVIGATE_H
#define CODECRAFTSDK_NAVIGATE_H

#include <string>
#include <cmath>

#include "data.h"
#include "Logger.h"

extern Logger logger;

void navigate(int, int);
double calculateTime(int, int, int, bool);

#endif //CODECRAFTSDK_NAVIGATE_H
