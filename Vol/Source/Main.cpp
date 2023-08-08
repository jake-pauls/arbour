#include "Windows/Win32Application.h"

#ifdef WIN32
_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE HInstance, HINSTANCE HPrevInstance, LPSTR LpCmdLine, int NCmdShow)
{
    const Win32Application* App = new Win32Application();
    is(App);

    int Result = App->Run(HInstance, NCmdShow);
    delete App;

    return Result;
}
#else
int main(int Argc, char* Argv[])
{ 
    return 0; 
}
#endif