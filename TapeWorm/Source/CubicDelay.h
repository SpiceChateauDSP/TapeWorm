/*
  ==============================================================================

    CubicDelay.h
    Created: 24 Jun 2024 9:40:48pm
    Author:  SpiceChateauDSP

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define pi 3.14159265358979

class ModDelay {
public:
    ModDelay (int newSize, float rate) {
        setParameters (newSize, rate);
    }
    ~ModDelay () {}
    
    void reset () {
        delayLine.fill (0);
        theta = 0;
    }
    
    void setParameters (int newSize, float rate) {
        size = newSize;
        auto size_minus1 = size - 1;
        writeIndex = size_minus1;
         
        a = 0.005f * size_minus1;
        dc = 0.995f * size_minus1;
        phi = rate / 48000.f * 2.f * pi;
        theta = 0;
        
        delayLine.resize (size);
        reset();
    }
    
    inline double processSample (double sample) {
        writeIndex++;
        if (writeIndex > size - 1)
            writeIndex = 0;
        
        delayLine.set (writeIndex, sample);
        
        auto delayTime = a * std::sin (theta) + dc;
        theta += phi;
        if (theta > (2 * pi))
            theta -= 2 * pi;
        
        int iDel = std::floor (delayTime);
        double frac = delayTime - iDel;
        
        auto readPointers = findReadPointers (iDel);
        return interpolate (readPointers, frac);
    }
    
    inline std::array<int, 4> findReadPointers (int iDel) {
        auto r1 = writeIndex - iDel + 1;
        if (r1 < 0)
            r1 = size + r1;
        else if (r1 > size - 1)
            r1 = r1 - size;
        
        auto r2 = writeIndex - iDel;
        if (r2 < 0)
            r2 = size + r2;
        
        auto r3 = writeIndex - iDel - 1;
        if (r3 < 0)
            r3 = size + r3;
        
        auto r4 = writeIndex - iDel - 2;
        if (r4 < 0)
            r4 = size + r4;
        
        std::array<int, 4> readPointers { r1, r2, r3, r4 };
        return readPointers;
    }
    
    inline double interpolate (std::array<int, 4> readPointers, double frac) {
        int r1 = readPointers[0];
        int r2 = readPointers[1];
        int r3 = readPointers[2];
        int r4 = readPointers[3];
        
        double frac2 = frac * frac;
        
        double a0 = delayLine[r4] - delayLine[r3] - delayLine[r1] + delayLine[r2];
        double a1 = delayLine[r1] - delayLine[r2] - a0;
        double a2 = delayLine[r3] - delayLine[r1];
        double a3 = delayLine[r2];
        
        return (a0 * frac * frac2 + a1 * frac2 + a2 * frac + a3);
    }
    
private:
    int size = 0;
    juce::Array<double> delayLine;
    
    int writeIndex = 0;
    
    float a, dc, theta, phi;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModDelay)
};
