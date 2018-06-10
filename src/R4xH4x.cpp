
#include "DLwigglz.hpp"

using namespace rack;

#include "dsp/digital.hpp"
#include "osdialog.h"
#include "unistd.h"
#include "window.hpp"
// #include <chrono>
// #include <thread>

struct R4xH4x: Module {
	enum ParamIds {
		MOMENTARY_SWITCH_1,
		MOMENTARY_SWITCH_2,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		MOMENTARY_LED_1,
		MOMENTARY_LED_2,
		NUM_LIGHTS
	};

	SchmittTrigger btnTrigger1;
	SchmittTrigger btnTrigger2;

	const float lightLambda = 0.075f;
	float resetLight1 = 0.0f;
	float resetLight2 = 0.0f;

	R4xH4x() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
	void catalog();
	void patch();
};

void R4xH4x::catalog() { // serialize installed plugin(s)/modules(s) catalog as catalog.json
	// int clear = 0; // periodically clear rendered moduleWidgets
	// std::vector<ModuleWidget*> moduleWidgets; // cache for width lookup
	Vec windowSize = windowGetWindowSize();
	info("Generating partial catalog for installed plugin models (missing module widths)");
	json_t *metaj = json_object(); // meta as json, plugins as json, etc
	json_t *psj = json_array();
	json_object_set_new(metaj, "applicationName", json_string(gApplicationName.c_str()));
	json_object_set_new(metaj, "applicationVersion", json_string(gApplicationVersion.c_str()));
	json_object_set_new(metaj, "apiHost", json_string(gApiHost.c_str()));
	json_object_set_new(metaj, "width", json_integer(windowSize.x));
	json_object_set_new(metaj, "height", json_integer(windowSize.y));
	json_object_set_new(metaj, "token", json_string(gToken.c_str()));
	json_object_set_new(metaj, "path", json_string(getcwd(NULL, 0)));
	json_object_set_new(metaj, "pluginCount", json_integer(gPlugins.size()));
	for (Plugin *plugin : gPlugins) {
		json_t *pj = json_object();
		json_object_set_new(pj, "slug", json_string(plugin->slug.c_str()));
		json_object_set_new(pj, "path", json_string(plugin->path.c_str()));
		json_object_set_new(pj, "version", json_string(plugin->version.c_str()));
		json_object_set_new(pj, "modelCount", json_integer(plugin->models.size()));
		json_t *msj = json_array();
		json_object_set_new(pj, "models", msj);
		for (Model *model : plugin->models) {
			// info("*** plugin module %d, %s from %s ***", clear++, model->slug.c_str(), plugin->slug.c_str());
			json_t *mj = json_object();
			json_object_set_new(mj, "slug", json_string(model->slug.c_str()));
			json_object_set_new(mj, "name", json_string(model->name.c_str()));
			json_object_set_new(mj, "author", json_string(model->author.c_str()));
			json_t *tsj = json_array();
			for (ModelTag tag : model->tags) {
				json_array_append(tsj, json_string(gTagNames[tag].c_str()));
			}
			json_object_set_new(mj, "tags", tsj);
			// json_object_set_new(mj, "width", json_integer(69)); // stub JIC

			// // can't instantiate ModuleWidgets outside main Rack thread in 0.6.x!
			// moduleWidgets.push_back(model->createModuleWidgetNull());
			// json_object_set_new(mj, "width", json_integer((int) moduleWidgets.back()->box.size.x));
			json_array_append(msj, mj);
			// if (clear % 10 == 0) {
			// 	moduleWidgets.clear(); info(" *** cleaning house %d ***", clear);
			// 	// std::this_thread::sleep_for(std::chrono::seconds(3)); // allow some for things to settle -- h4x!
			// }
		}
		json_array_append(psj, pj);
	}
	json_object_set_new(metaj, "plugins", psj);
	// moduleWidgets.clear(); info(" *** cleaning house %d, final ***", clear);

	// std::this_thread::sleep_for(std::chrono::seconds(5)); // allow some for things to settle -- h4x!
	info("Saving partial catalog as plugins/catalog.partial.json, use rackcli "
		"--cloud to add widths and generate full catalog.json");
	FILE *file = fopen(assetLocal("catalog.partial.json").c_str(), "w");
	if (!file) return;
	json_dumpf(metaj, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
	json_decref(metaj);
	fclose(file);
}

void R4xH4x::patch() { // serialize current patch as patch.json w/ widths
	json_t *rootJ = gRackWidget->toJson(); // get patch JSON

	Vec windowSize = windowGetWindowSize();
	json_object_set_new(rootJ, "width", json_integer(windowSize.x));
	json_object_set_new(rootJ, "height", json_integer(windowSize.y));

	// last mouse position
	//   unfortunately will be where patch button was clicked, for reference only
	//   use fork File.Save for real position since we can't assign custom
	//     keyboard chords to plugin modules
	json_t *posJ = json_pack("[i, i]",
		(int) gRackWidget->lastMousePos.x, (int) gRackWidget->lastMousePos.y);
	json_object_set_new(rootJ, "lastMousePos", posJ);

	std::vector<std::vector<int>> xy; // cache child (width, height) from patch
	for (Widget *w : gRackWidget->moduleContainer->children) {
		ModuleWidget *moduleWidget = dynamic_cast<ModuleWidget*>(w);
		assert(moduleWidget);
		xy.push_back({(int) moduleWidget->box.size.x, (int) moduleWidget->box.size.y});
	}
	size_t index;
	json_t *module; // inject missing width/height into each module's json
	json_array_foreach(json_object_get(rootJ, "modules"), index, module) {
		json_object_set(module, "width", json_integer(xy.at(index)[0]));
		json_object_set(module, "height", json_integer(xy.at(index)[1]));
	}
	// info("%s", json_dumps(rootJ, JSON_INDENT(2) | JSON_REAL_PRECISION(9)));
	info("Saving width-augmented patch to patch.json");
	FILE *file = fopen(assetLocal("patch.json").c_str(), "w");
	if (!file) return;
	json_dumpf(rootJ, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
	json_decref(rootJ);
	fclose(file);
}

void R4xH4x::step() {
	// catalog, top button
	if (btnTrigger1.process(params[MOMENTARY_SWITCH_1].value)) {
		resetLight1 = 1.0;
		this->catalog();
	}
	resetLight1 -= resetLight1 / lightLambda / engineGetSampleRate();
	lights[MOMENTARY_LED_1].value = resetLight1;

	// patch, bottom button
	if (btnTrigger2.process(params[MOMENTARY_SWITCH_2].value)) {
		resetLight2 = 1.0;
		this->patch();
	}
	resetLight2 -= resetLight2 / lightLambda / engineGetSampleRate();
	lights[MOMENTARY_LED_2].value = resetLight2;
}

struct R4xH4xWidget : ModuleWidget {
	R4xH4xWidget(R4xH4x *module);
};

R4xH4xWidget::R4xH4xWidget(R4xH4x *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/R4xH4x.svg")));

	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	static const float led_offset = 3.3;
	static const float led_center = 15;
	static const float y_base = 75;
	static const float y_offset = 100;

	// catalog, top button
	addParam(ParamWidget::create<BigLEDBezel>(Vec(led_center, y_base), module, R4xH4x::MOMENTARY_SWITCH_1, 0.0, 1.0, 0.0));
	addChild(ModuleLightWidget::create<GiantLight<RedLight>>(Vec(led_center+led_offset, y_base+led_offset), module, R4xH4x::MOMENTARY_LED_1));

	// patch, bottom button
	addParam(ParamWidget::create<BigLEDBezel>(Vec(led_center, y_base+y_offset), module, R4xH4x::MOMENTARY_SWITCH_2, 0.0, 1.0, 0.0));
	addChild(ModuleLightWidget::create<GiantLight<RedLight>>(Vec(led_center+led_offset, y_base+led_offset+y_offset), module, R4xH4x::MOMENTARY_LED_2));
}

Model *modelR4xH4x = Model::create<R4xH4x, R4xH4xWidget>("DLwigglz", "DWLwigglz-r4xH4x", "r4xH4x", UTILITY_TAG);

// thanks for the boost, AS!!!
