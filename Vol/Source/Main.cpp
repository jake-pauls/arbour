#include "volpch.h"

#include "Application.h"

#ifdef WIN32
#include <Windows.h>
#endif

int main(int Argc, char* Argv[])
{
    const Application* VolApplication = new Application();

    VolApplication->Run();
    delete VolApplication;

    return 0;
}

#ifdef WIN32
int WinMain(_In_ HINSTANCE HwndInstance, _In_opt_ HINSTANCE HwndPrevInstance, _In_ LPSTR LpCmdLine, _In_ int NShowCmd)
{
    return main(__argc, __argv);
}
#endif