**tabd** is a replacement for [OpenTabletDriver][otd] I made for fun. It uses the same Windows APIs OTD does
but doesn't take half a minute to start and won't open command prompt when added to autostart.

tabd uses presets for configuration stored in a simple text files under `presets` folder which is in the same directory as
`tabd.exe`. Here's my `Osu.txt` preset for example:
```
cx 80.41049
cy 85
w  100
h  56.222546
r  -90
m  mouse
```

The `cx`, `cy`, `w`, `h` and `r` parameters define what part of the tablet area should be mapped to the screen
and how much it should be rotated. These are the same values OTD uses so you don't have to calculate them
yourself. `m` however can be either `mouse`, which makes the driver emulate normal mouse cursor, or `ink`
which emulates Windows Ink inputs that support pressure levels.

## Supported Tablets

| Tablet        | Status             |
| :------------ | :----------------: |
| Wacom CTL-672 | :white_check_mark: |

If you want me to add another tablet, [send me an email][email] or [open an issue][support-issue].
If you want to do it yourself, see [tablets.c][tablets.c] file.

## Developing

If you don't have compiler installed, use [build steps for TCC][tcc-steps].

### Compiling using MSVC

1. Download [Build Tools for Visual Studio][vstools].
2. Run Visual Studio Installer and select `MSVC Build Tools for x64/x86 (Latest)` in `Individual components`.
3. Enable [x64 Native Tools Command Prompt for VS][vs64] (32-bit architecture is not supported).
4. Run `project build-msvc run`.

### Compiling using Tiny C Compiler

1. Download [TCC][tcc] and extract it to any folder.
2. Open `Settings > System > About > Advanced system settings (Related settings) > Environment Variables... > System variables > Path > Edit... > New`
   and paste there path to the extracted folder containing `tcc.exe`.
4. Run `project build-tcc run`.

### Compiling using GCC

1. Download and run `w64devkit-x64-X.X.X.7z.exe` from [w64devkit][w64dk]. Choose `Add to PATH` option.
2. Run `project build-gcc run`

[otd]: https://opentabletdriver.net
[email]: mailto:xfnty.x@gmail.com
[support-issue]: https://github.com/xfnty/tabd/issues/new?template=tablet-support-request.md
[tablets.c]: https://github.com/xfnty/tabd/blob/main/src/tablets.c
[tcc-steps]: https://github.com/xfnty/tabd?tab=readme-ov-file#compiling-using-tiny-c-compiler
[vstools]: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2026
[vs64]: https://learn.microsoft.com/en-us/cpp/build/how-to-enable-a-64-bit-visual-cpp-toolset-on-the-command-line?view=msvc-170
[tcc]: https://bellard.org/tcc/
[w64dk]: https://github.com/skeeto/w64devkit/releases/latest
