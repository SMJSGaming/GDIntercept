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

## Credits

- [RobTop](https://www.youtube.com/@RobTopGames) - Some UI tips, providing source code and help with determining the sensitive data keys.
- [Alphalaneous](https://github.com/Alphalaneous) - Providing the scaled border fix.
- [GD Endy](https://www.youtube.com/@GDEndy) - Helping out with sprite scaling issues.
- [HJfod](https://github.com/HJfod) - Helping out with issues and geode specific quirks.
- [Jasmine](https://github.com/hiimjasmine00) - Finding the cause of the Android crashes.
