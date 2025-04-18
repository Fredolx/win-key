#include <windows.h>
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_TOGGLE 1001
#define ID_TRAY_QUIT   1002

HINSTANCE hInst;
NOTIFYICONDATA nid = {};
HMENU hMenu;
bool isOn = false;
HHOOK keyboardHook;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKbd = (KBDLLHOOKSTRUCT*)lParam;
        if (pKbd->vkCode == VK_LWIN) {
            return 1; 
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void UpdateTrayIcon() {
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(isOn ? IDI_ICON_ON : IDI_ICON_OFF));
    Shell_NotifyIcon(NIM_MODIFY, &nid);
    ModifyMenu(hMenu, ID_TRAY_TOGGLE, MF_BYCOMMAND | MF_STRING,
               ID_TRAY_TOGGLE, isOn ? L"Enable Win Key" : L"Disable Win Key");
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
            if (isOn) {
                keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
            }
            else {
                UnhookWindowsHookEx(keyboardHook);
            }
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

bool InstanceAlreadyRunning() {
    HANDLE hMutex = CreateMutex(NULL, TRUE, L"dev.fredol.winkey");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }
    else return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    if (InstanceAlreadyRunning()) {
        return 1;
    }
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
    nid.uFlags = NIF_MESSAGE | NIF_ICON;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_OFF));
    Shell_NotifyIcon(NIM_ADD, &nid);
    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_TOGGLE, L"Disable Win Key");
    AppendMenu(hMenu, MF_STRING, ID_TRAY_QUIT, L"Quit");
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}


