**tabd** is a replacement for [OpenTabletDriver][otd] I made for fun. It uses the same Windows APIs OTD does
but doesn't take half a minute to start and won't open command prompt when added to autostart.

tabd uses presets for configuration stored in a simple `.json` files under `presets` folder which is in the
same directory as `tabd.exe`. Here's my `Osu.json` preset for example:
```json
{
    "center": [80.41049, 85],
    "size": [100, 56.222546],
    "rotation": 90,
    "ink": false
}
```

The `center`, `size`, and `rotation` parameters define what part of the tablet area should be mapped to the
screen and how much it should be rotated. These are the same values OTD uses so you don't have to calculate
them yourself. `ink` either makes the driver emulate Windows Ink inputs that support pressure levels or a
normal mouse cursor.

## Supported Tablets

| Tablet        | Status             |
| :------------ | :----------------: |
| Wacom CTL-672 | :white_check_mark: |

If you want me to add another tablet, [send me an email][email] or [open an issue][support-issue].
If you want to do it yourself, see [tablets.c][tablets.c] file.

## Developing

To test the build with all compilers, run `project build-msvc run build-gcc run`.
Run `project` to get more info.

### Compiling using MSVC

1. Download [Build Tools for Visual Studio][vstools].
2. Run Visual Studio Installer and select `MSVC Build Tools for x64/x86 (Latest)` and `Windows 11 SDK` in 
   `Individual components`.
3. Enable [x64 Native Tools Command Prompt for VS][vs64] (32-bit architecture is not supported).
4. Run `project build-msvc run`.

### Compiling using GCC

1. Download and run `w64devkit-x64-X.X.X.7z.exe` from [w64devkit][w64dk]. Choose `Add to PATH` option.
2. Run `project build-gcc run`

[otd]: https://opentabletdriver.net
[email]: mailto:xfnty.x@gmail.com
[support-issue]: https://github.com/xfnty/tabd/issues/new?template=tablet-support-request.md
[tablets.c]: https://github.com/xfnty/tabd/blob/main/src/tablets.c
[vstools]: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2026
[vs64]: https://learn.microsoft.com/en-us/cpp/build/how-to-enable-a-64-bit-visual-cpp-toolset-on-the-command-line?view=msvc-170
[w64dk]: https://github.com/skeeto/w64devkit/releases/latest
