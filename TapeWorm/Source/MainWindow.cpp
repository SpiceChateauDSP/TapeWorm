/*
  ==============================================================================

    MainWindow.cpp
    Created: 19 Jul 2024 12:32:22am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainWindow.h"

//==============================================================================
MainWindow::MainWindow()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

MainWindow::~MainWindow()
{
}

void MainWindow::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("MainWindow", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void MainWindow::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
