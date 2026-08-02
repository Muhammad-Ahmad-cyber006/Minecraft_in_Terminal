#include "raycast.h"
#include "world.h"
#include "render.h"
#include "vecmath.h"
#include "terminal.h"
#include <stdlib.h>
#include <math.h>

int crosshair_x()
{
    return X_PIXELS / 2;
}

int crosshair_y()
{
    return Y_PIXELS / 2;
}

vect ray_direction_for_pixel(vect2 view, int x_pix, int y_pix)
{
    view.psi -= VIEW_HEIGHT / 2.0f;
    vect screen_down = angles_to_vect(view);
    view.psi += VIEW_HEIGHT;
    vect screen_up = angles_to_vect(view);
    view.psi -= VIEW_HEIGHT / 2.0f;
    view.phi -= VIEW_WIDTH / 2.0f;
    vect screen_left = angles_to_vect(view);
    view.phi += VIEW_WIDTH;
    vect screen_right = angles_to_vect(view);
    view.phi -= VIEW_WIDTH / 2.0f;

    vect screen_mid_vert = vect_scale(0.5f, vect_add(screen_up, screen_down));
    vect screen_mid_hor = vect_scale(0.5f, vect_add(screen_left, screen_right));
    vect mid_to_left = vect_sub(screen_left, screen_mid_hor);
    vect mid_to_up = vect_sub(screen_up, screen_mid_vert);

    vect tmp = vect_add(vect_add(screen_mid_hor, mid_to_left), mid_to_up);
    tmp = vect_sub(tmp, vect_scale(((float)x_pix / (X_PIXELS - 1)) * 2.0f, mid_to_left));
    tmp = vect_sub(tmp, vect_scale(((float)y_pix / (Y_PIXELS - 1)) * 2.0f, mid_to_up));
    vect_normalize(&tmp);
    return tmp;
}

vect** init_directions(vect2 view)
{
    vect** dir = (vect**)malloc(sizeof(vect*) * Y_PIXELS);
    if (!dir) die("Out of memory allocating ray directions.");
    for (int i = 0; i < Y_PIXELS; i++)
    {
        dir[i] = (vect*)malloc(sizeof(vect) * X_PIXELS);
        if (!dir[i]) die("Out of memory allocating ray direction row.");
    }
    for (int y_pix = 0; y_pix < Y_PIXELS; y_pix++)
    {
        for (int x_pix = 0; x_pix < X_PIXELS; x_pix++)
        {
            dir[y_pix][x_pix] = ray_direction_for_pixel(view, x_pix, y_pix);
        }
    }
    return dir;
}

void free_directions(vect** dir)
{
    for (int i = 0; i < Y_PIXELS; i++) free(dir[i]);
    free(dir);
}

float next_voxel_distance(vect pos, vect dir)
{
    const float eps = 0.01f;
    float dist = 2.0f;
    float d;
    if (dir.x > eps)
    {
        d = ((int)(pos.x + 1) - pos.x) / dir.x;
        if (d < dist) dist = d;
    }
    else if (dir.x < -eps)
    {
        d = ((int)pos.x - pos.x) / dir.x;
        if (d < dist) dist = d;
    }
    if (dir.y > eps)
    {
        d = ((int)(pos.y + 1) - pos.y) / dir.y;
        if (d < dist) dist = d;
    }
    else if (dir.y < -eps)
    {
        d = ((int)pos.y - pos.y) / dir.y;
        if (d < dist) dist = d;
    }
    if (dir.z > eps)
    {
        d = ((int)(pos.z + 1) - pos.z) / dir.z;
        if (d < dist) dist = d;
    }
    else if (dir.z < -eps)
    {
        d = ((int)pos.z - pos.z) / dir.z;
        if (d < dist) dist = d;
    }
    return dist;
}

