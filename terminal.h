#pragma once
#include <windows.h>
extern HANDLE g_hOut;
extern HWND g_hwnd;
extern int g_screenCenterX, g_screenCenterY;

void die(const char* msg);
void init_terminal();
void restore_terminal();
void get_console_size(int* cols, int* rows);
