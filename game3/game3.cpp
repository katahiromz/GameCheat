#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

HINSTANCE g_hInst = NULL;
HWND g_hMainWnd = NULL;
HBITMAP g_hbm = NULL;

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    g_hbm = LoadBitmap(g_hInst, MAKEINTRESOURCE(1));
    SendDlgItemMessage(hwnd, stc1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hbm);
    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}

void OnDestroy(HWND hwnd)
{
    DeleteObject(g_hbm);
    g_hbm = NULL;
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
#ifdef NDEBUG
    if (IsDebuggerPresent())
        return -1;
#endif

    g_hInst = hInstance;
    InitCommonControls();
    DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
    return 0;
}
