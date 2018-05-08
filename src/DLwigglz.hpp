
#include "rack.hpp"

#pragma once

using namespace rack;

extern Plugin *plugin;
extern Model *modelR4xH4x;

// MODULE COMPONENTS (thanks, AS!!!)
struct BigLEDBezel : SVGSwitch, MomentarySwitch {
  BigLEDBezel() {
    addFrame(SVG::load(assetPlugin(plugin, "res/bigLEDBezel.svg")));
  }
};

template <typename BASE>
struct GiantLight : BASE {
  GiantLight() {
    this->box.size = mm2px(Vec(18.0, 18.0));
  }
};
