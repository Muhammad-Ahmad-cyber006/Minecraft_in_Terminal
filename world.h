#pragma once
#include "types.h"

// map dimensions
//changing these numbers automatically scales the entire map
#define Z_BLOCKS 12
#define Y_BLOCKS 20
#define X_BLOCKS 20
#define EYE_HEIGHT 1.5f

// Placeable block characters

#define WOOD_CHAR   '+'
#define STONE_CHAR  '#'
#define LEAVES_CHAR '&'
#define GLASS_CHAR  '/'
#define SAND_CHAR   ','
#define WATER_CHAR  '~'

//Function declarations
char*** init_blocks();
void free_blocks(char*** blocks);
player_pos_view init_posview();

int ray_outside(vect pos);

// Returns neighboring cell for right-click placement
// Returns 0 if aimed face is against world edge 
int place_position(vect pos, int* outX, int* outY, int* outZ);
void place_block(vect pos, char*** blocks, char block);

// Block identity functions
char display_char(char block, int axis);
int face_type_index(char c);
const char* preview_color_for(char placeChar);
const char* block_name_for(char placeChar);