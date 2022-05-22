#include <windows.h>
#include <stdio.h>
#include <string>

int main(void)
{
    for (;;)
    {
        printf("0x%08lX\r", GetTickCount());
        Sleep(100);
    }
    return 0;
}
