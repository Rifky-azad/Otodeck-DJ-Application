#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "RotatingDeckComponent.h"
#include "CustomLookAndFeel.h"

class DeckGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public FileDragAndDropTarget,
    public Timer
{
public:
    DeckGUI(DJAudioPlayer* player,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse,
        int& playerIndex,
        Colour knobColor = Colours::green,
        Colour ringColor = Colours::black,
        Colour indicatorColor = Colours::red,
        Colour trackColor = Colours::grey,
        Colour thumbColor = Colours::white,
        Colour buttonColor = Colours::blue,
        Colour buttonOnColor = Colours::darkblue);

    ~DeckGUI() override;

    void paint(Graphics& g) override;
    void resized() override;

    /** Implement Button::Listener */
    void buttonClicked(Button* button) override;

    /** Implement Slider::Listener */
    void sliderValueChanged(Slider* slider) override;

    /** Implement FileDragAndDropTarget */
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    /** Timer callback to update waveform display and slider position */
    void timerCallback() override;

    /** Load file from a file path string */
    bool loadFile(String filePath);
    void setmix(float mix);
private:
    DJAudioPlayer* player;
    WaveformDisplay waveformDisplay;
    RotatingDeckComponent rotatingDeck;

    TextButton playButton{ "Play" };
    TextButton stopButton{ "Stop" };
    TextButton loadButton{ "Load" };

    Slider volSlider;
    Slider speedSlider;
    Slider posSlider;

    Label volumeLabel;
    Label speedLabel;
    Label seekLabel;
    Label fileNameLabel;

    float QPoints[5] = {0.0, 0.0, 0.0, 0.0, 0.0}; // Array to store queue points

    int& playerIndex;
    String currentFilePath; // Store the file path as a string

    float angle;
    CustomLookAndFeel customLookAndFeel;
    juce::FileChooser fChooser{ "Select a file..." };
    TextButton qButton[5]; // Additional queue point buttons
    ToggleButton tButton[5]; // Additional toggle buttons

    bool initialLoad = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
