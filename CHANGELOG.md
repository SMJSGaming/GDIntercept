# GDIntercept Changelog

## v0.3.6-alpha.3 (2024-07-13) - Stability Update V4

- Removed the use of CCHttpClient threading because either Android breaks from it or the Android fix breaks windows

## v0.3.5-alpha.3 (2024-07-13) - Stability Update V3

- Fixed cocos getting so trigger happy that it cleaned up a request before it could even be read

## v0.3.4-alpha.3 (2024-07-13) - Stability Update V2

- Fixed an issue where cocos requests would be released before the thread was done with them

## v0.3.3-alpha.3 (2024-07-13) - Stability Update

- Changed the way per frame updates are handled to prevent crashes

## v0.3.2-alpha.3 (2024-07-12) - Cancel Support

- Added support for cancelling requests

## v0.3.1-alpha.3 (2024-07-12) - Bugfix Update

- Fixed a bug where certain requests would cause an unallocated reference exception
- Optimized the content loading

## v0.3.0-alpha.3 (2024-07-12) - API, Geode and Request Flow Support

- Added support for the Geode web API
- Added support for request pausing, resuming and resending
- Added API support for other mods
- Added a lot more keybinds
- Added a filter setting
- Fixed several bugs

## v0.2.3-alpha.2 (2024-06-25) - More Android Fixes

- Fixed Android having a lot of touch priority issues

## v0.2.2-alpha.2 (2024-06-23) - Binary Update

- Added binary support
- Optimized text processing
- Fixed binary being attempted to be rendered as forms
- Added cache support
- Made the setting descriptions clearer

## v0.2.1-alpha.2 (2024-06-22) - Android Fix

- Added a failsafe for Android devices

## v0.2.0-alpha.2 (2024-06-22) - Optimizations, QOL and Redesign

- Added a direct link to the Geode settings menu
- Fixed a ton of issues
- Optimized the JSON parser
- Made theme selecting easier
- Added an option to log requests

## v0.1.2-alpha.1 (2024-06-20) - Bugfix Update

- Fixed some bugs with the parsing of the response data
- Fixed some UI bugs
- Added better MacOS support

## v0.1.1-alpha.1 (2024-06-20) - Compatibility Update

- Made the mod compatible with other mods on the menu layer
- Added MacOS support

## v0.1.0-alpha.1 (2024-06-19) - Initial Release

- Added all the basic interface features
- Added some useful keybinds
- Introduced Android/Windows support
- Added data obscuring
- Added JSON conversions
- Added a readonly monaco editor
- Added request caching
