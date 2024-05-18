/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CarsonCPFinalBitCrusherAudioProcessorEditor::CarsonCPFinalBitCrusherAudioProcessorEditor (CarsonCPFinalBitCrusherAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //setSize (400, 300);

    setSize(450, 300);
    // Setup your sliders and other gui components - - - -
    auto& params = processor.getParameters();


    // Low Shelf dB Slider
    juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
    RateSlider.setBounds(0, 100, 150, 120);
    RateSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    RateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    RateSlider.setRange(audioParam->range.start, audioParam->range.end);
    RateSlider.setValue(*audioParam);
    RateSlider.addListener(this);
    addAndMakeVisible(RateSlider);

    // High Shelf dB Slider
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
    BitsSlider.setBounds(295, 100, 150, 120);
    BitsSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    BitsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    BitsSlider.setRange(audioParam->range.start, audioParam->range.end);
    BitsSlider.setValue(*audioParam);
    BitsSlider.addListener(this);
    addAndMakeVisible(BitsSlider);

    //// Mid Notch dB Slider
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
    LPFSlider.setBounds(150, 60, 150, 50);
   // LPFSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    LPFSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    LPFSlider.setRange(audioParam->range.start, audioParam->range.end);
    LPFSlider.setValue(*audioParam);
    LPFSlider.addListener(this);
    addAndMakeVisible(LPFSlider);

    //// Mid Notch dB Slider
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
    HPFSlider.setBounds(150, 140, 150, 50);
    //HPFSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    HPFSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    HPFSlider.setRange(audioParam->range.start, audioParam->range.end);
    HPFSlider.setValue(*audioParam);
    HPFSlider.addListener(this);
    addAndMakeVisible(HPFSlider);

    // Mid Notch dB Slider
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
    DryWetSlider.setBounds(150, 220, 150, 50);
    //DryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    DryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    DryWetSlider.setRange(audioParam->range.start, audioParam->range.end);
    DryWetSlider.setValue(*audioParam);
    DryWetSlider.addListener(this);
    addAndMakeVisible(DryWetSlider);
}

CarsonCPFinalBitCrusherAudioProcessorEditor::~CarsonCPFinalBitCrusherAudioProcessorEditor()
{
    

}
void CarsonCPFinalBitCrusherAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    auto& params = processor.getParameters();

    if (slider == &RateSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *audioParam = RateSlider.getValue();
        DBG("Low dB Slider Changed");
    }
    else if (slider == &BitsSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
        *audioParam = BitsSlider.getValue();
        DBG("High dB Slider Changed");
    }
    else if (slider == &LPFSlider) {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
        *audioParam = LPFSlider.getValue();
        DBG("Low Pass Slider Changed");
    }
    else if (slider == &HPFSlider) {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
        *audioParam = HPFSlider.getValue();
        DBG("High Pass Slider Changed");
    }
    else if (slider == &DryWetSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
        *audioParam = DryWetSlider.getValue();
        DBG("Mid dB Slider Changed");
    }


}
//==============================================================================
void CarsonCPFinalBitCrusherAudioProcessorEditor::paint (juce::Graphics& g)
{
    //g.fillAll(juce::Colours::darkblue);

    g.setColour(juce::Colours::ghostwhite);
    g.setFont(30.0f);


    g.drawSingleLineText("Bit Crusher", 155, 30, juce::Justification::left);

    g.setFont(25.0f);

    g.drawSingleLineText("Rate", 50, 95, juce::Justification::left);
    //g.drawSingleLineText("", 200, 95, juce::Justification::left);
    g.drawSingleLineText("Bits", 350, 95, juce::Justification::left);

    g.setFont(20.0f);
    g.drawSingleLineText("LPF", 207, 60, juce::Justification::left);
    g.drawSingleLineText("HPF", 207, 140, juce::Justification::left);
    g.drawSingleLineText("Wet%", 201, 220, juce::Justification::left);
}

void CarsonCPFinalBitCrusherAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
