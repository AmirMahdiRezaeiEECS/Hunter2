#!/usr/bin/env bash
# Builds android/dist/robot-controller-debug.apk with the Android SDK
# command-line tools (no Gradle project required).
set -euo pipefail
cd "$(dirname "$0")"

SDK="${ANDROID_HOME:-${ANDROID_SDK_ROOT:-$HOME/Android/Sdk}}"
BT="$(ls -d "$SDK"/build-tools/* 2>/dev/null | sort -V | tail -1)"
PLATFORM="$(ls -d "$SDK"/platforms/* 2>/dev/null | sort -V | tail -1)"
JARS="$(ls "$SDK"/platforms/android-*/android.jar 2>/dev/null | sort -V | tail -1)"

command -v java >/dev/null || { echo "JDK required (java not found)"; exit 1; }
[[ -n "$BT" && -n "$JARS" ]] || { echo "Android SDK build-tools + platform required. Set ANDROID_HOME."; exit 1; }
echo "build-tools: $BT"
echo "platform: $PLATFORM"

WORK=build
OUT=dist
rm -rf "$WORK" "$OUT"
mkdir -p "$WORK/classes" "$WORK/apk" "$OUT"

"$BT/aapt2" compile --dir res -o "$WORK/res.zip"
"$BT/aapt2" link -o "$WORK/unsigned.apk" -I "$JARS" --manifest AndroidManifest.xml \
  "$WORK/res.zip" --java "$WORK/src" -A assets
javac --release 11 -cp "$JARS" -d "$WORK/classes" java/com/eecs/hunter/MainActivity.java "$WORK/src/com/eecs/hunter/R.java"
"$BT/d8" --release --lib "$JARS" --min-api 21 --output "$WORK/apk" "$WORK/classes/com/eecs/hunter/MainActivity.class" \
  "$WORK/classes/com/eecs/hunter/MainActivity\$Bridge.class"
cp "$WORK/unsigned.apk" "$WORK/apk/base.apk"
(cd "$WORK/apk" && zip -ju base.apk classes.dex >/dev/null)
"$BT/zipalign" -f 4 "$WORK/apk/base.apk" "$WORK/aligned.apk"
KS="$WORK/debug.keystore"
if [[ ! -f "$KS" ]]; then
  keytool -genkeypair -keystore "$KS" -storepass android -keypass android -alias androiddebugkey \
    -dname "CN=Android Debug,O=Android,C=US" -keyalg RSA -keysize 2048 -validity 10000 >/dev/null 2>&1
fi
"$BT/apksigner" sign --ks "$KS" --ks-pass pass:android --key-pass pass:android \
  --out "$OUT/robot-controller-debug.apk" "$WORK/aligned.apk"
echo "Built $OUT/robot-controller-debug.apk"
