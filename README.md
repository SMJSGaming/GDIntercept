# GDIntercept

A GD proxy mod which allows you to intercept any communication sent through the Cocos2D-X HttpClient & Geode web client.

## Features

- Logging and listing all requests made by the game through the Cocos2D-X engine.
- Logging and listing all requires made by the Geode web client.
- Converting requests to a JSON format.
- Partially translating RobTop response formats.
- Decoding RobTop response formats.
- Obscuring sensitive data.
- Capturing requests before they are encrypted using SSL.
- Pausing requests.
- Blocking requests.
- Translating requests to Curl commands.
- Logging requests.
- A VSCode like view into the request.
- Lots of default themes.
- Custom themes.

## Notes

This mod has been tested by doing an official penetration test on the game and forwarding all results to RobTop. This mod is intended for educational purposes only and for that reason as many server sided issues as possible have been patched before its full release.

## Credits

- [RobTop](https://www.youtube.com/@RobTopGames) - Some UI tips, providing some source code and help with determining the sensitive data keys.
- [Alphalaneous](https://github.com/Alphalaneous) - Providing the scaled border fix.
- [GD Endy](https://www.youtube.com/@GDEndy) - Helping out with sprite scaling issues.
- [HJfod](https://github.com/HJfod) - Helping out with issues and geode specific quirks.
- [Jasmine](https://github.com/hiimjasmine00) - Finding the cause of the Android crashes.
