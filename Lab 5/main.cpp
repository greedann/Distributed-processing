#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cctype>

HWND hWndNextViewer;

bool isSimularToBankAccount(const char *str)
{
    if (strlen(str) != 9)
    {
        return false;
    }

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        hWndNextViewer = SetClipboardViewer(hwnd);
        break;

    case WM_DRAWCLIPBOARD:

        if (IsClipboardFormatAvailable(CF_TEXT))
        {
            if (OpenClipboard(hwnd))
            {
                HANDLE hClipboardData = GetClipboardData(CF_TEXT);
                if (hClipboardData)
                {
                    LPSTR clipboardData = (LPSTR)GlobalLock(hClipboardData);
                    if (clipboardData)
                    {
                        printf("copied: %s\n", clipboardData);
                        if (isSimularToBankAccount(clipboardData))
                        {
                            const char *output = "Do not copy bank account number!";
                            const size_t len = strlen(output) + 1;
                            HGLOBAL hGlMem = GlobalAlloc(GMEM_MOVEABLE, len);
                            LPVOID lpGlMem = GlobalLock(hGlMem);
                            memcpy(lpGlMem, output, len);
                            GlobalUnlock(hGlMem);

                            EmptyClipboard();
                            SetClipboardData(CF_TEXT, hGlMem);
                        }
                    }
                }
                CloseClipboard();
            }
        }
        SendMessage(hWndNextViewer, uMsg, wParam, lParam);
        break;

    case WM_DESTROY:
        ChangeClipboardChain(hwnd, hWndNextViewer);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main()
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ClipboardViewer";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}