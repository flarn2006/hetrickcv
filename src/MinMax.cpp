#include "HetrickCV.hpp"

struct MinMax : Module
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
        IN1_INPUT,
        IN2_INPUT,
        IN3_INPUT,
        IN4_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
        MIN_OUTPUT,
        MAX_OUTPUT,
		NUM_OUTPUTS
    };

    enum LightIds
    {
        MIN_POS_LIGHT,
        MIN_NEG_LIGHT,
		MAX_POS_LIGHT,
        MAX_NEG_LIGHT,
        NUM_LIGHTS
	};

	MinMax()
	{
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs &args) override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - reset, randomize: implements special behavior when user clicks these from the context menu
};


void MinMax::process(const ProcessArgs &args)
{
    auto in1 = inputs[IN1_INPUT].getVoltage();
    auto in2 = inputs[IN2_INPUT].isConnected() ? inputs[IN2_INPUT].getVoltage() : in1;
    auto in3 = inputs[IN3_INPUT].isConnected() ? inputs[IN3_INPUT].getVoltage() : in2;
    auto in4 = inputs[IN4_INPUT].isConnected() ? inputs[IN4_INPUT].getVoltage() : in3;

    auto max1 = std::max(in1, in2);
    auto max2 = std::max(in3, in4);
    auto totalMax = std::max(max1, max2);

    auto min1 = std::min(in1, in2);
    auto min2 = std::min(in3, in4);
    auto totalMin = std::min(min1, min2);

    outputs[MIN_OUTPUT].setVoltage(totalMin);
    outputs[MAX_OUTPUT].setVoltage(totalMax);
}

struct MinMaxWidget : ModuleWidget { MinMaxWidget(MinMax *module); };

MinMaxWidget::MinMaxWidget(MinMax *module)
{
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MinMax.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

    //////PARAMS//////

    //////INPUTS//////
    const int inSpacing = 42;
    const int jackX = 16;

    for(int i = 0; i < MinMax::NUM_INPUTS; i++)
    {
        addInput(createInput<PJ301MPort>(Vec(jackX, 54 + (i*inSpacing)), module, MinMax::IN1_INPUT + i));
    }

    //////OUTPUTS//////
    addOutput(createOutput<PJ301MPort>(Vec(jackX, 227), module, MinMax::MAX_OUTPUT));
    addOutput(createOutput<PJ301MPort>(Vec(jackX, 269), module, MinMax::MIN_OUTPUT));

    //////BLINKENLIGHTS//////
    //addChild(createLight<SmallLight<RedLight>>(Vec(lightPos, 158), module, MinMax::OR_LIGHT));
    //addChild(createLight<SmallLight<RedLight>>(Vec(lightPos, 203), module, MinMax::NOR_LIGHT));
    //addChild(createLight<SmallLight<RedLight>>(Vec(lightPos, 248), module, MinMax::TRIG_LIGHT));
}

Model *modelMinMax = createModel<MinMax, MinMaxWidget>("MinMax");
