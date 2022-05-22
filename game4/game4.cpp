#include <windows.h>
#include <stdio.h>
#include <string>

int main(void)
{
    for (;;)
    {
        printf("0x%08lX\r", GetTickCount());
    }
    return 0;
}
