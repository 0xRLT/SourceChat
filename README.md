# Source Chat (BETA)
Provides a complete rehaul of the Sven-Coop chat to a Source Engine alike chat including it's features (chat history, selection, scrolling, colors, etc.)

![](https://github.com/0Reality/SourceChat/blob/main/preview/preview.gif)

SourceChat is a plugin made for [SvenMod](https://github.com/sw1ft747/svenmod "SvenMod") by [Sw1ft](https://github.com/sw1ft747 "SvenMod") and me.
It would not have been so feaseable to do it without SvenMod, so check it out!

# Config
When the plugin is loading, it will execute file `sourcechat.cfg` from folder `../Sven Co-op/svencoop/`, you can use this config file to save some console variables for customization

# Console Commands
ConVar | Default Value | Type | Description
--- | --- | --- | ---
sourcechat | 1 | bool | Enable/disable Source-like chat
sourcechat_width_fraction | 0.0115 | float | Screen's fraction of width
sourcechat_height_fraction | 0.5732 | float | Screen's fraction of height
sourcechat_fadein_duration | 0.3 | float | Fade-in duration of chatbox
sourcechat_fadeout_duration | 0.3 | float | Fade-out duration of chatbox
sourcechat_text_stay_time | 10 | float | Stay time of recent message
sourcechat_text_fade_duration | 2 | float | Fade-out duration of recent message
sourcechat_monsterinfo_width_fraction | 1 | float | Text message's width fraction of monster info
sourcechat_monsterinfo_height_fraction | 0.8 | float | Text message's height fraction of monster info

# Console Commands
ConCommand | Description
--- | --- | ---
sourcechat_clear | Clear Source-like chat

# Known issues
* The chat has been done for 1080p resolution and not adaptive for smaller or bigger ones resolutions, it means small screens the chatbox will be too big for you and vice versa for big screens
* Text fading is not finished yet, it may appear buggy
* Sometimes selected text in the history box shifts to the left (same for input line)
* Some fade in/out durations can cause the chatbox to flicker
* In rare cases (or after first installation) there may appear artifacts, it can be fixed after restarting the PC

# Installation
You will first need to download and install [SvenMod](https://github.com/sw1ft747/svenmod "SvenMod")
then download a release from [Releases](https://github.com/sw1ft747/svenmod "Releases")
and place all files from the archive in the root folder of the game. 
Next, add the plugin (sourcechat.dll) to the file plugins.txt (see the header Adding plugins in SvenMod's [readme](https://github.com/sw1ft747/svenmod)). .
