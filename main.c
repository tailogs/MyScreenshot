#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <strsafe.h>  // For wide string handling

#define HOTKEY_ID 1
#define TRAY_ICON_ID 1
#define WM_TRAY_ICON WM_APP + 1
#define IDI_APPICON 101 // Идентификатор нашей иконки из rc файла

// Глобальные переменные
HINSTANCE g_hInst;
HWND g_hwnd;
LPCSTR g_szClassName = "MyTrayApp";
NOTIFYICONDATA nid = { 0 }; // Глобальная переменная

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Screenshot function
int CaptureScreenshotAndCopyToClipboard() {
    GdiplusStartupInput gdiPlusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiPlusStartupInput, NULL);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreenDC = GetDC(NULL);
    if (!hScreenDC) {
        GdiplusShutdown(gdiplusToken);
        return 1; // Не удалось получить контекст экрана
    }

    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return 1; // Не удалось создать совместимый DC
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    if (!hBitmap) {
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return 1; // Не удалось создать битмап
    }

    SelectObject(hMemoryDC, hBitmap);

    // Захват экрана
    if (!BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY)) {
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return 1; // Ошибка при копировании данных на битмап
    }

    // Копирование в буфер обмена
    if (!OpenClipboard(NULL)) {
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return 1; // Не удалось открыть буфер обмена
    }

    EmptyClipboard();
    if (!SetClipboardData(CF_BITMAP, hBitmap)) {
        CloseClipboard();
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return 1; // Ошибка при записи в буфер обмена
    }

    CloseClipboard();

    // Очистка ресурсов
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
    GdiplusShutdown(gdiplusToken);

    return 0; // Успешный захват и запись в буфер обмена
}

// Function to add the tray icon
void AddTrayIcon(HWND hwnd) {
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_TRAY_ICON;
    nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_APPICON)); // Устанавливаем кастомную иконку

    // Use char strings for the tooltip
    StringCchCopyA(nid.szTip, ARRAYSIZE(nid.szTip), "Screenshot in Tray");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

// Function to create the context menu
void CreateContextMenu(HWND hwnd) {
    HMENU hMenu = CreatePopupMenu();
    AppendMenuA(hMenu, MF_STRING, 2, "Exit");

    POINT p;
    GetCursorPos(&p);

    // Show the context menu
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, NULL);

    // Clean up
    DestroyMenu(hMenu);
}


// Главная функция
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    g_hInst = hInstance;

    WNDCLASSEXA wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));  // Загружаем иконку
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON)); // Загружаем иконку для маленького отображения

    if (!RegisterClassExA(&wc)) {
        MessageBoxA(NULL, "Failed to register window class!", "Error!", MB_ICONERROR);
        return 1;
    }


    g_hwnd = CreateWindowExA(
        0,
        g_szClassName,
        "My Tray Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hwnd) {
        MessageBoxA(NULL, "Failed to create window!", "Error!", MB_ICONERROR);
        return 1;
    }

    AddTrayIcon(g_hwnd);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
       // Register hotkey here
        if (!RegisterHotKey(hwnd, HOTKEY_ID, MOD_CONTROL | MOD_SHIFT, 0x53)) // 0x53 is 'S' key
        {
            MessageBoxA(hwnd, "Failed to register hotkey!", "Error!", MB_ICONERROR);
        }
        return 0;

    case WM_HOTKEY:
    // Handle hotkey press
        if (wParam == HOTKEY_ID) {
            CaptureScreenshotAndCopyToClipboard();
        }
        return 0;

    case WM_TRAY_ICON:
        if (lParam == WM_RBUTTONDOWN) {
            CreateContextMenu(hwnd);
        }
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 2) {
            DestroyWindow(hwnd);
        }
        return 0;

    case WM_DESTROY:
        // Unregister hotkey here
        UnregisterHotKey(hwnd, HOTKEY_ID);
        Shell_NotifyIcon(NIM_DELETE, &nid); // Удаляем значок из трея
        PostQuitMessage(0);
        return 0;
    
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
