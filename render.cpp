#include "render.h"
#include "world.h"
#include "raycast.h"
#include "terminal.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int X_PIXELS = 120;
int Y_PIXELS = 40;
float VIEW_WIDTH = 1.0f;
float VIEW_HEIGHT = 0.5f;
const char* g_previewColor = "\x1b[1;38;5;231m";

char** init_picture()
{
    char** picture = (char**)malloc(sizeof(char*) * Y_PIXELS);
    if (!picture) die("Out of memory allocating picture buffer.");
    for (int i = 0; i < Y_PIXELS; i++)
    {
        picture[i] = (char*)malloc(sizeof(char) * X_PIXELS);
        if (!picture[i]) die("Out of memory allocating picture row.");
    }
    return picture;
}

void free_picture(char** picture)
{
    for (int i = 0; i < Y_PIXELS; i++) free(picture[i]);
    free(picture);
}

int** init_int_grid()
{
    int** g = (int**)malloc(sizeof(int*) * Y_PIXELS);
    if (!g) die("Out of memory allocating grid buffer.");
    for (int i = 0; i < Y_PIXELS; i++)
    {
        g[i] = (int*)malloc(sizeof(int) * X_PIXELS);
        if (!g[i]) die("Out of memory allocating grid row.");
    }
    return g;
}

void free_int_grid(int** g)
{
    for (int i = 0; i < Y_PIXELS; i++) free(g[i]);
    free(g);
}

float** init_dist()
{
    float** d = (float**)malloc(sizeof(float*) * Y_PIXELS);
    if (!d) die("Out of memory allocating distance buffer.");
    for (int i = 0; i < Y_PIXELS; i++)
    {
        d[i] = (float*)malloc(sizeof(float) * X_PIXELS);
        if (!d[i]) die("Out of memory allocating distance row.");
    }
    return d;
}

void free_dist(float** d)
{
    for (int i = 0; i < Y_PIXELS; i++) free(d[i]);
    free(d);
}

static char* g_framebuf = NULL;
static size_t g_framecap = 0;
static size_t g_framepos = 0;

void fb_init()
{
    g_framecap = (size_t)(X_PIXELS + 16) * (size_t)Y_PIXELS * 16 + 256;
    g_framebuf = (char*)malloc(g_framecap);
    if (!g_framebuf) die("Out of memory allocating frame buffer.");
}

void fb_free()
{
    free(g_framebuf);
    g_framebuf = NULL;
}

static void fb_reset()
{
    g_framepos = 0;
}

static void fb_puts(const char* s)
{
    while (*s) g_framebuf[g_framepos++] = *s++;
}

static void fb_putc(char c)
{
    g_framebuf[g_framepos++] = c;
}

static void fb_flush()
{
    fwrite(g_framebuf, 1, g_framepos, stdout);
    fflush(stdout);
}

static char glyph_for(char c, int texture)
{
    if (c == ' ') return ' ';
    if (c == 'X') return '+';
    if (c == '_') return '-';
    if (c == '|') return '|';
    if (c == PREVIEW_GLYPH) return ':';
    if (texture == 2) return ':';
    return c;
}

