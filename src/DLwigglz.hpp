
/*
  This file is covered by the LICENSING file in the root of this project.
*/

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

struct as_PJ301MPort : SVGPort {
	as_PJ301MPort() {
		setSVG(SVG::load(assetPlugin(plugin,"res/PJ301M.svg")));
	}
};

template <typename BASE>
struct GiantLight : BASE {
  GiantLight() {
    this->box.size = mm2px(Vec(18.0, 18.0));
  }
};
