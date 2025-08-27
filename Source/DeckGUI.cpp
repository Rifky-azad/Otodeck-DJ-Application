#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"
#include "GlobalStateManager.h" // Include your StateManager header
#include <cmath>

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse,
    int& playerIndex,
    Colour knobColor,
    Colour ringColor,
    Colour indicatorColor,
    Colour trackColor,
    Colour thumbColor,
    Colour buttonColor,
    Colour buttonOnColor)
    : player(_player),
    waveformDisplay(formatManagerToUse, cacheToUse, &posSlider, Colours::black, Colours::grey, ringColor, Colours::red),
    playerIndex(playerIndex),
    customLookAndFeel(knobColor, ringColor, indicatorColor, trackColor, thumbColor, buttonColor, buttonOnColor),
    rotatingDeck(indicatorColor)
{
    angle = 0.0;
    fileNameLabel.setJustificationType(Justification::centred);
    fileNameLabel.setText("No file loaded", dontSendNotification); // Default text
    addAndMakeVisible(fileNameLabel); // Add this line to make it visible

    // Change the sliders to rotary style
    volSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    speedSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    posSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);

    volumeLabel.setJustificationType(Justification::centred);
    speedLabel.setJustificationType(Justification::centred);
    seekLabel.setJustificationType(Justification::centred);

    // Initialize labels
    volumeLabel.setText("Volume", dontSendNotification);
    speedLabel.setText("Speed", dontSendNotification);
    seekLabel.setText("Seek", dontSendNotification);

    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);

    for(auto& button : qButton) {
		button.addListener(this);
	}

    for (auto& button : tButton) {
        button.addListener(this);
    }

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    volSlider.setRange(0.0, 30.0); // Volume range from 0 to 30
    speedSlider.setRange(0.0, 2.0); // Speed range from 0.25 to 2.0 with a step of 0.25
    posSlider.setRange(0.0, 1.0); // Position remains the same

    volSlider.setDoubleClickReturnValue(true, 15.0); // Volume range from 0 to 30
    speedSlider.setDoubleClickReturnValue(true, 1.0); // Speed range from 0.25 to 2.0 with a step of 0.25

    volSlider.setValue(15.0, dontSendNotification); // Midpoint of range for volume
    speedSlider.setValue(1.0, dontSendNotification); // Default speed
    posSlider.setValue(0.0, dontSendNotification); // Start at the beginning

    volSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 0, 0); // Remove text box
    speedSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0); // Remove text box
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0); // Remove text box

    // Load settings from StateManager
    GlobalStateManager::getInstance().loadSettings(playerIndex, &volSlider, &speedSlider, &posSlider, currentFilePath);

    setLookAndFeel(&customLookAndFeel);

    addAndMakeVisible(fileNameLabel);
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(volumeLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(seekLabel);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(rotatingDeck);

    for (TextButton& button : qButton) {
        addAndMakeVisible(button);
    }

    for (ToggleButton& button : tButton) {
        addAndMakeVisible(button);
    }

    // Set the DJAudioPlayer as the listener for the waveform display

    startTimer(100);
}

DeckGUI::~DeckGUI()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

void DeckGUI::paint(Graphics& g)
{
    if (!isShowing())
    {
        DBG("lol");
        return;
    }
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)); // clear the background
    g.setColour(Colours::grey);
    g.setColour(Colours::white);
    g.setFont(14.0f);
}

