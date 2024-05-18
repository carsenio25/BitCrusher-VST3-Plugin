/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "StkLite-4.6.1/BiQuad.h"
//

//==============================================================================
/**
*/
class CarsonCPFinalBitCrusherAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    CarsonCPFinalBitCrusherAudioProcessor();
    ~CarsonCPFinalBitCrusherAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================

    void calcAlgorithmParams();

    juce::AudioParameterFloat* RateParam;
    juce::AudioParameterFloat* BitParam;
    juce::AudioParameterFloat* LowPassParam;
    juce::AudioParameterFloat* HighPassParam;
    juce::AudioParameterFloat* MixParam;

    float bitDepth;
    int rate;
    float LPFRange;
    float HPFRange;
    float wetPercent;

    float mFs;
    stk::BiQuad mLowPassFilterL, mLowPassFilterR;
    stk::BiQuad mHighPassFilterL, mHighPassFilterR;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarsonCPFinalBitCrusherAudioProcessor)
};
