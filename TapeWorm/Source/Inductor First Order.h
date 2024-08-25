/*
  ==============================================================================

    Inductor First Order.h
    Created: 25 Aug 2024 4:58:28pm
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum FilterType {
    HPF,
    LPF
};

class Inductor_FirstOrder {
public:
    Inductor_FirstOrder (juce::AudioProcessorValueTreeState&, FilterType type, bool variable);
    ~Inductor_FirstOrder () {};
    
    void prepare (int new_numChannels, int new_bufferSize, double new_sampleRate);
    void reset ();
    
    void processBuffer (juce::AudioBuffer<float>& buffer);
    void processBlock (juce::dsp::AudioBlock<float> block);
    double processSample (int channel, double sample);
    std::pair<double, double> processSample_stereo (std::pair<double, double> input);
    
    void setFilterType (FilterType newType) { type = newType; }
    
    void updateParameters_sample (bool forceUpdate = false);
    void updateParameters_buffer (bool forceUpdate = false);
    
    void smoothParameters (int channel);
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    
    FilterType type;
    
    double Fs = 48000;
    double Ts = 1 / Fs;
    int numChannels = 2;
    int bufferSize = 128;
    
    double Ui = 400;
    const double Uo = 4 * juce::MathConstants<double>::pi * 10e-7;
    double Bsat = 1.3;
    double N = 1000;
    const double S = 1;
    const double l = 2;
    
    double u = Ui * Uo;
    double b = (1 - std::sqrt (1 / Ui)) / Bsat;
    double c = 1 / u;
    
    struct {
        std::atomic<float>* P_vts;
    } valueTree;
    
    float P_target = -1.f;
    double P_smooth[2] {0.0};
    
    double R = 400e3;
    
    double x1[2] {0};
    double y1[2] {0};
    
    bool isVariable = true;
    
    float smoothTime_seconds = 0.1f; // 100 ms
    double smooth_alpha = std::exp (-1 * std::log (9) / (Fs * smoothTime_seconds));
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Inductor_FirstOrder);
};
