#pragma once

#include <JuceHeader.h>

class GlobalStateManager
{
public:
    // Singleton pattern to ensure a single instance
    static GlobalStateManager& getInstance()
    {
        static GlobalStateManager instance;
        return instance;
    }

    // Save settings for a specific player
    void saveSettings(int playerIndex, Slider* volSlider, Slider* speedSlider, Slider* posSlider, const String& currentFile)
    {
        // Generate a prefix for the player index
        String playerPrefix = "player" + String(playerIndex) + "_";

        // Save slider values to preferences
        prefs->setValue(playerPrefix + "volume", volSlider->getValue());
        prefs->setValue(playerPrefix + "speed", speedSlider->getValue());
        prefs->setValue(playerPrefix + "position", posSlider->getValue());
        prefs->setValue(playerPrefix + "currentFile", currentFile); // Save the current playing file
        prefs->saveIfNeeded();
    }

    // Load settings for a specific player
    void loadSettings(int playerIndex, Slider* volSlider, Slider* speedSlider, Slider* posSlider, String& currentFile)
    {
        // Generate a prefix for the player index
        String playerPrefix = "player" + String(playerIndex) + "_";

        volSlider->setValue(prefs->getDoubleValue(playerPrefix + "volume", 0.0));
        speedSlider->setValue(prefs->getDoubleValue(playerPrefix + "speed", 1.0));
        posSlider->setValue(prefs->getDoubleValue(playerPrefix + "position", 0.0));
        currentFile = prefs->getValue(playerPrefix + "currentFile", String()); // Load the current playing file
    }

private:
    GlobalStateManager()
    {
        // Set up properties file options
        PropertiesFile::Options options;
        options.applicationName = ProjectInfo::projectName;
        options.filenameSuffix = ".settings";
        options.osxLibrarySubFolder = "Application Support";
        options.folderName = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory)
            .getChildFile(ProjectInfo::projectName)
            .getFullPathName();
        options.storageFormat = PropertiesFile::storeAsXML;

        // Create the properties file with the specified options
        prefs = std::make_unique<PropertiesFile>(options);
    }

    ~GlobalStateManager() = default;

    // Prevent copying
    GlobalStateManager(const GlobalStateManager&) = delete;
    GlobalStateManager& operator=(const GlobalStateManager&) = delete;

    // JUCE properties file to store settings
    std::unique_ptr<PropertiesFile> prefs;
};
