#include <windows.h>
#include <stdio.h>
#include <string>
#include <imagehlp.h>

int score = 123456;

BOOL checkCheckSum(HINSTANCE hInstance)
{
    WCHAR szBuff[MAX_PATH];
    GetModuleFileNameW(hInstance, szBuff, MAX_PATH);

    FILE *fp = _wfopen(szBuff, L"rb");
    if (!fp)
        return FALSE;

    std::string contents;
    CHAR buf[MAX_PATH];
    for (;;)
    {
        size_t cb = fread(buf, 1, _countof(buf), fp);
        if (!cb)
            break;
        contents.append(buf, cb);
    }

    fclose(fp);

    IMAGE_NT_HEADERS *nt;
    DWORD header_sum, checksum;
    nt = CheckSumMappedFile(&contents[0], contents.size(), &header_sum, &checksum);
    if (nt == NULL)
    {
        return FALSE;
    }

    IMAGE_OPTIONAL_HEADER32 *opt32 = NULL;
    IMAGE_OPTIONAL_HEADER64 *opt64 = NULL;
    switch (nt->FileHeader.SizeOfOptionalHeader)
    {
    case IMAGE_SIZEOF_NT_OPTIONAL32_HEADER:
        opt32 = (IMAGE_OPTIONAL_HEADER32*)&nt->OptionalHeader;
        break;
    case IMAGE_SIZEOF_NT_OPTIONAL64_HEADER:
        opt64 = (IMAGE_OPTIONAL_HEADER64*)&nt->OptionalHeader;
        break;
    default:
        return FALSE;
    }

    if (opt32 && opt32->CheckSum != checksum)
        return FALSE;
    if (opt64 && opt64->CheckSum != checksum)
        return FALSE;
    return TRUE;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    if (!checkCheckSum(hInstance))
    {
        MessageBox(NULL, TEXT("CheckSum mismatch"), TEXT("ERROR"), MB_ICONERROR);
        return -1;
    }

    TCHAR szText[MAX_PATH];
    wsprintf(szText, TEXT("Your score is %d."), score);
    MessageBox(NULL, szText, TEXT("SCORE"), MB_ICONINFORMATION);
    return 0;
}
