#include "vecmath.h"
#include <math.h>

vect angles_to_vect(vect2 angles) 
{
    vect res;
    res.x = cosf(angles.psi) * cosf(angles.phi);
    res.y = cosf(angles.psi) * sinf(angles.phi);
    res.z = sinf(angles.psi);
    return res;
}

vect vect_add(vect v1, vect v2) 
{
    vect res = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    return res;
}

vect vect_scale(float s, vect v) 
{
    vect res = { s * v.x, s * v.y, s * v.z };
    return res;
}

vect vect_sub(vect v1, vect v2) 
{
    return vect_add(v1, vect_scale(-1.0f, v2));
}

void vect_normalize(vect* v) 
{
    float len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len < 1e-6f) return;
    v->x /= len; v->y /= len; v->z /= len;
}

float vect_length(vect v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
