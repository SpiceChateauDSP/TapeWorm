/*
  ==============================================================================

    Inductor First Order.h
    Created: 25 Aug 2024 4:58:28pm
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Inductor_FirstOrder {
public:
    enum FilterType {
        HPF,
        LPF
    };
    
    Inductor_FirstOrder (FilterType type);
    ~Inductor_FirstOrder () {};
    
    void prepare (const float new_sampleRate, const int new_bufferSize);
    void reset ();
    
    double processSample (int channelIndex, double sample);
    void smoothParameters (int channelIndex);
    
    void setFilterType (FilterType newType);
    void setPotentiometer (const float newP);
    
private:
    FilterType type;
    
    double sampleRate = 48000;
    double samplePeriod = 1 / sampleRate;
    
    double Ui = 400;
    const double Uo = 4 * juce::MathConstants<double>::pi * 10e-7;
    double Bsat = 1.3;
    double N = 1000;
    const double S = 1;
    const double l = 2;
    
    double u = Ui * Uo;
    double b = (1 - std::sqrt (1 / Ui)) / Bsat;
    double c = 1 / u;
    
    double P_target = -1.f;
    double P_smooth[2] { 0.0 };
    
    double R = 400e3;
    
    double x1[2] {0};
    double y1[2] {0};
    
    float smoothTime_seconds = 0.1f; // 100 ms
    double smooth_alpha = std::exp (-1 * std::log (9) / (sampleRate * smoothTime_seconds));
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Inductor_FirstOrder);
};
