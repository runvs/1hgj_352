#include "game_properties.hpp"
#include <fstream>
#include <sstream>

jt::Palette GP::getPalette()
{
    std::ifstream file { "assets/palette.gpl" };
    std::stringstream buffer;
    buffer << file.rdbuf();
    auto palette_gpl = jt::Palette::parseGPL(buffer.str());
    return palette_gpl;
}

int GP::PhysicVelocityIterations() { return 20; }

int GP::PhysicPositionIterations() { return 20; }
