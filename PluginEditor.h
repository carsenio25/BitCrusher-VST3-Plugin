/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CarsonCPFinalBitCrusherAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    CarsonCPFinalBitCrusherAudioProcessorEditor (CarsonCPFinalBitCrusherAudioProcessor&);
    ~CarsonCPFinalBitCrusherAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    juce::Slider RateSlider;
    juce::Slider BitsSlider;
    juce::Slider DryWetSlider;
    juce::Slider LPFSlider;
    juce::Slider HPFSlider;

    CarsonCPFinalBitCrusherAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarsonCPFinalBitCrusherAudioProcessorEditor)
};
