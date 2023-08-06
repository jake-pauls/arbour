#include "Application.h"

#if WIN32
_Use_decl_annotations_
int WINAPI WinMain(_In_ HINSTANCE HwndInstance, _In_opt_ HINSTANCE HwndPrevInstance, _In_ LPSTR LpCmdLine, _In_ int NShowCmd)
{
    const Application* VolApplication = new Application();
    is(VolApplication);

    VolApplication->Run();
    delete VolApplication;

    return 0;
}
#endif