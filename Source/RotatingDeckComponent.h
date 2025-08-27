#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class RotatingDeckComponent : public Component
{
public:
    RotatingDeckComponent(Colour lineColour = Colours::red)
        : currentAngle(0.0f), lineColour(lineColour)
    {
    }

    void paint(Graphics& g) override
    {
        // Clear the background with transparency
        g.fillAll(Colours::transparentBlack);

        // Get the bounds of the component
        auto bounds = getLocalBounds().toFloat();
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        float sliderSize = jmin(width, height);

        float centerX, centerY;

        // Calculate center based on the smaller dimension
        if (width < height)
        {
            centerX = bounds.getX() + sliderSize * 0.5f;
            centerY = bounds.getY() + height * 0.5f;
        }
        else
        {
            centerX = bounds.getX() + width * 0.5f;
            centerY = bounds.getY() + sliderSize * 0.5f;
        }


        // Apply rotation
        g.addTransform(AffineTransform::rotation(currentAngle, centerX, centerY));

        // Draw the rotating deck (make it smaller by applying a scale factor)
        float deckScaleFactor = 0.8f; // Adjust this factor to make the deck smaller or larger (0.8 means 80% of original size)
        float smallerSliderSize = sliderSize * deckScaleFactor;

        g.setColour(Colours::black);
        g.fillEllipse(centerX - smallerSliderSize * 0.5f, centerY - smallerSliderSize * 0.5f, smallerSliderSize, smallerSliderSize);
        

        // Draw the rotation marker line with the specified color
        g.setColour(lineColour);
        g.fillRect(centerX, centerY, smallerSliderSize * 0.5f, smallerSliderSize*0.01f);
        smallerSliderSize = smallerSliderSize * 1.10f;
        g.drawEllipse(centerX - smallerSliderSize * 0.5f, centerY - smallerSliderSize * 0.5f, smallerSliderSize, smallerSliderSize, smallerSliderSize * 0.01f); // Ring thickness is set to 2.0f
        g.setColour(lineColour.withAlpha(0.2f));
        g.drawEllipse(centerX - smallerSliderSize * 0.5f, centerY - smallerSliderSize * 0.5f, smallerSliderSize, smallerSliderSize, smallerSliderSize * 0.05f);
    }

    void resized() override
    {
        // This is where you can set the bounds for child components
    }

    void setAngle(float angle) {
        jassert(angle >= 0.0f && angle <= MathConstants<float>::twoPi);
        currentAngle = angle;
        repaint();
    }

private:
    float currentAngle;
    Colour lineColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotatingDeckComponent)
};
