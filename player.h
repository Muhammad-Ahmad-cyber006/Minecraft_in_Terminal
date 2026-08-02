#pragma once
#include "types.h"

#define MOVE_SPEED 3.2f
#define TURN_SPEED 1.0f
#define MOUSE_SENSITIVITY 0.00015f
#define MOUSE_MAX_DELTA 60.0f
#define PITCH_LIMIT 1.5f

void update_pos_view(player_pos_view* posview, char*** blocks, float dt);