/*
  ==============================================================================

    Inductor First Order.cpp
    Created: 25 Aug 2024 4:58:28pm
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#include "Inductor First Order.h"

Inductor_FirstOrder::Inductor_FirstOrder (FilterType t) : type (t) {
    setFilterType (t);
}

void Inductor_FirstOrder::prepare (const float new_sampleRate, const int new_bufferSize) {
    sampleRate = new_sampleRate;
    samplePeriod = 1 / sampleRate;
    
    smooth_alpha = std::exp (-1 * std::log (9) / (sampleRate * smoothTime_seconds));
    
    reset();
}

void Inductor_FirstOrder::reset () {
    x1[0] = x1[1] = 0;
    y1[0] = y1[1] = 0;
    
    P_smooth[0] = P_smooth[1] = P_target;
    R = (type == HPF) ? P_target * 1975000 + 25000 : P_target * 9.9e6 + 100e3;
}

double Inductor_FirstOrder::processSample (int channelIndex, double sample) {
    smoothParameters (channelIndex);
    
    double Il = y1[channelIndex] / R;
    double L = (c * std::pow(N, 2) * S) / std::pow (l * (c + ((b * N)/l) * abs (Il)), 2);

    double n0 = (type == HPF) ? 2 * L : R * samplePeriod;
    double n1 = (type == HPF) ? -2 * L : R * samplePeriod;
    
    double d = (R * samplePeriod + 2 * L);
    
    double b0 = n0 / d;
    double b1 = n1 / d;
    double a1 = (R * samplePeriod - 2 * L) / d;

    double output = sample * b0 + x1[channelIndex] * b1 - y1[channelIndex] * a1;
    
    y1[channelIndex] = output;
    x1[channelIndex] = sample;
    
    return output;
}

void Inductor_FirstOrder::smoothParameters (int channelIndex) {
    if (!juce::approximatelyEqual (P_target, P_smooth[channelIndex])) {
        P_smooth[channelIndex] = smooth_alpha * (P_smooth[channelIndex] - P_target) + P_target;
        R = (type == HPF) ? P_smooth[channelIndex] * 1975000 + 25000 : P_smooth[channelIndex] * 9.9e6 + 100e3;
    }
}

void Inductor_FirstOrder::setFilterType (FilterType newType) {
    type = newType;
    
    if (type == HPF) {
        Ui = 400;
        Bsat = 1.3;
        N = 1000;
    } else {
        Ui = 800;
        Bsat = 1.0;
        N = 100;
    }
    
    u = Ui * Uo;
    b = (1 - std::sqrt (1 / Ui)) / Bsat;
    c = 1 / u;
}

void Inductor_FirstOrder::setPotentiometer (const float newP) {
    P_target = (1.f - newP);
}
