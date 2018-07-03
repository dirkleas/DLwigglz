
/*
  This file is covered by the LICENSING file in the root of this project.
*/

#include "DLwigglz.hpp"

Plugin *plugin;

void init(Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);
	p->addModel(modelR4xH4x);
}
