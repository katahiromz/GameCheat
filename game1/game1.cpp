#include <windows.h>
#include <stdio.h>

int score = 123456;

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    TCHAR szText[MAX_PATH];
    wsprintf(szText, TEXT("Your score is %d."), score);
    MessageBox(NULL, szText, TEXT("SCORE"), MB_ICONINFORMATION);
    return 0;
}
