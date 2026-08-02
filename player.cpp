#include "player.h"
#include "world.h"
#include "input.h"
#include "terminal.h"
#include "vecmath.h"
#include <windows.h>
#include <math.h>

void update_pos_view(player_pos_view* posview, char*** blocks, float dt) {
    // Crude step-based vertical collision (push out of solid floor, fall through solid ceiling). This is a bit weird but it works 
    int x = (int)posview->pos.x, y = (int)posview->pos.y;
    int z = (int)(posview->pos.z - EYE_HEIGHT + 0.01f);
    if (z >= 0 && z < Z_BLOCKS && blocks[z][y][x] != ' ') {
        posview->pos.z += 1;
    }
    z = (int)(posview->pos.z - EYE_HEIGHT - 0.01f);
    if (z >= 0 && z < Z_BLOCKS && blocks[z][y][x] == ' ') {
        posview->pos.z -= 1;
    }

    // Keyboard/arrow-key turning 
    if (is_vk_pressed(VK_UP))    posview->view.psi += TURN_SPEED * dt;
    if (is_vk_pressed(VK_DOWN))  posview->view.psi -= TURN_SPEED * dt;
    if (is_vk_pressed(VK_RIGHT)) posview->view.phi += TURN_SPEED * dt;
    if (is_vk_pressed(VK_LEFT))  posview->view.phi -= TURN_SPEED * dt;

    // Mouse-look, only while our console is the focused window, so the
    if (has_focus()) 
    {
        POINT cur;
        GetCursorPos(&cur);
        float dx = (float)(cur.x - g_screenCenterX);
        float dy = (float)(cur.y - g_screenCenterY);

        if (dx > MOUSE_MAX_DELTA) dx = MOUSE_MAX_DELTA;
        if (dx < -MOUSE_MAX_DELTA) dx = -MOUSE_MAX_DELTA;
        if (dy > MOUSE_MAX_DELTA) dy = MOUSE_MAX_DELTA;
        if (dy < -MOUSE_MAX_DELTA) dy = -MOUSE_MAX_DELTA;

        posview->view.phi += dx * MOUSE_SENSITIVITY;
        posview->view.psi -= dy * MOUSE_SENSITIVITY;
        SetCursorPos(g_screenCenterX, g_screenCenterY);
    }

    if (posview->view.psi > PITCH_LIMIT) posview->view.psi = PITCH_LIMIT;
    if (posview->view.psi < -PITCH_LIMIT) posview->view.psi = -PITCH_LIMIT;

    // WASD movement

    float moveForward = 0.0f, moveStrafe = 0.0f;
    if (is_key_pressed('w')) moveForward += 1.0f;
    if (is_key_pressed('s')) moveForward -= 1.0f;
    if (is_key_pressed('d')) moveStrafe += 1.0f;  // right
    if (is_key_pressed('a')) moveStrafe -= 1.0f;  // left

    float mag = sqrtf(moveForward * moveForward + moveStrafe * moveStrafe);
    if (mag > 1e-4f) {
        moveForward /= mag;
        moveStrafe /= mag;
        vect direction = angles_to_vect(posview->view);
        float dist = MOVE_SPEED * dt;
        posview->pos.x += dist * (moveForward * direction.x - moveStrafe * direction.y);
        posview->pos.y += dist * (moveForward * direction.y + moveStrafe * direction.x);
    }

    // Keep the player inside the world grid so array look-ups stay valid.
    if (posview->pos.x < 0.001f) posview->pos.x = 0.001f;
    if (posview->pos.x > X_BLOCKS - 0.001f) posview->pos.x = X_BLOCKS - 0.001f;
    if (posview->pos.y < 0.001f) posview->pos.y = 0.001f;
    if (posview->pos.y > Y_BLOCKS - 0.001f) posview->pos.y = Y_BLOCKS - 0.001f;
}