FaceInfo compute_face_info(vect pos)
{
    float fx = pos.x - floorf(pos.x);
    float fy = pos.y - floorf(pos.y);
    float fz = pos.z - floorf(pos.z);
    float dx = fminf(fx, 1.0f - fx);
    float dy = fminf(fy, 1.0f - fy);
    float dz = fminf(fz, 1.0f - fz);
    FaceInfo info;
    if (dz <= dx && dz <= dy)
    {
        info.axis = 2;
        info.u = fx;
        info.v = fy;
    }
    else if (dx <= dy)
    {
        info.axis = 0;
        info.u = fy;
        info.v = fz;
    }
    else
    {
        info.axis = 1;
        info.u = fx;
        info.v = fz;
    }
    return info;
}

#define EDGE_THRESHOLD 0.075f
#define TEXTURE_MAX_DIST FOG_FAR

static int texture_variant(FaceInfo f, float dist)
{
    if (dist > TEXTURE_MAX_DIST) return 0;
    if (f.u < EDGE_THRESHOLD || f.u > 1.0f - EDGE_THRESHOLD ||
        f.v < EDGE_THRESHOLD || f.v > 1.0f - EDGE_THRESHOLD)
    {
        return 2;
    }
    return 0;
}

#define PREVIEW_EDGE_THRESHOLD 0.09f

char raytrace(vect pos, vect dir, char*** blocks, int tx, int ty, int tz,
              int px, int py, int pz,
              int* isTarget, float* outDist, int* outParity, int* outTexture)
{
    const float eps = 0.01f;
    vect origin = pos;
    while (!ray_outside(pos))
    {
        int bx = (int)pos.x, by = (int)pos.y, bz = (int)pos.z;
        char c = blocks[bz][by][bx];
        if (c != ' ')
        {
            FaceInfo f = compute_face_info(pos);
            float d = vect_length(vect_sub(pos, origin));
            *isTarget = (bx == tx && by == ty && bz == tz);
            *outDist = d;
            *outParity = (bx + by + bz) & 1;
            *outTexture = texture_variant(f, d);
            return display_char(c, f.axis);
        }
        if (px >= 0 && bx == px && by == py && bz == pz)
        {
            FaceInfo f = compute_face_info(pos);
            if (f.u < PREVIEW_EDGE_THRESHOLD || f.u > 1.0f - PREVIEW_EDGE_THRESHOLD ||
                f.v < PREVIEW_EDGE_THRESHOLD || f.v > 1.0f - PREVIEW_EDGE_THRESHOLD)
            {
                *isTarget = 0;
                *outDist = vect_length(vect_sub(pos, origin));
                *outParity = 0;
                *outTexture = 0;
                return PREVIEW_GLYPH;
            }
        }
        float dist = next_voxel_distance(pos, dir);
        pos = vect_add(pos, vect_scale(dist + eps, dir));
    }
    *isTarget = 0;
    *outDist = 1e9f;
    *outParity = 0;
    *outTexture = 0;
    return ' ';
}

void get_picture(char** picture, int** highlight, float** dist, int** parity, int** texture,
                  player_pos_view posview, char*** blocks, int tx, int ty, int tz,
                  int px, int py, int pz)
{
    vect** directions = init_directions(posview.view);
    for (int y = 0; y < Y_PIXELS; y++)
    {
        for (int x = 0; x < X_PIXELS; x++)
        {
            int isTarget = 0, par = 0, tex = 0;
            float d = 0.0f;
            picture[y][x] = raytrace(posview.pos, directions[y][x], blocks, tx, ty, tz,
                                      px, py, pz, &isTarget, &d, &par, &tex);
            highlight[y][x] = isTarget;
            dist[y][x] = d;
            parity[y][x] = par;
            texture[y][x] = tex;
        }
    }
    free_directions(directions);
}

vect get_current_block(player_pos_view posview, char*** blocks)
{
    vect pos = posview.pos;
    vect dir = ray_direction_for_pixel(posview.view, crosshair_x(), crosshair_y());
    const float eps = 0.01f;
    while (!ray_outside(pos))
    {
        char c = blocks[(int)pos.z][(int)pos.y][(int)pos.x];
        if (c != ' ') return pos;
        float dist = next_voxel_distance(pos, dir);
        pos = vect_add(pos, vect_scale(dist + eps, dir));
    }
    return pos;
}