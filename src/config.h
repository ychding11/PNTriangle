#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

 #define MSAA_ENABLE

struct RenderSetting
{
    // the input of current setting
    int  width = 1280;
    int  height = 720;
    bool enableTess{false};
    glm::vec4 outerTessLevel{2.f};
    glm::vec3 innerTessLevel{1.f};
};

struct DisplayOption
{
    bool wireframe;
    bool showUI;

    DisplayOption()
        : wireframe(false)
        , showUI(true)
    { }
};

RenderSetting& GetRenderSetting() ;

DisplayOption& GetDisplayOption() ;

