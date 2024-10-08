# GDIntercept Changelog

## v0.6.7-alpha.6 (2024-10-08) - Release Migration

- Migrated the releases to the Binary Locker service to track downloads and speed up the development cycle
- Moved pages to the new deployment platform

## v0.6.6-alpha.6 (2024-10-02) - Hook Compatibility Patch

- Added compatibility for hooks that modify the request data

## v0.6.5-alpha.6 (2024-09-30) - Race Condition Patch

- Patched all remaining race conditions (closed source for security reasons)

## v0.6.4-alpha.6 (2024-09-27) - Theme Docs

- Added a theme documentation page

## v0.6.3-alpha.6 (2024-09-26) - Pause Persisting

- Added a confirmation setting to persist the pause state between sessions

## v0.6.2-alpha.6 (2024-09-25) - Minor Semantic Fixes

- Fixed the release date of the previous version
- Fixed the one-dark-pro theme targeting an old schema ID

## v0.6.1-alpha.6 (2024-09-24) - Themes Update

- Added custom theme support
- Added additional default themes
- Adjusted several nodes to allow for more customization
- Added a reloadable theme setting
- Added key and string quote tokens
- Added a persistent paused request warning
- Added a client flag to show the client used for the request
- Patched a race condition which would occur when a request was finished receiving but still being processed while reaching the request cache limit
- Patched a memory leak when a cocos request is canceled
- Fixed a bug where the bottom scrollbar would slightly scroll down the content of the code block
- Fixed a bug where opening and closing settings followed by receiving a request while the menu is still open would cause a crash
- Removed the pause requests setting in favor of an instance based pause
- Increased performance by reducing the amount of updates per frame

## v0.6.0-alpha.6 (2024-09-14) - UI v3 Update

- Replaced the small menus with a main code block menu
- Added horizontal scrolling to the code block menu
- Added a side bar with quick actions and view switching
- Added a request info view
- Added a response headers view
- Made line numbers stick to the left side of the code block
- Added a save request button
- Added more themes
- Added support for more theme customizability for the future
- Added snapping aggression to snap the scroll to a specific axis for a few update cycles
- Implemented the new settings system
- Major keybind overhaul
- Added a cancel feature to the proxy
- Added more stability for proxying geode requests

## v0.5.6-alpha.5 (2024-09-10) - ID System Implementation

- To prevent being incompatible with other geode web API mods, the ID system has been implemented

## v0.5.5-alpha.5 (2024-08-31) - Mod Size Reduction

- Removed a build setting that was causing the mod to be larger than it should be

## v0.5.4-alpha.5 (2024-08-06) - Alpha.5 Hotfix 4

- Moved some logic around to prevent accidentally deleting data before providing it to the events

## v0.5.3-alpha.5 (2024-08-06) - Alpha.5 Hotfix 3

- Added race condition protection due to some edge cases where progress may be sent to a canceled request

## v0.5.2-alpha.5 (2024-08-05) - Alpha.5 Hotfix 2

- Replaced nulls with empty strings in the form and Robtop conversions to prevent incorrect to raw conversions

## v0.5.1-alpha.5 (2024-08-05) - Alpha.5 Hotfix

- Fixed a bug where certain query strings may cause crashes

## v0.5.0-alpha.5 (2024-08-05) - Badges Update

- Added badges to the request list
- Added several API changes for better conversion support and repeat detection
- Added soft reloading support for minor state changes
- Added tooltips
- Added hovering support
- Added a new setting to disable the new badge system
- Added a copy button to the code block and info block
- Added a new setting to disable the new copy button
- Fixed thread safety for API events
- Fixed a bug where the raw data would not censor sensitive data
- Fixed crashes occurring when a non ASCII character was anywhere in the request

## v0.4.2-alpha.4 (2024-07-23) - UI Scaling Update

- Added compatibility for different aspect ratios and resolution changes

## v0.4.1-alpha.4 (2024-07-23) - Pause/Resume Safety Update

- Added checks to prevent pause/resume from accidentally triggering a recursive loop
- Fixed settings overlapping the send button once closed

## v0.4.0-alpha.4 (2024-07-22) - Optimization Update

- Added node culling to only show the visible nodes
- Added request limiting to prevent excessive RAM usage
- Removed conversion caching as they significantly increase RAM usage
- Optimized the binary parser
- Fixed scroll resets when pausing/resuming requests

## v0.3.7-alpha.3 (2024-07-13) - Stability Update V5

- Changed the way Android NDK specific objects are initialized

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
