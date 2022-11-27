#pragma once

#include <JuceHeader.h>
class AudioThumbnailComp :
    public juce::Component,
    public juce::ChangeListener,
    public juce::FileDragAndDropTarget,
    public juce::ChangeBroadcaster,
    private juce::ScrollBar::Listener,
    private juce::Timer
{
public:
    AudioThumbnailComp(
        juce::AudioFormatManager& formatManager,
        juce::AudioTransportSource& transport,
        juce::AudioThumbnailCache& thumbCache,
        const juce::File& existingFile = juce::File());

    ~AudioThumbnailComp();

    void setFile(const juce::File& file);

    juce::File getLastDroppedFile() const noexcept;

    void setZoomFactor(double amount);

    void setRange(juce::Range<double> newRange);

    void setFollowsTransport(bool shouldFollow);

    void paint(juce::Graphics& g) override;

    void resized() override;

    void changeListenerCallback(ChangeBroadcaster*) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override { return true; }

    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void mouseDown(const juce::MouseEvent& e) override;

    void mouseDrag(const juce::MouseEvent& e) override;

    void mouseUp(const juce::MouseEvent&) override;

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;

private:
    juce::AudioTransportSource& transportSource;
    juce::ScrollBar scrollbar;

    juce::AudioThumbnail thumbnail;
    juce::Range<double> visibleRange;
    bool isFollowingTransport;
    juce::File lastFileDropped;

    juce::DrawableRectangle currentPositionMarker;

    float timeToX(const double time) const;

    double xToTime(const float x) const;

    bool canMoveTransport() const noexcept;

    void scrollBarMoved(juce::ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;

    void timerCallback() override;

    void updateCursorPosition();
};
