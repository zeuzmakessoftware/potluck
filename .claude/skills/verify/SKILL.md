---
name: verify-potluck
summary: Launch and drive the native raylib game on macOS, capturing frames with raylib.
---

# Verify Potluck

1. Build the current app with `make build`.
2. Launch it with an isolated home directory so saves/preferences stay out of the real profile:
   ```sh
   verify_home=$(mktemp -d /tmp/potluck-verify.XXXXXX)
   HOME="$verify_home" ./build/dev/potluck > /tmp/potluck-verify.log 2>&1 &
   game_pid=$!
   ```
3. Use `$game_pid` to target and later stop the isolated process; do not use `pgrep`, which can select another Potluck window.
4. Drive keyboard input with a short Swift/CoreGraphics script using `CGEvent(...).postToPid(pid)`. Useful macOS virtual key codes:
   - N `45`, C `8`, S `1`, I `34`, R `15`, Escape `53`
   - Left `123`, Right `124`
   - F5 `96`, F9 `101`, F12 `111`
5. F12 uses raylib's built-in capture and writes `screenshotNNN.png` in the repository root. Read the image to verify the rendered frame, then remove the capture before finishing.
6. Typical feature path: N → Escape (skip intro) → Escape (pause) → S (settings). Settings shortcuts are I for inversion, Left/Right for speed, R for defaults, and Escape for back.
7. For comparable camera captures, quick-load with F9 to reset the camera, wait about three seconds for smoothing to settle, apply the same held arrow input, then capture with F12.

macOS terminal screen-capture permission is not required because raylib captures its own framebuffer.
