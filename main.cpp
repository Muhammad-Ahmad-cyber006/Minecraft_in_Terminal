#include "terminal.h"
#include "input.h"
#include "world.h"
#include "raycast.h"
#include "render.h"
#include "player.h"
#include <windows.h>

#define ACTION_COOLDOWN_MS 160
#define TARGET_FRAME_MS 16

int main()
{
    init_terminal();

    int cols, rows;
    get_console_size(&cols, &rows);
    X_PIXELS = cols > 20 ? cols : 20;
    Y_PIXELS = (rows - 2) > 10 ? (rows - 2) : 10;
    VIEW_HEIGHT = VIEW_WIDTH * ((float)Y_PIXELS / (float)X_PIXELS) * CHAR_ASPECT;

    char** picture = init_picture();
    int** highlight = init_int_grid();
    int** parity = init_int_grid();
    int** texture = init_int_grid();
    float** dist = init_dist();
    char*** blocks = init_blocks();
    fb_init();

    for (int x = 0; x < X_BLOCKS; x++)
    {
        for (int y = 0; y < Y_BLOCKS; y++)
        {
            for (int z = 0; z < 4; z++)
            {
                blocks[z][y][x] = '@';
            }
        }
    }

    player_pos_view posview = init_posview();
    char place_char = WOOD_CHAR;
    int placed_count = 0, mined_count = 0;

    ULONGLONG last_tick = GetTickCount64();
    ULONGLONG last_break = 0, last_place = 0;

    while (1)
    {
        ULONGLONG frame_start = GetTickCount64();
        float dt = (float)(frame_start - last_tick) / 1000.0f;
        if (dt > 0.1f) dt = 0.1f;
        last_tick = frame_start;

        if (is_vk_pressed('Q') || is_vk_pressed(VK_ESCAPE)) break;
        if (is_key_pressed('1')) place_char = WOOD_CHAR;
        if (is_key_pressed('2')) place_char = STONE_CHAR;
        if (is_key_pressed('3')) place_char = LEAVES_CHAR;
        if (is_key_pressed('4')) place_char = GLASS_CHAR;
        if (is_key_pressed('5')) place_char = SAND_CHAR;
        if (is_key_pressed('6')) place_char = WATER_CHAR;

        update_pos_view(&posview, blocks, dt);

        vect current_block = get_current_block(posview, blocks);
        int have_current_block = !ray_outside(current_block);
        int cbx = -1, cby = -1, cbz = -1;
        if (have_current_block)
        {
            cbx = (int)current_block.x;
            cby = (int)current_block.y;
            cbz = (int)current_block.z;
        }

        int pbx = -1, pby = -1, pbz = -1;
        if (have_current_block)
        {
            place_position(current_block, &pbx, &pby, &pbz);
        }
        g_previewColor = preview_color_for(place_char);

        ULONGLONG now = frame_start;
        int want_break = is_vk_pressed(VK_LBUTTON) || is_key_pressed('x');
        int want_place = is_vk_pressed(VK_RBUTTON) || is_key_pressed(' ');

        if (have_current_block)
        {
            if (want_break && (now - last_break) >= ACTION_COOLDOWN_MS)
            {
                blocks[cbz][cby][cbx] = ' ';
                mined_count++;
                last_break = now;
            }
            else if (want_place && (now - last_place) >= ACTION_COOLDOWN_MS)
            {
                place_block(current_block, blocks, place_char);
                placed_count++;
                last_place = now;
            }
        }

        get_picture(picture, highlight, dist, parity, texture, posview, blocks,
                    cbx, cby, cbz, pbx, pby, pbz);
        draw_crosshair(picture);
        draw_ascii(picture, highlight, dist, parity, texture, posview, place_char,
                   placed_count, mined_count);

        ULONGLONG frame_end = GetTickCount64();
        ULONGLONG elapsed = frame_end - frame_start;
        if (elapsed < TARGET_FRAME_MS)
        {
            Sleep((DWORD)(TARGET_FRAME_MS - elapsed));
        }
    }

    fb_free();
    free_picture(picture);
    free_int_grid(highlight);
    free_int_grid(parity);
    free_int_grid(texture);
    free_dist(dist);
    free_blocks(blocks);
    restore_terminal();
    return 0;
}