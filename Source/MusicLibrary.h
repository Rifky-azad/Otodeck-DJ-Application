#pragma once

#include <JuceHeader.h>

struct MusicEntry
{
    juce::String title;
    juce::String artist;
    juce::String album;
    juce::String duration;
    juce::String filePath;
};

class MusicLibrary : public juce::Component,
    public juce::TableListBoxModel,
    private juce::TextEditor::Listener,
    private juce::Button::Listener,
    public juce::FileDragAndDropTarget // Add this
{
public:
    MusicLibrary(DeckGUI& deck1, DeckGUI& deck2) : deckGUI1(deck1), deckGUI2(deck2)
    {

        // Register audio formats
        formatManager.registerBasicFormats();

        // Set up the table list box
        tableListBox.setModel(this);
        addAndMakeVisible(tableListBox);

        // Set up the search box
        searchBox.setMultiLine(false);
        searchBox.setReturnKeyStartsNewLine(false);
        searchBox.setTextToShowWhenEmpty("Search...", juce::Colours::grey);
        searchBox.addListener(this);
        addAndMakeVisible(searchBox);

        // Set up buttons for decks
        addAndMakeVisible(addToDeck1Button);
        addAndMakeVisible(addToDeck2Button);
        addToDeck1Button.setButtonText("Add to Deck 1");
        addToDeck2Button.setButtonText("Add to Deck 2");
        addToDeck1Button.addListener(this);
        addToDeck2Button.addListener(this);


        addAndMakeVisible(clearButton);
        clearButton.setButtonText("Clear Library");
        clearButton.addListener(this);

        // Add columns to the table
        tableListBox.getHeader().addColumn("Title", 1, 200);
        tableListBox.getHeader().addColumn("Artist", 2, 150);
        tableListBox.getHeader().addColumn("Album", 3, 150);
        tableListBox.getHeader().addColumn("Duration", 4, 100);
        tableListBox.getHeader().addColumn("File Path", 5, 250);

        // Set up FlexBox properties
        flexBox.flexDirection = juce::FlexBox::Direction::column; // Stack vertically
        loadFileListFromFile();
    }

    ~MusicLibrary() override
    {
        saveFileListToFile();
    }

    // Override methods from TableListBoxModel
    int getNumRows() override
    {
        return filteredData.size(); // Return the number of filtered entries
    }

    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(juce::Colours::darkgrey);
        else
            g.fillAll(juce::Colour(0xFF1A1A1A));
    }

    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {
        g.setColour(juce::Colours::white); // Set text color to white

        const MusicEntry& entry = filteredData[rowNumber];
        switch (columnId)
        {
        case 1: g.drawText(entry.title, 2, 0, width - 4, height, juce::Justification::left); break;
        case 2: g.drawText(entry.artist, 2, 0, width - 4, height, juce::Justification::left); break;
        case 3: g.drawText(entry.album, 2, 0, width - 4, height, juce::Justification::left); break;
        case 4: g.drawText(entry.duration, 2, 0, width - 4, height, juce::Justification::left); break;
        case 5: g.drawText(entry.filePath, 2, 0, width - 4, height, juce::Justification::left); break;
        }
    }

    void resized() override
    {
        flexBox.items.clear(); // Clear previous items

        // Add the search box, table, and button area to the FlexBox
        flexBox.items.add(juce::FlexItem(searchBox).withFlex(0).withHeight(30)); // Search box
        flexBox.items.add(juce::FlexItem(tableListBox).withFlex(1)); // Table list box

        // Create a container for buttons and add them to the FlexBox
        juce::FlexBox buttonBox;
        buttonBox.flexDirection = juce::FlexBox::Direction::row; // Stack buttons horizontally
        buttonBox.items.add(juce::FlexItem(addToDeck1Button).withFlex(4).withMargin(5)); // Button 1
        buttonBox.items.add(juce::FlexItem(addToDeck2Button).withFlex(4).withMargin(5)); // Button 2
        buttonBox.items.add(juce::FlexItem(clearButton).withFlex(1).withMargin(5)); // Button 3

        // Add the buttonBox to the main FlexBox
        flexBox.items.add(juce::FlexItem(buttonBox).withFlex(0).withHeight(40)); // Buttons area

        // Perform layout
        flexBox.performLayout(getLocalBounds());
    }

    void loadMusicFile(const juce::File& musicFile)
    {
        juce::String title = musicFile.getFileNameWithoutExtension();
        juce::String artist = "Unknown Artist";
        juce::String album = "Unknown Album";
        juce::String duration = "Unknown Duration";
        juce::String filePath = musicFile.getFullPathName(); // Get the full file path

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(musicFile));
        if (reader != nullptr)
        {
            int seconds = static_cast<int>(reader->lengthInSamples / reader->sampleRate) % 60;
            int minutes = static_cast<int>((reader->lengthInSamples / reader->sampleRate) / 60) % 60;
            duration = juce::String::formatted("%02d:%02d", minutes, seconds);
        }

        data.add({ title, artist, album, duration, filePath });
        filteredData = data; // Update filtered data
        tableListBox.updateContent();
    }

    void addToDeck(int deckNumber, const MusicEntry& entry)
    {
        // Implement your logic to add the selected entry to the specified deck
        if (deckNumber == 1)
        {
            DBG("Added to Deck 1: " << entry.title);
            // Add logic to handle Deck 1
        }
        else if (deckNumber == 2)
        {
            DBG("Added to Deck 2: " << entry.title);
            // Add logic to handle Deck 2
        }
    }

    juce::String getFileListAsString() const
    {
        juce::StringArray fileList;
        for (const auto& entry : data)
        {
            fileList.add(entry.filePath);
        }
        return fileList.joinIntoString("\n");
    }