void DeckGUI::resized()
{
    juce::FlexBox mainFlexBox;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
    mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;

    juce::FlexBox fileFlexBox;
    fileFlexBox.flexDirection = juce::FlexBox::Direction::row;
    fileFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    juce::FlexBox sliderFlexBox;
    sliderFlexBox.flexDirection = juce::FlexBox::Direction::column;
    sliderFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    sliderFlexBox.items.add(juce::FlexItem(volumeLabel).withFlex(1));
    sliderFlexBox.items.add(juce::FlexItem(volSlider).withFlex(6));
    sliderFlexBox.items.add(juce::FlexItem(speedLabel).withFlex(1));
    sliderFlexBox.items.add(juce::FlexItem(speedSlider).withFlex(6));
    sliderFlexBox.items.add(juce::FlexItem(seekLabel).withFlex(1));
    sliderFlexBox.items.add(juce::FlexItem(posSlider).withFlex(6));

    juce::FlexBox tButtonFlexBox;
    tButtonFlexBox.flexDirection = juce::FlexBox::Direction::column;
    tButtonFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    for (auto& button : tButton)
    {
        tButtonFlexBox.items.add(juce::FlexItem(button).withFlex(1).withMargin(1));
    }

    juce::FlexBox qButtonFlexBox;
    qButtonFlexBox.flexDirection = juce::FlexBox::Direction::column;
    qButtonFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    for (auto& button : qButton)
    {
        qButtonFlexBox.items.add(juce::FlexItem(button).withFlex(1).withMargin(1));
    }

    juce::FlexBox buttonFlexBox;
    buttonFlexBox.flexDirection = juce::FlexBox::Direction::column;
    buttonFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    const float buttonSize = 25.0f; // Define a square size for the buttons

    // Set square size for each button
    playButton.setBounds(0, 0, buttonSize, buttonSize);
    stopButton.setBounds(0, 0, buttonSize, buttonSize);
    loadButton.setBounds(0, 0, buttonSize, buttonSize);

    buttonFlexBox.items.add(juce::FlexItem(playButton).withFlex(1).withMinWidth(buttonSize).withMinHeight(buttonSize));
    buttonFlexBox.items.add(juce::FlexItem(stopButton).withFlex(1).withMinWidth(buttonSize).withMinHeight(buttonSize));
    buttonFlexBox.items.add(juce::FlexItem(loadButton).withFlex(1).withMinWidth(buttonSize).withMinHeight(buttonSize));

    if (playerIndex == 1) {
        fileFlexBox.items.add(juce::FlexItem(waveformDisplay).withFlex(10));
        fileFlexBox.items.add(juce::FlexItem(buttonFlexBox).withFlex(1));
    }
    else {
        fileFlexBox.items.add(juce::FlexItem(buttonFlexBox).withFlex(1));
        fileFlexBox.items.add(juce::FlexItem(waveformDisplay).withFlex(10));
    }

    FlexBox deckFlexbox;
    deckFlexbox.flexDirection = juce::FlexBox::Direction::row;
    deckFlexbox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    if (playerIndex == 1) {
        deckFlexbox.items.add(juce::FlexItem(tButtonFlexBox).withFlex(0.25));
        deckFlexbox.items.add(juce::FlexItem(qButtonFlexBox).withFlex(0.75).withMargin(2.0f));
        deckFlexbox.items.add(juce::FlexItem(rotatingDeck).withFlex(4).withMargin(2.0f));
        deckFlexbox.items.add(juce::FlexItem(sliderFlexBox).withFlex(2));
    }
    else {
        deckFlexbox.items.add(juce::FlexItem(sliderFlexBox).withFlex(2));
        deckFlexbox.items.add(juce::FlexItem(rotatingDeck).withFlex(4).withMargin(2.0f));
        deckFlexbox.items.add(juce::FlexItem(qButtonFlexBox).withFlex(0.75).withMargin(2.0f));
        deckFlexbox.items.add(juce::FlexItem(tButtonFlexBox).withFlex(0.25));
    }

    mainFlexBox.items.add(juce::FlexItem(fileNameLabel).withFlex(0.25));
    mainFlexBox.items.add(juce::FlexItem(fileFlexBox).withFlex(1));
    mainFlexBox.items.add(juce::FlexItem(deckFlexbox).withFlex(5));

    mainFlexBox.performLayout(getLocalBounds().toFloat());
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        std::cout << "Play button was clicked " << std::endl;
        player->setGain(volSlider.getValue() / 30);
        player->setSpeed(speedSlider.getValue());
        player->start();
    }
    if (button == &stopButton)
    {
        std::cout << "Stop button was clicked " << std::endl;
        player->stop();
    }
    if (button == &loadButton)
    {
        if (initialLoad) {
            loadFile(currentFilePath);
            initialLoad = false;
            return;
        }
        // Call the loadFile function with the current file path
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                File chosenFile = chooser.getResult();
                if (chosenFile.exists())
                {
                    loadFile(chosenFile.getFullPathName()); // Call the loadFile function with the chosen file
                }
            });
    }

    for (int i = 0; i < 5; i++) {
        if (button == &tButton[i]) {
            if (!button->getToggleState()) {
                QPoints[i] = 0.0;
            }
            else {
                QPoints[i] = posSlider.getValue();
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        if (button == &qButton[i]) {
            posSlider.setValue(QPoints[i]);
			}
		}
    }
  // Handle toggle button clicks here if needed

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
    {
        player->setGain(volSlider.getValue() / 30);
    }
    else if (slider == &speedSlider)
    {
        player->setSpeed(speedSlider.getValue());
    }
    else if (slider == &posSlider)
    {
        player->setPositionRelative(posSlider.getValue());
    }

    // Save settings whenever a slider is changed
    GlobalStateManager::getInstance().saveSettings(playerIndex, &volSlider, &speedSlider, &posSlider, currentFilePath);
}

bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    if (files.size() == 1)
    {
        String filePath = files[0];
        loadFile(filePath);
        GlobalStateManager::getInstance().saveSettings(playerIndex, &volSlider, &speedSlider, &posSlider, filePath);
    }
}

void DeckGUI::timerCallback()
{
    if (!player) return; // Check if player is valid
    // Get current position and ensure it's within range
    double currentPosition = jlimit(0.0, 1.0, player->getPositionRelative());

    if (player->isPlaying()) {
        angle += speedSlider.getValue() * 0.05f;
        angle = fmod(angle, degreesToRadians(360.0));
    }

    if (!isnan(currentPosition)) {
        waveformDisplay.setPositionRelative(currentPosition);
        posSlider.setValue(currentPosition, dontSendNotification);
        rotatingDeck.setAngle(angle);
    }

}

bool DeckGUI::loadFile(String filePath)
{

        currentFilePath = filePath;
        File file{ filePath };
        
        if (file.existsAsFile()) {
            DBG("Loading file: " << file.getFullPathName());
            player->loadURL(URL{ file });
            waveformDisplay.loadURL(URL{ file });
            fileNameLabel.setText(file.getFileName(), dontSendNotification); // Update label text
            return true;
        }
        return false;


}

void DeckGUI::setmix(float mix)
{
    if (&volSlider != nullptr) {
        volSlider.setValue(mix);
    }
    
}