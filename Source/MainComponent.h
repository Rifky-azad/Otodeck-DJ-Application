/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"

//==============================================================================
/**
 * MainComponent is responsible for managing audio playback controls and
 * displaying the deck GUI components for the DJ application.
 */
#include "MusicLibrary.h"

class MainComponent : public AudioAppComponent, public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();  // Constructor to initialize the component
    ~MainComponent(); // Destructor to clean up resources

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override; // Prepare audio playback
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override; // Fill audio buffer with the next block
    void releaseResources() override; // Release resources when audio device stops
    void sliderValueChanged(Slider* slider) override; // Respond to slider value changes

    //==============================================================================
    void paint(Graphics& g) override; // Render the component's graphics
    void resized() override; // Handle component resizing

private:
    //==============================================================================
    // Private member variables for audio players and UI components
    int playerIndex1{ 1 }; // Index for player 1
    int playerIndex2{ 2 }; // Index for player 2
    AudioFormatManager formatManager; // Manages audio file formats
    AudioThumbnailCache thumbCache{ 100 }; // Cache for audio thumbnails

    // Create audio player instances for each deck
    DJAudioPlayer player1{ formatManager };
    DeckGUI deckGUI1{ &player1, formatManager, thumbCache, playerIndex1,
                      Colours::black, Colours::springgreen,
                      Colours::springgreen, Colours::black,
                      Colours::springgreen, Colours::springgreen,
                      Colours::springgreen };

    DJAudioPlayer player2{ formatManager };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache, playerIndex2,
                      Colours::black, Colours::dodgerblue,
                      Colours::dodgerblue, Colours::black,
                      Colours::dodgerblue, Colours::dodgerblue,
                      Colours::dodgerblue };
    
    MusicLibrary musicLibrary{ deckGUI1 ,deckGUI2 };

    MixerAudioSource mixerSource; // Mixer for combining audio sources
    Slider mixSlider{}; // Slider to control audio mix level

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent) // Prevent copying and memory leaks
};
