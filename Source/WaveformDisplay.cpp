/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 14 Mar 2020 3:50:16pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse,
    Slider* posSlider,
    Colour backgroundColour,
    Colour outlineColour,
    Colour waveformColour,
    Colour playheadColour) :
    audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0.0),
    posSlider(posSlider),
    backgroundColour(backgroundColour),
    outlineColour(outlineColour),
    waveformColour(waveformColour),
    playheadColour(playheadColour) // Initialize new member variables
{   
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
    audioThumb.removeChangeListener(this); // Remove the listener in the destructor
}

void WaveformDisplay::paint(Graphics& g)
{
    g.fillAll(backgroundColour); // Clear the background

    g.setColour(outlineColour);
    g.drawRect(getLocalBounds(), 1); // Draw an outline around the component

    g.setColour(waveformColour);
    if (fileLoaded)
    {
        audioThumb.drawChannel(g,
            getLocalBounds(),
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f
        );

        // Draw playhead
        g.setColour(playheadColour.withAlpha(0.8f));
        g.fillRect(position * getWidth(), 0, getWidth() / 150, getHeight());
    }
    else
    {
        g.setFont(20.0f);
        g.drawText("File not loaded...", getLocalBounds(),
            Justification::centred, true); // Draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child components that your component contains..
}

void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));

    if (fileLoaded)
    {
        std::cout << "wfd: loaded! " << std::endl;
        repaint();
    }
    else
    {
        std::cout << "wfd: not loaded! " << std::endl;
    }
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    std::cout << "wfd: change received! " << std::endl;
    repaint();
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}


