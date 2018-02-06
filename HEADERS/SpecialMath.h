#pragma once
#include "Vector.h"

bool CheckPointInTriangle(Vector point, Vector triangle[], Vector normal);												//sprawdzenie czy pkt. 'point' znajduje siê wewn¹trz trójk¹ta 'triangle'

Vector PointSurfaceProjection(Vector surface[], Vector normal, Vector point, Vector projectionVec);						//rzut pkt. 'point' na powierzchniê 'surface' wzd³u¿ wek. 'projectionVec'