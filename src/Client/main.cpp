#include <cpu-engine.h>
#include <LibLoader.h>
#include "App.h"

int WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    LibLoader::Loadlib();
    CPU_RUN(800, 450, false, true);
    LibLoader::CleanLib();
}