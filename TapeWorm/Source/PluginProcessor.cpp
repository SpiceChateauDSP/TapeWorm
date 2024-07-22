/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapeWormAudioProcessor::TapeWormAudioProcessor()
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
#if PERFETTO
    MelatoninPerfetto::get().beginSession();
#endif
}

TapeWormAudioProcessor::~TapeWormAudioProcessor() {
#if PERFETTO
    MelatoninPerfetto::get().endSession();
#endif
}

//==============================================================================
const juce::String TapeWormAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool TapeWormAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapeWormAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapeWormAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapeWormAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int TapeWormAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TapeWormAudioProcessor::getCurrentProgram() {
    return 0;
}

void TapeWormAudioProcessor::setCurrentProgram (int index) {
}

const juce::String TapeWormAudioProcessor::getProgramName (int index) {
    return {};
}

void TapeWormAudioProcessor::changeProgramName (int index, const juce::String& newName) {
}

//==============================================================================
void TapeWormAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    auto numChannels = getMainBusNumInputChannels() > getMainBusNumOutputChannels() ? getMainBusNumInputChannels() :
                                                                                      getMainBusNumOutputChannels();
    
    engine.prepare (numChannels, samplesPerBlock, sampleRate);
}

void TapeWormAudioProcessor::releaseResources() {
    engine.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapeWormAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
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

void TapeWormAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    TRACE_DSP();
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    engine.processBuffer (buffer);
}

//==============================================================================
bool TapeWormAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapeWormAudioProcessor::createEditor() {
//    return new TapeWormAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void TapeWormAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    auto currentState = apvts.copyState();

    std::unique_ptr<juce::XmlElement> xml (currentState.createXml());
    copyXmlToBinary (*xml, destData);
}

void TapeWormAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml && xml->hasTagName (apvts.state.getType())) {
        juce::ValueTree newTree = juce::ValueTree::fromXml (*xml);
        
        apvts.replaceState (newTree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout TapeWormAudioProcessor::createParameters() {
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;

    const int versionHint_0 = 1;
    
    juce::NormalisableRange<float> range (0.f, 1.f, 0.001f);
    range.setSkewForCentre (0.30f);
    auto attributes_float = juce::AudioParameterFloatAttributes()
                            .withStringFromValueFunction ([] (float x, int i) {
                                return (juce::String) std::round (-7.142857142856996 * x * x + 2.521428571428570e02 * x + 5) + " Hz"; });
    
    parameters.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"Cutoff", versionHint_0},
                                                                 "Cutoff",
                                                                 range,
                                                                 0.f,
                                                                 attributes_float));

    return parameters;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new TapeWormAudioProcessor();
}
