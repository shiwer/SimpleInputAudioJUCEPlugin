#include "AudioThumbnailComp.h"

AudioThumbnailComp::AudioThumbnailComp(
    juce::AudioFormatManager& formatManager,
    juce::AudioTransportSource& transport,
    juce::AudioThumbnailCache& thumbCache,
    const juce::File& existingFile) :
        transportSource(transport),
        scrollbar(false),
        thumbnail(512, formatManager, thumbCache),
        isFollowingTransport(false)
{
    thumbnail.addChangeListener(this);

    addAndMakeVisible(scrollbar);
    scrollbar.setRangeLimits(visibleRange);
    scrollbar.setAutoHide(false);
    scrollbar.addListener(this);

    currentPositionMarker.setFill(juce::Colours::white.withAlpha(0.85f));
    addAndMakeVisible(currentPositionMarker);

    setFile(existingFile);
}

AudioThumbnailComp::~AudioThumbnailComp()
{
    scrollbar.removeListener(this);
    thumbnail.removeChangeListener(this);
}

void AudioThumbnailComp::setFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        thumbnail.setSource(new juce::FileInputSource(file));
        const juce::Range<double> newRange(0.0, thumbnail.getTotalLength());
        scrollbar.setRangeLimits(newRange);
        setRange(newRange);

        startTimerHz(40);
    }
}

juce::File AudioThumbnailComp::getLastDroppedFile() const noexcept
{
    return lastFileDropped;
}

void AudioThumbnailComp::setZoomFactor(double amount)
{
    if (thumbnail.getTotalLength() > 0)
    {
        const double newScale = juce::jmax(0.001, thumbnail.getTotalLength() * (1.0 - juce::jlimit(0.0, 0.99, amount)));
        const double timeAtCentre = xToTime(getWidth() / 2.0f);
        setRange(juce::Range<double>(timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
    }
}

void AudioThumbnailComp::setRange(juce::Range<double> newRange)
{
    visibleRange = newRange;
    scrollbar.setCurrentRange(visibleRange);
    updateCursorPosition();
    repaint();
}

void AudioThumbnailComp::setFollowsTransport(bool shouldFollow)
{
    isFollowingTransport = shouldFollow;
}

void AudioThumbnailComp::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightblue);

    if (thumbnail.getTotalLength() > 0.0)
    {
        juce::Rectangle<int> thumbArea(getLocalBounds());
        thumbArea.removeFromBottom(scrollbar.getHeight() + 4);
        thumbnail.drawChannels(g, thumbArea.reduced (2),
            visibleRange.getStart(), visibleRange.getEnd(), 1.0f);
    }
    else
    {
        g.setFont(14.0f);
        g.drawFittedText("(No audio file selected)", getLocalBounds(), juce::Justification::centred, 2);
    }
}

void AudioThumbnailComp::resized()
{
    scrollbar.setBounds(getLocalBounds().removeFromBottom(14).reduced(2));
}

void AudioThumbnailComp::changeListenerCallback(ChangeBroadcaster*)
{
    // this method is called by the thumbnail when it has changed, so we should repaint it..
    repaint();
}

void AudioThumbnailComp::filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/)
{
    lastFileDropped = juce::File(files[0]);
    sendChangeMessage();
}

void AudioThumbnailComp::mouseDown(const juce::MouseEvent& e)
{
    mouseDrag(e);
}

void AudioThumbnailComp::mouseDrag(const juce::MouseEvent& e)
{
    if (canMoveTransport())
        transportSource.setPosition(juce::jmax(0.0, xToTime((float)e.x)));
}

void AudioThumbnailComp::mouseUp(const juce::MouseEvent&)
{
    transportSource.start();
}

void AudioThumbnailComp::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    if (thumbnail.getTotalLength() > 0.0)
    {
        double newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
        newStart = juce::jlimit(0.0, juce::jmax(0.0, thumbnail.getTotalLength() - (visibleRange.getLength())), newStart);

        if (canMoveTransport())
            setRange(juce::Range<double>(newStart, newStart + visibleRange.getLength()));

        repaint();
    }
}

float AudioThumbnailComp::timeToX(const double time) const
{
    return getWidth() * (float)((time - visibleRange.getStart()) / (visibleRange.getLength()));
}

double AudioThumbnailComp::xToTime(const float x) const
{
    return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
}

bool AudioThumbnailComp::canMoveTransport() const noexcept
{
    return !(isFollowingTransport && transportSource.isPlaying());
}

void AudioThumbnailComp::scrollBarMoved(juce::ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    if (scrollBarThatHasMoved == &scrollbar)
        if (!(isFollowingTransport && transportSource.isPlaying()))
            setRange(visibleRange.movedToStartAt(newRangeStart));
}

void AudioThumbnailComp::timerCallback()
{
    if (canMoveTransport())
        updateCursorPosition();
    else
        setRange(visibleRange.movedToStartAt(transportSource.getCurrentPosition() - (visibleRange.getLength() / 2.0)));
}

void AudioThumbnailComp::updateCursorPosition()
{
    currentPositionMarker.setVisible(transportSource.isPlaying() || isMouseButtonDown());

    currentPositionMarker.setRectangle(juce::Rectangle<float>(timeToX(transportSource.getCurrentPosition()) - 0.75f, 0,
        1.5f, (float)(getHeight() - scrollbar.getHeight())));
}
