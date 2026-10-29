# Reproduce the Android timezone reset thread

This branch uses the unpatched 0.5.0 native library with React Native 0.88.0.
The example has one screen. It compares Android's current UTC offset with
`Date.getTimezoneOffset()` in Hermes. Both values use minutes west of UTC.

## Build

From the repository root, install with Bun. These local links give Gradle
the example's React Native tools instead of the root library's older tools.
The existing Yarn lockfile belongs to the original 0.87 example.

```sh
bun install --no-save --ignore-scripts --linker hoisted
ln -s ../react-native/node_modules/@react-native/gradle-plugin example/node_modules/@react-native/gradle-plugin
ln -s ../react-native/node_modules/@react-native/codegen example/node_modules/@react-native/codegen
ln -s react-native/node_modules/hermes-compiler example/node_modules/hermes-compiler
cd example/android
./gradlew assembleRelease -PreactNativeArchitectures=arm64-v8a
adb install -r app/build/outputs/apk/release/app-release.apk
```

Set `ANDROID_HOME` to your Android SDK directory before the build. The APK
includes its JS bundle, so Metro is not needed. Open the installed example.

## Reproduce

1. Disable automatic timezone selection in Android Settings.
2. Choose `Europe/Bucharest`, then launch the example.
3. Confirm that the native and Hermes offsets match.
4. Open Settings and change the timezone to `America/Los_Angeles`.
5. Return to the same app process and tap **Read again**.
6. Inspect the native log and process threads with the commands below.

```sh
adb shell pidof timezonehermesfix.example
adb logcat -d -v threadtime -s TimezoneHermesFix
adb shell ps -T -p <pid>
```

The log shows the process ID and thread ID before the tag. If they are equal,
`resetTzHermes` ran on the process's main thread. Compare that thread ID with
`mqt_v_js` in the thread list. The cache reset must run on the JS thread.

The offsets and last library event show whether the reset changed the date
values. A `MATCH` does not establish that the reset used the correct thread.
The button reads both offsets again, independently of the library's event.

Restore the original timezone and automatic timezone setting afterward.
The existing Maestro flow targets the original demo and its fixed date.
It does not drive this reproduction screen.

## Observed result

On a Samsung SM-G985F with Android 13, both React Native 0.87.0 and 0.88.0
updated the offsets and emitted the new zone with unpatched library 0.5.0.
The app process stayed alive. No stale dates or crash occurred in these runs.

On 0.88.0, the reset log used process ID `14953` and thread ID `14953`.
The same process's JS thread, `mqt_v_js`, had thread ID `14986`.
The reproduced problem is the main-thread runtime access.
