#pragma once
#include "types.h"

#define PREVIEW_GLYPH '?'

int crosshair_x();
int crosshair_y();

vect ray_direction_for_pixel(vect2 view, int x_pix, int y_pix);

vect** init_directions(vect2 view);
void free_directions(vect** dir);

float next_voxel_distance(vect pos, vect dir);

FaceInfo compute_face_info(vect pos);

char raytrace(vect pos, vect dir, char*** blocks, int tx, int ty, int tz,
              int px, int py, int pz,
              int* isTarget, float* outDist, int* outParity, int* outTexture);

void get_picture(char** picture, int** highlight, float** dist, int** parity, int** texture,
                player_pos_view posview, char*** blocks, int tx, int ty, int tz,
                int px, int py, int pz);

vect get_current_block(player_pos_view posview, char*** blocks);