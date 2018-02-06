#include "WINAPI_INPUT.h"

//definicja elementów statycznych klasy interfejsu
bool WINAPI_INPUT::keyPressed[256] = { 0 };

POSITION WINAPI_INPUT::mousePos = { 0 };