#include <Windows.h>


int CALLBACK WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nShowCmd
) {
    MessageBoxA(0, "Hello World", "Hello World", MB_OK|MB_ICONINFORMATION);

    return 0;
}