
/*
  This file is covered by the LICENSING file in the root of this project.
*/

// gl0bz - actuate global cable opacity, tension), and rack zoom

#include "DLwigglz.hpp"

using namespace rack;

#include "dsp/digital.hpp"

struct Gl0bz: Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		CV_INPUT1,
		CV_INPUT2,
		CV_INPUT3,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	float opacity = gToolbar->wireOpacitySlider->value;
	float tension = gToolbar->wireTensionSlider->value;
	float zoom = gRackScene->zoomWidget->zoom;
	int zoomFrequency = (int) engineGetSampleRate() / 60, zooms = 1;

	Gl0bz() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

template<typename tVal>
tVal map_value(std::pair<tVal,tVal> a, std::pair<tVal, tVal> b, tVal inVal) {
  tVal inValNorm = inVal - a.first;
  tVal aUpperNorm = a.second - a.first;
  tVal normPosition = inValNorm / aUpperNorm;

  tVal bUpperNorm = b.second - b.first;
  tVal bValNorm = normPosition * bUpperNorm;
  tVal outVal = b.first + bValNorm;

  return outVal;
}
std::pair<float,float> cvOpacityRange(0.0,10.0), opacityRange(0.0,100.0);
std::pair<float,float> cvTensionRange(0,10), tensionRange(0,1);
std::pair<float,float> cvZoomRange(0,10), zoomRange(0.25,2);

void Gl0bz::step() {
	if (inputs[CV_INPUT1].active) {
		opacity = map_value(cvOpacityRange, opacityRange, inputs[CV_INPUT1].value);
		if (opacity != gToolbar->wireOpacitySlider->value) {
			gToolbar->wireOpacitySlider->value = opacity;
		}
	}

	if (inputs[CV_INPUT2].active) {
		tension = map_value(cvTensionRange, tensionRange, inputs[CV_INPUT2].value);
		if (tension != gToolbar->wireTensionSlider->value) {
			gToolbar->wireTensionSlider->value = tension;
		}
	}

	if (inputs[CV_INPUT3].active) {
		if (++zooms % zoomFrequency == 0) {
			zoom = map_value(cvZoomRange, zoomRange, inputs[CV_INPUT3].value);
			if  (zoom != gRackScene->zoomWidget->zoom) {
				gRackScene->zoomWidget->setZoom(clamp((float) zoom, 0.25f, 4.0f));
				gToolbar->zoomSlider->setValue(zoom * 100.0);
				zooms = 1;
			}
		}
	}
}

struct Gl0bzWidget : ModuleWidget {
	Gl0bzWidget(Gl0bz *module);
};

int base = 50, offset = 65;

Gl0bzWidget::Gl0bzWidget(Gl0bz *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/3hp.svg")));

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

	auto inaInputPosition1 = Vec(10.5, base);
	addInput(Port::create<PJ301MPort>(inaInputPosition1, Port::INPUT, module, Gl0bz::CV_INPUT1));

	auto inaInputPosition2 = Vec(10.5, base + offset);
	addInput(Port::create<PJ301MPort>(inaInputPosition2, Port::INPUT, module, Gl0bz::CV_INPUT2));

	auto inaInputPosition3 = Vec(10.5, base + offset * 2);
	addInput(Port::create<PJ301MPort>(inaInputPosition3, Port::INPUT, module, Gl0bz::CV_INPUT3));
}

Model *modelGl0bz = Model::create<Gl0bz, Gl0bzWidget>("DLwigglz", "DLwigglz-gl0bz", "gl0bz", UTILITY_TAG);

// thanks for the boost, bogaudio!!!
