#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

// CustomLookAndFeel class to provide custom drawing for various UI components
class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    // Constructor to initialize colors for different UI elements
    CustomLookAndFeel(Colour knobColor, Colour ringColor, Colour indicatorColor,
        Colour trackColor, Colour thumbColor, Colour buttonColor,
        Colour buttonOnColor)
        : knobColor(knobColor), ringColor(ringColor), indicatorColor(indicatorColor),
        trackColor(trackColor), thumbColor(thumbColor), buttonColor(buttonColor),
        buttonOnColor(buttonOnColor) {}

    // Draws a rotary slider with custom appearance
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, Slider& slider) override
    {
        Colour fillColour = knobColor;
        Colour outlineColour = knobColor;
        Colour ringColour = ringColor;
        Colour indicatorColour = indicatorColor;

        float sliderSize = jmin(width, height);
        float centerX = (width < height) ? x + sliderSize * 0.5f : x + width * 0.5f;
        float centerY = (width < height) ? y + height * 0.5f : y + sliderSize * 0.5f;

        float knobRadius = jmin(sliderSize / 2.5f - 20.0f, 50.0f);
        float knobDiameter = knobRadius * 2.0f;

        // Draw knob
        g.setColour(fillColour);
        g.fillEllipse(centerX - knobRadius, centerY - knobRadius, knobDiameter, knobDiameter);

        // Draw knob outline
        g.setColour(outlineColour);
        g.drawEllipse(centerX - knobRadius, centerY - knobRadius, knobDiameter, knobDiameter, 1.0f);

        float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Draw outer glow
        g.setColour(ringColour.withAlpha(0.2f));
        Path glowPath;
        glowPath.addCentredArc(centerX, centerY, knobRadius + 18.0f, knobRadius + 18.0f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath(glowPath, PathStrokeType(5.0f));

        // Draw active ring
        g.setColour(ringColour);
        Path ringPath;
        ringPath.addCentredArc(centerX, centerY, knobRadius + 10.0f, knobRadius + 10.0f, 0.0f, rotaryStartAngle, angle, true);
        g.strokePath(ringPath, PathStrokeType(5.0f));

        // Draw indicator
        float indicatorRadius = knobRadius * 0.1f;
        float indicatorX = centerX + (knobRadius - 10.0f) * std::cos(angle - degreesToRadians(90.0f));
        float indicatorY = centerY + (knobRadius - 10.0f) * std::sin(angle - degreesToRadians(90.0f));

        g.setColour(indicatorColour);
        g.fillEllipse(indicatorX - indicatorRadius, indicatorY - indicatorRadius, indicatorRadius * 2.0f, indicatorRadius * 2.0f);
    }

    // Draws a linear slider with custom appearance
    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const Slider::SliderStyle, Slider& slider) override
    {
        // Draw slider track
        g.setColour(trackColor);
        g.fillRect(static_cast<int>(x + width * 0.5f - 2.0f), y, 4, height);

        float thumbRadius = 10.0f;
        float thumbY = jlimit(static_cast<float>(y), y + height - thumbRadius * 2.0f, sliderPos - thumbRadius);

        // Draw slider thumb
        g.setColour(thumbColor);
        g.fillEllipse(x + width * 0.5f - thumbRadius, thumbY, thumbRadius * 2.0f, thumbRadius * 2.0f);

        // Draw thumb outline
        g.setColour(trackColor.withAlpha(0.5f));
        g.drawEllipse(x + width * 0.5f - thumbRadius - 2.0f, thumbY - 2.0f, thumbRadius * 2.0f + 4.0f, thumbRadius * 2.0f + 4.0f, 2.0f);
    }

    // Draws the background of a button with custom appearance
    void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        Colour baseColour = Colours::black;

        if (shouldDrawButtonAsHighlighted)
            baseColour = indicatorColor.darker();

        if (shouldDrawButtonAsDown)
            baseColour = indicatorColor;

        const float cornerSize = 0.0f;
        const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

        // Draw outer glow effect
        for (int i = 1; i <= 2; ++i)
        {
            g.setColour(baseColour.withAlpha(0.3f / i));
            g.drawRoundedRectangle(bounds.expanded(i * 2.0f), cornerSize, 1.0f);
        }

        // Fill button background
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);

        // Draw button border
        g.setColour(baseColour.darker());
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }

    // Draws the text of a button with custom appearance
    void drawButtonText(Graphics& g, TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        Font font = getTextButtonFont(button, button.getHeight());
        g.setFont(font);
        g.setColour(Colours::white); // White text color for better contrast

        const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
        const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;
        const int fontHeight = roundToInt(font.getHeight() * 0.6f);
        const int leftIndent = jmin(fontHeight, button.getWidth() / 3);
        const int rightIndent = leftIndent;

        const int textWidth = button.getWidth() - leftIndent - rightIndent;
        const int textHeight = button.getHeight() - yIndent * 2;

        // Draw button text
        g.drawFittedText(button.getButtonText(), leftIndent, yIndent, textWidth, textHeight,
            Justification::centred, 2);
    }

    // Draws the background of a toggle button with custom appearance
    void drawToggleButton(Graphics& g, ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        Colour baseColour = Colours::black;

        if (shouldDrawButtonAsHighlighted)
            baseColour = indicatorColor.darker();

        if (shouldDrawButtonAsDown)
            baseColour = indicatorColor;

        if (button.getToggleState())
            baseColour = indicatorColor;

        const float cornerSize = 0.0f;
        const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

        // Draw outer glow effect
        for (int i = 1; i <= 2; ++i)
        {
            g.setColour(baseColour.withAlpha(0.3f / i));
            g.drawRoundedRectangle(bounds.expanded(i * 2.0f), cornerSize, 1.0f);
        }

        // Fill button background
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);

        // Draw button border
        g.setColour(baseColour.darker());
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

        // Set text color for toggle button
        g.setColour(Colours::white); // White text color for better contrast
    }

private:
    // Colors for various UI components
    Colour knobColor;
    Colour ringColor;
    Colour indicatorColor;
    Colour trackColor;
    Colour thumbColor;
    Colour buttonColor;
    Colour buttonOnColor;
};
