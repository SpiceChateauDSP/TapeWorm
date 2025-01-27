/*
  ==============================================================================

    State.h
    Created: 4 Oct 2024 12:05:51am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class State : public HackAudio::GenericState {
public:
    State (juce::String identifier, juce::AudioProcessor& p) : HackAudio::GenericState (identifier) {
        apvts = new juce::AudioProcessorValueTreeState (p, nullptr, "AutomatableParameters", createParameterLayout());
    }
    ~State () {
        delete apvts;
    };
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() override {
        juce::AudioProcessorValueTreeState::ParameterLayout parameters;
        
        const int versionHint_0 = 1;
        
        juce::NormalisableRange<float> range (0.f, 1.f, 0.00001f);
        range.setSkewForCentre (0.30f);
        auto attributes_float = juce::AudioParameterFloatAttributes()
                                .withStringFromValueFunction ([] (float x, int i) {
                                    return (juce::String) std::round (x * 100) + "%"; });
        
        parameters.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"Damping", versionHint_0},
                                                                     "Damping",
                                                                     range,
                                                                     1.f,
                                                                     attributes_float));
        
        return parameters;
    }
    
    juce::AudioProcessorValueTreeState& getAPVTS () { return *apvts; };
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (State);
};
