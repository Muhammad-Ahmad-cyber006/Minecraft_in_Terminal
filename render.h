#pragma once
#include "types.h"
extern int X_PIXELS;
extern int Y_PIXELS;
extern float VIEW_WIDTH;
extern float VIEW_HEIGHT;

#define CHAR_ASPECT 2.0f  
#define FOG_NEAR 6.0f     
#define FOG_FAR 14.0f      
extern const char* g_previewColor;

char** init_picture();
void free_picture(char** picture);
int** init_int_grid();
void free_int_grid(int** g);
float** init_dist();
void free_dist(float** d);

void fb_init();
void fb_free();
void draw_crosshair(char** picture);

void draw_ascii(char** picture, int** highlight, float** dist, int** parity, int** texture,
player_pos_view posview, char place_char, int placed_count, int mined_count);

int heading_degrees(float phi);
