#include <Windows.h>


#define internal static
#define global_variable static
#define local_persist static

global_variable bool running;
global_variable BITMAPINFO bitMapInfo;
global_variable void *BitMapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;


internal void
Win32ResizeDIBSection(int width, int height) {

    if(BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }
    if(!BitmapDeviceContext)
    {
        BitmapDeviceContext = CreateCompatibleDC(0);
    }

    bitMapInfo.bmiHeader.biSize = sizeof(bitMapInfo.bmiHeader);
    bitMapInfo.bmiHeader.biWidth = width;
    bitMapInfo.bmiHeader.biHeight = height;
    bitMapInfo.bmiHeader.biPlanes = 1;
    bitMapInfo.bmiHeader.biBitCount = 32;
    bitMapInfo.bmiHeader.biCompression = BI_RGB;

    BitmapHandle = CreateDIBSection(
            BitmapDeviceContext,
            &bitMapInfo,
            DIB_RGB_COLORS,
            &BitMapMemory,
            0,
            0
            );
    ReleaseDC(0, BitmapDeviceContext);
}

internal void
Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height) {
    StretchDIBits(deviceContext,
                  x, y, width, height,
                  x, y, width, height, BitMapMemory, &bitMapInfo, DIB_RGB_COLORS, SRCCOPY);
}


LRESULT CALLBACK Win32MainWindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

    LRESULT result = 0;
    switch (message) {
        case WM_SIZE: {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            INT width = clientRect.right - clientRect.left;
            INT height = clientRect.bottom - clientRect.top;
            Win32ResizeDIBSection(width, height);
            OutputDebugStringA("WM_SIZE\n");
            break;
        }
        case WM_DESTROY: {
            running = false;
            break;
        }
        case WM_CLOSE: {
            PostQuitMessage(0);
            running = false;
            break;
        }
        case WM_ACTIVATEAPP: {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT Paint;
            HDC deviceContext = BeginPaint(window, &Paint);
            INT x = Paint.rcPaint.left;
            INT y = Paint.rcPaint.top;
            INT width = Paint.rcPaint.right - Paint.rcPaint.left;
            INT height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            Win32UpdateWindow(deviceContext, x, y, width, height);
            EndPaint(window, &Paint);
            break;
        }
        default:
            // OutputDebugStringA("Test");
            result = DefWindowProc(window, message, wparam, lparam);
    }
    return result;

}

int CALLBACK WinMain(HINSTANCE Instance,
                     HINSTANCE PrevInstance,
                     LPSTR CommandLine,
                     int ShowCode
) {

    WNDCLASS WndClass = {};
    WndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = &Win32MainWindowCallback;
    WndClass.hInstance = Instance;
    WndClass.lpszClassName = "HandmadeHeroWindowClass";

    if (RegisterClass(&WndClass)) {
        HWND WindowHandle = CreateWindowEx(
                0,
                WndClass.lpszClassName,
                "Handmade Hero",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);

        if (WindowHandle) {
            running = true;
            MSG message;
            while (running) {
                BOOL messageResult = GetMessage(&message, 0, 0, 0);
                if (messageResult > 0) {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                } else {
                    break;
                }
            }

        } else {
            //TODO:More logging
        }
    } else {
        // TODO: One day to log a failure in the class registration logic
    }

    return 0;


}