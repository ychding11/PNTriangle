#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "viewer.h"


int window_width = 1280, window_height = 720;

float tessellationLevel = 1.0f;
bool shouldTessellateModel = false;

bool shouldDisplayWireframeMode = false;

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
    }

    Viewer viewer{"Viewer", 1280, 720};
    viewer.SetModel(modelPath);
    viewer.Run();

    return 0;
}

