#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>

HANDLE g_hOut;
static DWORD g_origMode;
HWND g_hwnd;
int g_screenCenterX, g_screenCenterY;

void die(const char* msg) 
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void init_terminal() 
{
    g_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(g_hOut, &g_origMode);
    SetConsoleMode(g_hOut, g_origMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    g_hwnd = GetConsoleWindow();
    g_screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
    g_screenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2;
    SetCursorPos(g_screenCenterX, g_screenCenterY);

    ShowCursor(FALSE);
    printf("\x1b[?25l\x1b[2J\x1b[H"); 
    fflush(stdout);
}

void restore_terminal() 
{
    ShowCursor(TRUE);
    printf("\x1b[?25h\x1b[0m\x1b[2J\x1b[H");
    fflush(stdout);
    SetConsoleMode(g_hOut, g_origMode);
}

void get_console_size(int* cols, int* rows) 
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(g_hOut, &csbi)) 
    {
        *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } 
    else 
    {
        *cols = 120;
        *rows = 40;
    }
}
