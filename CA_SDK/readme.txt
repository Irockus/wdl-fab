On Xcode4+ you may need to manually download the CoreAudio SDK. 
Currently you can get it here
	https://developer.apple.com/library/mac/samplecode/CoreAudioUtilityClasses/Introduction/Intro.html
	(click download sample code)

Experimental (needs testing!):

Option 1. Update XCode4/5 directly (no need for extra AU folder in that distrib!)
=============================================================================
	You need to right click on the XCode app and select "Show package contents"...
	Then in 'Contents/Developer', add an 'Extras' folder and 
	drag and drop the CoreAudio root folder there -> Done.
	(i.e. /Applications/Xcode.app/Contents/Developer/Extras/CoreAudio/)
	
Option 2. (WIP)
===============
Extract CoreAudioUtilityClasses.zip and place the contents of the CoreAudio Folder here, i.e.
./AudioCodecs
./AudioFile
./AudioUnits
./PublicUtility

(TBC: add a populated CMakeLists.txt file there and handle it if user chooses that method ...)