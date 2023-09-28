#include "Windows/Win32Application.h"

#ifdef WIN32
_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const Win32Application* app = new Win32Application();
    is(app);

    int Result = app->Run(hInstance, nCmdShow);
    delete app;

    return Result;
}
#else
int main(int argc, char* argv[])
{ 
    return 0; 
}
#endif