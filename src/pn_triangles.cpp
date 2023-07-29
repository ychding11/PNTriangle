#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "viewer.h"

//< When the Nvidia and AMD drivers see their respective symbol exported and set to nonzero in a program, 
//< they will take precedence over the integrated GPU when creating the OpenGL context.
#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

int main(int argc, char **argv)
{
    std::string modelPath;
    char **filename = nullptr; 
    for (int i = 0; i < argc; ++i)
    {
        if (i + 1 < argc && strcmp(argv[i], "-obj") == 0)
            filename = argv + i + 1, ++i;
        else
        {
        }
    }

    if ( filename != nullptr )
    {
        modelPath = *filename;
    }
    else
    {
        modelPath = "Model/Suzanne.obj";
       // modelPath = "Model/bunny.obj";
    }

    Viewer viewer{"Viewer", 1280, 720};
    viewer.SetModel(modelPath);
    viewer.Run();

    return 0;
}

