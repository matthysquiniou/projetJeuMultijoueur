#include "Server.h"
#include "cpu-engine.h"
#include "App.h"

int WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    LibLoader::Loadlib();
    CPU_RUN(256,128,false,false);
    LibLoader::CleanLib();
}