/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :
    mixSlider(juce::Slider::LinearHorizontal, juce::Slider::TextBoxBelow),
    player1(formatManager),
    deckGUI1(&player1, formatManager, thumbCache, playerIndex1,
        juce::Colours::black, juce::Colours::springgreen,
        juce::Colours::springgreen, juce::Colours::black,
        juce::Colours::springgreen, juce::Colours::springgreen,
        juce::Colours::springgreen),
    player2(formatManager),
    deckGUI2(&player2, formatManager, thumbCache, playerIndex2,
        juce::Colours::black, juce::Colours::dodgerblue,
        juce::Colours::dodgerblue, juce::Colours::black,
        juce::Colours::dodgerblue, juce::Colours::dodgerblue,
        juce::Colours::dodgerblue),
    musicLibrary(deckGUI1, deckGUI2)
{

    // Set the size of the component after adding child components
    setSize(800, 600);

    // Request permission to record audio if needed
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
        && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
            [&](bool granted) {
                if (granted)
                    setAudioChannels(2, 2);
            });
    }
    else
    {
        // Specify the number of output channels
        setAudioChannels(0, 2);
    }

    // Initialize the mix slider properties
    mixSlider.addListener(this);
    mixSlider.setRange(0.0, 30.0);
    mixSlider.setValue(15.0, dontSendNotification);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setDoubleClickReturnValue(true, 15.0);

    // Make child components visible
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(musicLibrary);
    // Register basic audio formats
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // Shut down the audio device and clear the audio source
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Prepare audio players for playback
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Prepare the mixer source
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Add audio sources to the mixer
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // Fill the audio buffer with the next audio block from the mixer
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // Release resources when the audio device stops or restarts
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    // Fill the background with the component's background color
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Additional drawing code can be added here
}

void MainComponent::resized()
{
    // Configure layout using FlexBox for responsive design
    FlexBox deckBox;
    deckBox.flexDirection = FlexBox::Direction::row;
    deckBox.items.add(FlexItem(deckGUI1).withFlex(1));
    deckBox.items.add(FlexItem(deckGUI2).withFlex(1));

    FlexBox mainBox;
    mainBox.flexDirection = FlexBox::Direction::column;
    mainBox.justifyContent = FlexBox::JustifyContent::center;

    FlexBox mixBox;
    mixBox.flexDirection = FlexBox::Direction::row;
    mixBox.items.add(FlexItem().withFlex(2));
    mixBox.items.add(FlexItem(mixSlider).withFlex(1).withHeight(50));
    mixBox.items.add(FlexItem().withFlex(2));

    // Add layout items to the main box
    mainBox.items.add(FlexItem(deckBox).withFlex(5));
    mainBox.items.add(FlexItem(mixBox).withFlex(0).withHeight(50));
    mainBox.items.add(FlexItem(musicLibrary).withFlex(2.5));

    // Perform the layout based on the current component bounds
    mainBox.performLayout(getLocalBounds().toFloat());
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    // Update deck GUI mix values based on slider changes
    DBG("sliderValueChanged");
    deckGUI2.setmix(mixSlider.getValue());
    deckGUI1.setmix(30.0 - mixSlider.getValue());
}
