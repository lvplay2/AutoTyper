#define UNICODE
#define _UNICODE
#include <windows.h>
#include <string>
#include <vector>
#include <thread>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TypeText();

HWND hTextBox, hIntervalBox, hStartButton, hStopButton;
bool running = false;
int interval = 1000;
std::wstring text;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"AutoTyperClass";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Auto Typer", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, nullptr, nullptr, hInstance, nullptr);
    
    if (!hwnd) return 0;
    
    ShowWindow(hwnd, nCmdShow);
    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            hTextBox = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL,
                           10, 10, 400, 200, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            
            CreateWindowW(L"STATIC", L"Interval", WS_CHILD | WS_VISIBLE,
                         10, 220, 50, 20, hwnd, NULL, NULL, NULL);
            hIntervalBox = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"1000", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                                          70, 220, 50, 20, hwnd, NULL, NULL, NULL);
            
            hStartButton = CreateWindowW(L"BUTTON", L"Start (F9)", WS_CHILD | WS_VISIBLE,
                                        140, 220, 100, 30, hwnd, NULL, NULL, NULL);
            hStopButton = CreateWindowW(L"BUTTON", L"Stop (F9)", WS_CHILD | WS_VISIBLE,
                                       250, 220, 100, 30, hwnd, NULL, NULL, NULL);
            
            RegisterHotKey(hwnd, 1, 0, VK_F9);
            return 0;
        }
        case WM_HOTKEY:
            if (wParam == 1) {
                running = !running;
                if (running) {
                    wchar_t buffer[1024];
                    GetWindowTextW(hTextBox, buffer, 1024);
                    text = buffer;
                    wchar_t intervalBuffer[10];
                    GetWindowTextW(hIntervalBox, intervalBuffer, 10);
                    interval = _wtoi(intervalBuffer);
                    std::thread(TypeText).detach();
                }
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void TypeText() {
    size_t index = 0;
    while (running && index < text.length()) {
        wchar_t ch = text[index++];
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;
        input.ki.wScan = ch;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &input, sizeof(INPUT));
        
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
        
        Sleep(interval);
    }
    running = false;
}
