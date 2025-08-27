#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
//==============================================================================
/*
*/
class WaveformDisplay : public Component,
    public ChangeListener, public MouseListener, public ChangeBroadcaster
{
public:
    WaveformDisplay(AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse,
        Slider* posSlider,
        Colour backgroundColour = Colours::black,
        Colour outlineColour = Colours::grey,
        Colour waveformColour = Colours::orange,
        Colour playheadColour = Colours::lightgreen);

    ~WaveformDisplay();

    void paint(Graphics&) override;
    void resized() override;

    void changeListenerCallback(ChangeBroadcaster* source) override;

    void loadURL(URL audioURL);

    /** set the relative position of the playhead*/
    void setPositionRelative(double pos);

    void mouseDown(const MouseEvent& event) override
    {
        // Calculate the relative position based on the mouse click position
        relativePosition = (float)event.position.x / getWidth();
        if (relativePosition >= 0.0f && relativePosition <= 1.0f)
        {
            posSlider->setValue(relativePosition);
        }
    }


private:
    AudioThumbnail audioThumb;
    bool fileLoaded;
    double position;
    Slider* posSlider;
    float relativePosition;

    Colour backgroundColour;
    Colour outlineColour;
    Colour waveformColour;
    Colour playheadColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
