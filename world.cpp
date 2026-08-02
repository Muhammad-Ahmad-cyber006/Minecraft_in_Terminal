#include "world.h"
#include "terminal.h"
#include <stdlib.h>
#include <math.h>


 //Initialize 3D block array for world
char*** init_blocks()
{
    char*** blocks = (char***)malloc(sizeof(char**) * Z_BLOCKS);
    if (!blocks) die("Out of memory allocating world.");

    for (int i = 0; i < Z_BLOCKS; i++) 
    {
        blocks[i] = (char**)malloc(sizeof(char*) * Y_BLOCKS);
        if (!blocks[i]) die("Out of memory allocating world layer.");

        for (int j = 0; j < Y_BLOCKS; j++) 
        {
            blocks[i][j] = (char*)malloc(sizeof(char) * X_BLOCKS);
            if (!blocks[i][j]) die("Out of memory allocating world row.");

            for (int k = 0; k < X_BLOCKS; k++) 
            {
                blocks[i][j][k] = ' ';
            }
        }
    }
    return blocks;
}


void free_blocks(char*** blocks)
{
    for (int i = 0; i < Z_BLOCKS; i++) 
    {
        for (int j = 0; j < Y_BLOCKS; j++) free(blocks[i][j]);
        free(blocks[i]);
    }
    free(blocks);
}


player_pos_view init_posview()
{
    player_pos_view posview;
    posview.pos.x = X_BLOCKS / 2.0f;
    posview.pos.y = Y_BLOCKS / 2.0f;
    posview.pos.z = 4 + EYE_HEIGHT;
    posview.view.phi = 0;
    posview.view.psi = 0;
    return posview;
}


int ray_outside(vect pos)//Check if position is outside world boundaries
{
    //Returns 1 if outside, 0 if inside
    return (pos.x >= X_BLOCKS || pos.y >= Y_BLOCKS || pos.z >= Z_BLOCKS ||pos.x < 0 || pos.y < 0 || pos.z < 0);
}


int place_position(vect pos, int* outX, int* outY, int* outZ)//Determine placement position for block
{
    // Finds closest face to position and returns adjacent cell
    int x = (int)pos.x, y = (int)pos.y, z = (int)pos.z;
    float dists[6] = {
        fabsf(x + 1 - pos.x), fabsf(pos.x - x),
        fabsf(y + 1 - pos.y), fabsf(pos.y - y),
        fabsf(z + 1 - pos.z), fabsf(pos.z - z)
    };

    int best = 0;
    for (int i = 1; i < 6; i++) if (dists[i] < dists[best]) best = i;
    // Returns 1 if valid position found, 0 if at world edge
    switch (best) 
    {
        case 0: if (x + 1 < X_BLOCKS) { *outX = x + 1; *outY = y;     *outZ = z;     return 1; } break;
        case 1: if (x - 1 >= 0)       { *outX = x - 1; *outY = y;     *outZ = z;     return 1; } break;
        case 2: if (y + 1 < Y_BLOCKS) { *outX = x;     *outY = y + 1; *outZ = z;     return 1; } break;
        case 3: if (y - 1 >= 0)       { *outX = x;     *outY = y - 1; *outZ = z;     return 1; } break;
        case 4: if (z + 1 < Z_BLOCKS) { *outX = x;     *outY = y;     *outZ = z + 1; return 1; } break;
        case 5: if (z - 1 >= 0)       { *outX = x;     *outY = y;     *outZ = z - 1; return 1; } break;
    }
    return 0;
}


//Place block at calculated position

void place_block(vect pos, char*** blocks, char block)
{
    int px, py, pz;
    if (place_position(pos, &px, &py, &pz)) 
    {
        blocks[pz][py][px] = block;
    }
}

//Get display character for block based on view axis
char display_char(char block, int axis)
{
    if (block == '@') return (axis == 2) ? '@' : '%'; 
    if (block == WOOD_CHAR) return (axis == 2) ? '+' : '='; 
    return block;
}


int face_type_index(char c)
{
    switch (c) 
    {
        case '@': return 0; 
        case '%': return 1;  
        case STONE_CHAR:  return 2;
        case WOOD_CHAR:   return 3; 
        case '=':         return 4; 
        case LEAVES_CHAR: return 5;
        case GLASS_CHAR:  return 6;
        case SAND_CHAR:   return 7;
        case WATER_CHAR:  return 8;
        default:  return -1;
    }
}


const char* preview_color_for(char placeChar)
{
    switch (placeChar) 
    {
        case WOOD_CHAR:   return "\x1b[1;38;5;215m"; // wood 
        case STONE_CHAR:  return "\x1b[1;38;5;250m"; // stone 
        case LEAVES_CHAR: return "\x1b[1;38;5;46m";  // leaves
        case GLASS_CHAR:  return "\x1b[1;38;5;195m"; // glass 
        case SAND_CHAR:   return "\x1b[1;38;5;222m"; // sand 
        case WATER_CHAR:  return "\x1b[1;38;5;39m";  // water 
        default:  return "\x1b[1;38;5;231m"; // fallback just for safety, should not happen
    }
}



const char* block_name_for(char placeChar)
{
    switch (placeChar) 
    {
        case WOOD_CHAR:   return "Wood";
        case STONE_CHAR:  return "Stone";
        case LEAVES_CHAR: return "Leaves";
        case GLASS_CHAR:  return "Glass";
        case SAND_CHAR:   return "Sand";
        case WATER_CHAR:  return "Water";
        default:  return "?";
    }
}