#include "input.h"
#include "terminal.h"
#include <windows.h>
#include <ctype.h>
//Returns non-zero if the key is down, 0 otherwise
int is_vk_pressed(int vk)//Check if a virtual key is currently pressed
{
    return (GetAsyncKeyState(vk) & 0x8000) != 0;//Returns non-zero if the key is down, 0 otherwise
}


int is_key_pressed(char key)//Check if a character key is currently pressed
{
    //Handles space key specially, converts other keys to uppercase
    int vk = (key == ' ') ? VK_SPACE : toupper((unsigned char)key);

    return is_vk_pressed(vk);
}


//Used to avoid capturing input when window isnot focused
int has_focus()//Check if our console window has focus
{
    //Returns 1 if active, 0 if not 
    if (!g_hwnd) return 1; // Fallback is assuming active if no window handle
    return GetForegroundWindow() == g_hwnd;
}