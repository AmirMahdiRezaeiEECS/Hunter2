# Android App — Robot Controller (APK)

An installable Android app wrapper for the dual-joystick Robot Controller web UI. The
controller page runs inside a native WebView, so it installs like a normal app from the
home screen — no browser needed, and it also avoids Chrome's secure-origin rules that
block installing a PWA over the ESP8266's plain-HTTP server.

The app sends exactly the same commands as the web page:

    GET /control?left=-100..100&right=-100..100

where `left`/`right` are percentages for the differential-drive motors
(release both sticks → `0,0`; app paused → forced stop is also sent).

- Package: `com.eecs.hunter`
- Min Android: 5.0 (API 21), Target: Android 14 (API 34)
- Screen stays on while driving, landscape orientation, plain-HTTP allowed
  (`usesCleartextTraffic`) so it can talk to the ESP8266 directly.

## Install the prebuilt APK

1. Copy `android/dist/robot-controller-debug.apk` to your phone (or open this page on
   the phone and download it).
2. Tap the APK, allow "Install unknown apps" for the file manager if asked, install.
3. It appears on the home screen as **Robot Controller**.

It is signed with a generated debug key (this is normal for sideloading). If Play
Protect warns because the app is not from the Play Store, choose install anyway.

## First launch

1. The app asks for the **robot address** once. Enter the ESP8266's base URL, e.g.
   `http://192.168.4.1` (AP mode) or `http://<STA-IP>` — the IP printed on the serial
   monitor after the board connects. A scheme is optional (`192.168.4.1` works too);
   it is saved and reused on every launch.
2. The phone must be on the same Wi-Fi network as the robot (or connected to the
   robot's own access point).
3. To change the address later, tap the **☰** button in the top-right corner.

> Note: `nodemcu_car.ino` in this repository only implements the older
> `/car?a=1..9` button interface. The controller needs the `/control?left=&right=`
> endpoint on the ESP8266 firmware side.

## Rebuilding the APK yourself

Requires a JDK and the Android SDK command-line tools with one platform and matching
build-tools (no Gradle project needed):

```bash
export ANDROID_HOME=/path/to/android-sdk
cd android
bash build-apk.sh     # produces dist/robot-controller-debug.apk
```

The controller UI lives in `android/assets/index.html`. It is the same file as the
browser version, plus three small additions: the robot base URL is read from/saved to
native preferences via a tiny JS bridge (`window.Android`), the ☰ button opens the
address dialog, and `window.__stopDrive()` is invoked by the app when it goes to the
background so the car stops even if the page's fallback is not reached.

Layout:

    android/
    ├── AndroidManifest.xml          app metadata, permissions, fullscreen/no-title theme
    ├── java/com/eecs/hunter/MainActivity.java   WebView activity + address dialog + JS bridge
    ├── assets/index.html            the controller UI (this is the app)
    ├── res/                         launcher icons (regenerate: node gen-icons.js)
    └── build-apk.sh                 aapt2 + javac + d8 + apksigner build script
