# GDIntercept

A GD proxy mod which allows you to intercept and modify any communication sent through the Cocos2D-X HttpClient.

## Features

- Logging and listing for all requests made by the game through the Cocos2D-X engine.
- Converting requests to the JSON format.
- Obscuring sensitive data.
- Capturing requests before they are encrypted.
- Pausing requests to modify them (Disabled in ALPHA.1).
- Modifying requests (Disabled in ALPHA.1).
- Blocking requests (Disabled in Alpha.1).
- Sending custom requests (Disabled in Alpha.1).

## Notes

This mod has been tested by doing an official penetration test on the game and forwarding all results to RobTop. This mod is intended for educational purposes only and for that reason as many server sided issues as possible will be patched before its full release.

### ALPHA.1

Request manipulation has for the time being been disabled due to security concerns and bugs. Once the official penetration test has concluded and all issues have been patched, this feature will be re-enabled.

## Credits

- [RobTop](https://www.youtube.com/@RobTopGames) - Some UI tips, providing source code and help with determining the sensitive data keys.
- [Alphalaneous](https://github.com/Alphalaneous) - Providing the scaled border fix.
- [GD Endy](https://www.youtube.com/@GDEndy) - Helping out with sprite scaling issues.
- [HJfod](https://github.com/HJfod) - Helping out with issues and geode specific quirks.