static const char* COLOR_TABLE[9][3][2] ={
    { {"\x1b[38;5;255m", "\x1b[38;5;254m"}, {"\x1b[38;5;250m", "\x1b[38;5;248m"}, {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;253m", "\x1b[38;5;252m"}, {"\x1b[38;5;245m", "\x1b[38;5;244m"}, {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;246m", "\x1b[38;5;244m"}, {"\x1b[38;5;240m", "\x1b[38;5;238m"}, {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;215m", "\x1b[38;5;214m"}, {"\x1b[38;5;173m", "\x1b[38;5;172m"}, {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;130m", "\x1b[38;5;94m"},  {"\x1b[38;5;94m",  "\x1b[38;5;58m"},  {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;34m",  "\x1b[38;5;40m"},  {"\x1b[38;5;28m",  "\x1b[38;5;22m"},  {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;195m", "\x1b[38;5;159m"}, {"\x1b[38;5;117m", "\x1b[38;5;116m"}, {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;223m", "\x1b[38;5;222m"}, {"\x1b[38;5;180m", "\x1b[38;5;179m"}, {"\x1b[38;5;60m", "\x1b[38;5;66m"} },
    { {"\x1b[38;5;39m",  "\x1b[38;5;38m"},  {"\x1b[38;5;31m",  "\x1b[38;5;30m"},  {"\x1b[38;5;60m", "\x1b[38;5;66m"} }
};

#define EDGE_COLOR "\x1b[38;5;233m"

static const char* pick_color(char c, int isTarget, float dist, int parity, int texture)
{
    if (c == 'X' || c == '_' || c == '|') return "\x1b[38;5;217m";
    if (c == PREVIEW_GLYPH) return g_previewColor;

    if (isTarget)
    {
        switch (c)
        {
            case '@': return "\x1b[38;5;255m";
            case '%': return "\x1b[38;5;254m";
            case STONE_CHAR:  return "\x1b[38;5;250m";
            case WOOD_CHAR:   return "\x1b[38;5;222m";
            case '=':         return "\x1b[38;5;173m";
            case LEAVES_CHAR: return "\x1b[38;5;46m";
            case GLASS_CHAR:  return "\x1b[38;5;195m";
            case SAND_CHAR:   return "\x1b[38;5;223m";
            case WATER_CHAR:  return "\x1b[38;5;45m";
            default:  return "\x1b[0m";
        }
    }

    if (texture == 2) return EDGE_COLOR;

    int idx = face_type_index(c);
    if (idx < 0) return "\x1b[0m";

    int tier = 0;
    if (dist > FOG_FAR) tier = 2;
    else if (dist > FOG_NEAR) tier = 1;

    return COLOR_TABLE[idx][tier][parity & 1];
}

void draw_crosshair(char** picture)
{
    int cx = crosshair_x();
    int cy = crosshair_y();
    picture[cy][cx] = 'X';

    if (cx - 2 >= 0) picture[cy][cx - 2] = '_';
    if (cx + 2 < X_PIXELS) picture[cy][cx + 2] = '_';

    if (cy - 1 >= 0) picture[cy - 1][cx] = '|';
    if (cy + 1 < Y_PIXELS) picture[cy + 1][cx] = '|';
}

int heading_degrees(float phi)
{
    float deg = fmodf(phi * (180.0f / 3.14159265f), 360.0f);
    if (deg < 0.0f) deg += 360.0f;
    return (int)(deg + 0.5f) % 360;
}

void draw_ascii(char** picture, int** highlight, float** dist, int** parity, int** texture,
                 player_pos_view posview, char place_char, int placed_count, int mined_count)
{
    fb_reset();
    fb_puts("\x1b[H");
    for (int i = 0; i < Y_PIXELS; i++)
    {
        const char* current = NULL;
        for (int j = 0; j < X_PIXELS; j++)
        {
            char c = picture[i][j];
            int tex = texture[i][j];
            const char* color = pick_color(c, highlight[i][j], dist[i][j], parity[i][j], tex);
            if (color != current)
            {
                fb_puts(color);
                current = color;
            }
            fb_putc(glyph_for(c, tex));
        }
        fb_puts("\x1b[0m\n");
    }

    char hud[128];
    snprintf(hud, sizeof(hud),
             "\x1b[KQ to quit  |  Block: %-6s  Pos: (%.0f, %.0f, %.0f)  Facing: %3d deg  |  Placed: %d  Mined: %d\n",
             block_name_for(place_char), posview.pos.x, posview.pos.y, posview.pos.z,
             heading_degrees(posview.view.phi), placed_count, mined_count);
    fb_puts(hud);

    fb_flush();
}