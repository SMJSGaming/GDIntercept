# GDIntercept

A GD proxy mod which allows you to intercept and modify any communication sent through the Cocos2D-X HttpClient.

## API

Check the [API documentation](https://github.com/SMJSGaming/GDIntercept/wiki).

## Features

- Logging and listing all requests made by the game through the Cocos2D-X engine.
- Logging and listing all requires made by the geode web client.
- Converting requests to a JSON format.
- Obscuring sensitive data.
- Capturing requests before they are encrypted using SSL.
- Pausing requests to modify them.
- Sending custom requests.
- Built in request and response API for other mods.
- Modifying requests (Disabled in ALPHA).
- Blocking requests.

## Notes

This mod has been tested by doing an official penetration test on the game and forwarding all results to RobTop. This mod is intended for educational purposes only and for that reason as many server sided issues as possible will be patched before its full release.

### ALPHA.1

Request manipulation has for the time being been disabled due to security concerns and bugs. Once the official penetration test has concluded and all issues have been patched, this feature will be re-enabled.

### ALPHA.2

This version has yet to enable request manipulation due to the official penetration test still being in progress. This version mostly aims to simplify the use of settings and fixing a lot of bugs. Some features requested by the community have also been added.

### ALPHA.3

Some minor request manipulation features have been re-enabled. This version also adds Geode and API support. This means that in the future GDIntercept will also be useable for other mods. Check [the wiki](https://github.com/SMJSGaming/GDIntercept/wiki) for more details.

### ALPHA.4

This version significantly improves the performance of the mod and removes as much RAM usage as possible. The exposed API path has also been fixed.

### ALPHA.5

This version contains a lot of reworks in the background which are needed for the final release. The only thing remaining is the input support for the code block and the final stage of the official penetration test.

### ALPHA.6

This version overhauls the UI and adds a lot of new features. The official Geometry Dash penetration test has also concluded and all issues have been patched. This means that request manipulation will be re-enabled in the next version.

## Credits

- [RobTop](https://www.youtube.com/@RobTopGames) - Some UI tips, providing source code and help with determining the sensitive data keys.
- [Alphalaneous](https://github.com/Alphalaneous) - Providing the scaled border fix.
- [GD Endy](https://www.youtube.com/@GDEndy) - Helping out with sprite scaling issues.
- [HJfod](https://github.com/HJfod) - Helping out with issues and geode specific quirks.
- [Justin](https://github.com/hiimjustin000) - Finding the cause of the Android crashes.
