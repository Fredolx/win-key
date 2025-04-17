#include <windows.h>
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_TOGGLE 1001
#define ID_TRAY_QUIT   1002

HINSTANCE hInst;
NOTIFYICONDATA nid = {};
HMENU hMenu;
bool isOn = false;

void UpdateTrayIcon() {
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(isOn ? IDI_ICON_ON : IDI_ICON_OFF));
    Shell_NotifyIcon(NIM_MODIFY, &nid);
    ModifyMenu(hMenu, ID_TRAY_TOGGLE, MF_BYCOMMAND | MF_STRING,
               ID_TRAY_TOGGLE, isOn ? L"Toggle off" : L"Toggle on");
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_TOGGLE:
            isOn = !isOn;
            UpdateTrayIcon();
            break;
        case ID_TRAY_QUIT:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    hInst = hInstance;
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TrayIconApp";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, L"", 0,
                             0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_ON));
    Shell_NotifyIcon(NIM_ADD, &nid);

    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_TOGGLE, L"Toggle on");
    AppendMenu(hMenu, MF_STRING, ID_TRAY_QUIT, L"Quit");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
