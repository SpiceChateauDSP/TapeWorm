/*
  ==============================================================================

    Inductor First Order.cpp
    Created: 25 Aug 2024 4:58:28pm
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#include "Inductor First Order.h"

Inductor_FirstOrder::Inductor_FirstOrder (juce::AudioProcessorValueTreeState& vts, FilterType t, bool v) :
    apvts (vts),
    type (t),
    isVariable (v)
{
    /* Connect ValueTree parameters to local pointer */
    valueTree.P_vts = apvts.getRawParameterValue ("Damping");
}

void Inductor_FirstOrder::prepare (int new_numChannels, int new_bufferSize, double new_sampleRate) {
    if (numChannels != new_numChannels)
        numChannels = new_numChannels;
    
    if (bufferSize != new_bufferSize)
        bufferSize = new_bufferSize;
    
    if (Fs != new_sampleRate) {
        Fs = new_sampleRate;
        Ts = 1 / Fs;
    }
    
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
    
    smooth_alpha = std::exp (-1 * std::log (9) / Fs * smoothTime_seconds);
    
    reset();
}

void Inductor_FirstOrder::reset () {
    updateParameters_buffer (true);
    updateParameters_sample (true);
}

void Inductor_FirstOrder::processBuffer (juce::AudioBuffer<float>& buffer) {
    /* Update any per-buffer parameters */
    updateParameters_buffer();
    
    /* Channel Loop */
    for (int channelIndex = 0; channelIndex < buffer.getNumChannels(); ++channelIndex) {
        auto* channelData = buffer.getWritePointer (channelIndex);
        
        /* Sample Loop */
        for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); ++sampleIndex) {
            auto sample = channelData[sampleIndex];
            
            channelData[sampleIndex] = processSample (channelIndex, sample);
        }
    }
}

void Inductor_FirstOrder::processBlock (juce::dsp::AudioBlock<float> block) {
    /* Update any per-buffer parameters */
    updateParameters_buffer();
    
    /* Channel Loop */
    for (int channelIndex = 0; channelIndex < block.getNumChannels(); ++channelIndex) {
        /* Sample Loop */
        for (int sampleIndex = 0; sampleIndex < block.getNumSamples(); ++sampleIndex) {
            auto sample = block.getSample (channelIndex, sampleIndex);
            
            block.setSample (channelIndex, sampleIndex, sample);
        }
    }
}

double Inductor_FirstOrder::processSample (int channel, double sample) {
    updateParameters_sample();
    smoothParameters (channel);
    
    double Il = y1[channel] / R;
    double L = (c * std::pow(N, 2) * S) / std::pow (l * (c + ((b * N)/l) * abs (Il)), 2);

    double n0, n1;
    if (type == HPF) {
        n0 = 2 * L;
        n1 = -2 * L;
    } else {
        n0 = R * Ts;
        n1 = R * Ts;
    }
    
    double d = (R * Ts + 2 * L);
    
    double b0 = n0 / d;
    double b1 = n1 / d;
    double a1 = (R * Ts - 2 * L) / d;

    double output = sample * b0 + x1[channel] * b1 - y1[channel] * a1;
    
    y1[channel] = output;
    x1[channel] = sample;
    
    return output;
}

std::pair<double, double> Inductor_FirstOrder::processSample_stereo (std::pair<double, double> input) {
    auto output = std::make_pair (processSample (0, input.first), processSample (1, input.second));
    return output;
}

void Inductor_FirstOrder::updateParameters_sample (bool forceUpdate) {
    if (!isVariable)
        return;
    
    auto value = valueTree.P_vts->load (std::memory_order_acquire);
    if (std::abs (P_target - value) > 0.005 || forceUpdate)
        P_target = value;
}

void Inductor_FirstOrder::updateParameters_buffer (bool forceUpdate) {
    
}

void Inductor_FirstOrder::smoothParameters (int channel) {
    if (!isVariable)
        return;
    
    if (P_target != P_smooth[channel]) {
            P_smooth[channel] = smooth_alpha * (P_smooth[channel] - P_target) + P_target;
        
        
        R = (type == HPF) ? P_smooth[channel] * 1975000 + 25000 : P_smooth[channel] * 9.9e6 + 100e3;
    }
}
