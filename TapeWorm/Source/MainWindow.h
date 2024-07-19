/*
  ==============================================================================

    MainWindow.h
    Created: 19 Jul 2024 12:32:22am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MainWindow  : public juce::Component
{
public:
    MainWindow();
    ~MainWindow() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
