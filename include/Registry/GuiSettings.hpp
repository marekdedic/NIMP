#pragma once

#include "Global.hpp"

class GuiSettings
{
public:
    std::map<std::string, QColor> palette;
    std::map<std::string, float> dimensions;
    GuiSettings();
};