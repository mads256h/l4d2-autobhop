# l4d2-autobhop
 This cheat makes you autobhop when you hold down the spacebar in Left 4 Dead 2.

## Disclaimer
 I am not responsible for bans that are a result of using this software. If you choose to use this software and you get banned its on you not me. 

## Downloading
 Go to actions and select the first one in the list. Then there should be a list of downloads. I recommend the Release x86 one, but all of them work. Don't use the Debug ones, unless you experience errors, as the Debug build run a lot slower.

## Running
 `l4d2-autobhop <playerbase> <mFlags offset>`
 
 I use these offsets, but they can change if the game recieves an update:
 ```
 player base = 0x6FD9D8
 mFlags offset = 0xF0 
 ```
 If these doesnt work you can find other ones on the unknowncheats forum.
 
 This is the commandline i use:
 
 `l4d2-autobhop 0x6FD9D8 0xF0`

## Building 
 Open this project in Visual Studio 2019. Select the configuration and platform and hit build.
