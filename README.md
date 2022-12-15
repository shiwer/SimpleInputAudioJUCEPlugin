# SimpleInputAudioJUCEPlugin
JUCE-powered audio plugin for playing audio files for test environments.

Based on Demo files that you can find on the JUCE project and on the Github project: https://github.com/jonathonracz/AudioFilePlayerPlugin.

If you're using Visual Studio Code you might need to :
1- go to the Solution Explorer
1- go into the Source folder of the SimpleAudioInputPlugin
1- Right click and add both Existing items 
	- AudioThumbnailComp.h
	- AudioThumbnailComp.cpp

This is an updated functional version to have in your AudioPluginHost just a drag and drop music file reader.
It :
- Plays audio files of various formats
- Lightweight, with none of the frills of a full fledged sampler
- Open source
- Capable of loading/saving parameters including the audio file loaded
- Cross platform.

!(example)[screenshot.png]