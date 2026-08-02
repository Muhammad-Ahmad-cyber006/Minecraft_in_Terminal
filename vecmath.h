#pragma once
#include "types.h"

vect angles_to_vect(vect2 angles);
vect vect_add(vect v1, vect v2);
vect vect_scale(float s, vect v);
vect vect_sub(vect v1, vect v2);
void vect_normalize(vect* v);
float vect_length(vect v);
