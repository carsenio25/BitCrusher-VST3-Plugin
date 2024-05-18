/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CarsonCPFinalBitCrusherAudioProcessor::CarsonCPFinalBitCrusherAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(RateParam = new juce::AudioParameterFloat("Rate", // parameterID,
        "Rate", // parameterName,
        0.0f, // minValue,
        100.0f, // maxValue,
        0.0f)); // defaultValue

    addParameter(BitParam = new juce::AudioParameterFloat("Bits", // parameterID,
        "Bits", // parameterName,
        0.0f, // minValue,
        32.0f, // maxValue,
        32.0f)); // defaultValue

    addParameter(LowPassParam = new juce::AudioParameterFloat("LPF", // parameterID,
        "LPF", // parameterName,
        0.0f, // minValue,
        20000.0f, // maxValue,
        20000.0f)); // defaultValue

    addParameter(HighPassParam = new juce::AudioParameterFloat("HPF", // parameterID,
        "HPF", // parameterName,
        0.0f, // minValue,
        20000.0f, // maxValue,
        0.0f)); // defaultValue

    addParameter(MixParam = new juce::AudioParameterFloat("Wet/Dry Mix", // parameterID,
        "Wet/Dry", // parameterName,
        0.0f, // minValue,
        100.0f, // maxValue,
        100.0f)); // defaultValue


}

CarsonCPFinalBitCrusherAudioProcessor::~CarsonCPFinalBitCrusherAudioProcessor()
{
}

//==============================================================================
const juce::String CarsonCPFinalBitCrusherAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CarsonCPFinalBitCrusherAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CarsonCPFinalBitCrusherAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CarsonCPFinalBitCrusherAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CarsonCPFinalBitCrusherAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CarsonCPFinalBitCrusherAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CarsonCPFinalBitCrusherAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CarsonCPFinalBitCrusherAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CarsonCPFinalBitCrusherAudioProcessor::getProgramName (int index)
{
    return {};
}

void CarsonCPFinalBitCrusherAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CarsonCPFinalBitCrusherAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mFs = sampleRate;
}

void CarsonCPFinalBitCrusherAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CarsonCPFinalBitCrusherAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CarsonCPFinalBitCrusherAudioProcessor::calcAlgorithmParams()
{

    bitDepth = BitParam->get();
    rate = RateParam->get();
    LPFRange = LowPassParam->get();
    HPFRange = HighPassParam->get();
    wetPercent = MixParam->get();

    // Calculate filter coefficients
    float HPFCoeffs[5]; 
    float LPFCoeffs[5];

    Mu45FilterCalc::calcCoeffsLPF(LPFCoeffs, LPFRange, 1, mFs);
    Mu45FilterCalc::calcCoeffsHPF(HPFCoeffs, HPFRange, 1, mFs);



    // Set the coefficients for each filter
    mHighPassFilterL.setCoefficients(HPFCoeffs[0], HPFCoeffs[1], HPFCoeffs[2], HPFCoeffs[3], HPFCoeffs[4]);
    mHighPassFilterR.setCoefficients(HPFCoeffs[0], HPFCoeffs[1], HPFCoeffs[2], HPFCoeffs[3], HPFCoeffs[4]);

    mLowPassFilterL.setCoefficients(LPFCoeffs[0], LPFCoeffs[1], LPFCoeffs[2], LPFCoeffs[3], LPFCoeffs[4]);
    mLowPassFilterR.setCoefficients(LPFCoeffs[0], LPFCoeffs[1], LPFCoeffs[2], LPFCoeffs[3], LPFCoeffs[4]);

}

void CarsonCPFinalBitCrusherAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    calcAlgorithmParams();

    wetPercent /= 100.0;
    float dryPercent = 1.0 - wetPercent;
    

    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    float wetOutL, wetOutR, dryOutL, dryOutR;

        for (int samp = 0; samp < buffer.getNumSamples(); samp++) {

            dryOutL = channelDataLeft[samp] * dryPercent;
            dryOutR = channelDataRight[samp] * dryPercent;

            // reduce bit depth
            float quantization = powf(2, bitDepth); // how many different quantization levels we can use
            float remL = fmodf(channelDataLeft[samp], 1 / quantization);
            float remR = fmodf(channelDataRight[samp], 1 / quantization);

            // quantization on y-axis
            channelDataLeft[samp] = channelDataLeft[samp] - remL; //finds data a little bit above the quantization level and throws it out
            channelDataRight[samp] = channelDataRight[samp] - remR;

            // quantization on x-axis
            // hold each sample for a certain number of spots
            // if this isn't the 1st, 2nd, 3rd sample, and so on, hold it there
            if (rate > 1) {
                if (samp % rate != 0) {
                    channelDataLeft[samp] = channelDataLeft[samp - samp % rate];
                    channelDataRight[samp] = channelDataRight[samp - samp % rate];
                }
            }

            // wet/dry mixing
            wetOutL = channelDataLeft[samp] * wetPercent;
            wetOutR = channelDataRight[samp] * wetPercent;
            channelDataLeft[samp] = dryOutL + wetOutL;
            channelDataRight[samp] = dryOutR + wetOutR;

            // add filter
            channelDataLeft[samp] = mHighPassFilterL.tick(channelDataLeft[samp]);
            channelDataRight[samp] = mHighPassFilterR.tick(channelDataRight[samp]);
            channelDataLeft[samp] = mLowPassFilterL.tick(channelDataLeft[samp]);
            channelDataRight[samp] = mLowPassFilterR.tick(channelDataRight[samp]);
            
        }
}

//==============================================================================
bool CarsonCPFinalBitCrusherAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CarsonCPFinalBitCrusherAudioProcessor::createEditor()
{
    return new CarsonCPFinalBitCrusherAudioProcessorEditor (*this);
}

//==============================================================================
void CarsonCPFinalBitCrusherAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CarsonCPFinalBitCrusherAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CarsonCPFinalBitCrusherAudioProcessor();
}