private:
    void saveFileListToFile() const
    {
        juce::String fileListString = getFileListAsString();
        juce::File userDocumentsFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
        juce::File file = userDocumentsFolder.getChildFile("MusicLibraryFileList.txt");
        file.replaceWithText(fileListString);
    }


    void loadFileListFromFile()
    {
        juce::File userDocumentsFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
        juce::File file = userDocumentsFolder.getChildFile("MusicLibraryFileList.txt");

        if (file.existsAsFile())
        {
            juce::String fileListString = file.loadFileAsString();
            juce::StringArray fileList;
            fileList.addLines(fileListString);

            for (const auto& filePath : fileList)
            {
                juce::File musicFile(filePath);
                if (musicFile.existsAsFile())
                {
                    loadMusicFile(musicFile);
                }
            }
        }
    }

    void textEditorTextChanged(TextEditor& editor) override
    {
        if (&editor == &searchBox)
        {
            filterData();
            tableListBox.updateContent(); // Update the table to show filtered results
        }
    }

    void filterData()
    {
        juce::String searchText = searchBox.getText().toLowerCase();
        filteredData.clear();

        for (const auto& entry : data)
        {
            if (entry.title.toLowerCase().contains(searchText) ||
                entry.artist.toLowerCase().contains(searchText) ||
                entry.album.toLowerCase().contains(searchText))
            {
                filteredData.add(entry);
            }
        }
    }

    void selectedRowsChanged(int /*lastRowSelected*/) override
    {
        // Update the currently selected row whenever the selection changes
        currentlySelectedRow = tableListBox.getSelectedRow();
    }

    void buttonClicked(juce::Button* button) override
    {
        if (button == &addToDeck1Button)
        {
            if (currentlySelectedRow >= 0 && currentlySelectedRow < filteredData.size())
            {
                const MusicEntry& entry = filteredData[currentlySelectedRow];
                deckGUI1.loadFile(entry.filePath);
            }
        }
        else if (button == &addToDeck2Button)
        {
            if (currentlySelectedRow >= 0 && currentlySelectedRow < filteredData.size())
            {
                const MusicEntry& entry = filteredData[currentlySelectedRow];
                deckGUI2.loadFile(entry.filePath);
            }
        }
        else if (button == &clearButton)
        {
            clearButtonClicked();
        }
    }

    // Implement drag-and-drop methods
    bool isInterestedInFileDrag(const juce::StringArray& files) override
    {
        // Check if any of the files are valid music files
        for (const auto& file : files)
        {
            if (file.endsWithIgnoreCase(".mp3") || file.endsWithIgnoreCase(".wav") || file.endsWithIgnoreCase(".flac"))
                return true;
        }
        return false;
    }



    void filesDropped(const juce::StringArray& files, int x, int y) override
    {
        for (const auto& filePath : files)
        {
            juce::File file(filePath);
            if (file.existsAsFile())
            {
                loadMusicFile(file); // Load the music file
            }
        }

        saveFileListToFile();
    }


    void MusicLibrary::clearButtonClicked()
    {
        data.clear();
        filteredData.clear();
        saveFileListToFile();
        loadFileListFromFile();
        tableListBox.updateContent();
    }

    juce::AudioFormatManager formatManager; // Declare the formatManager


    juce::TableListBox tableListBox;
    juce::Array<MusicEntry> data;         // Store original data
    juce::Array<MusicEntry> filteredData; // Store filtered data
    juce::TextEditor searchBox;           // Search box
    juce::TextButton addToDeck1Button;    // Button to add to Deck 1
    juce::TextButton addToDeck2Button;
    juce::TextButton clearButton;    // Button to add to Deck 2
    int currentlySelectedRow = -1;         // Store the currently selected row

    juce::FlexBox flexBox; // FlexBox for layout

    DeckGUI& deckGUI1;
    DeckGUI& deckGUI2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicLibrary)
};
