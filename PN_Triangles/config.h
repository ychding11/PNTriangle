
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>



struct RenderSetting
{
    // the input of current setting
    int  onlyUseForIndentification;
    int  width;
    int  height;
    int  bvhBuilderType;
    bool enableTess;
};

struct DisplayOption
{
    bool wireframe;

    DisplayOption()
        : wireframe(false)
    { }
};

RenderSetting& GetRenderSetting() ;

DisplayOption& GetDisplayOption() ;

