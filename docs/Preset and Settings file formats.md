# Presets

Preset format should be easily editable by hand, allow simple serialization and deserialization and,
ideally, not require third-party library.

Preset format should describe `Preset` structure.

A single preset is stored in a separate file. File name could define the name of a preset, like 
`Drawing.txt`.

Example preset with fixed schema:
```
center-x 108
center-y 67.5
width    216
height   135
rotation 0
mode     ink
```

Serialization can be done with a single `printf`.

Deserialization can be done with the following algorithm:
1. Skip any amount of whitespace.
2. Parse string until whitespace and remember it as key.
3. Try binding known key to `Preset`'s field by pointer and field offset, if key is invalid abandon 
   the file.
3. Skip one or more whitespace.
4. Parse string with whitespace until the last non-whitespace character as bound value type and 
   store it in struct's field by pointer.
5. Skip any amount of whitespace.
6. Skip newline.

Values can be one of the following:
- Floats
- Strings
- Enums

Enums are strings without whitespace that are deserialized as integers.

At the beginning Preset struct is initialized with default values making all of the keys optional.

# Settings

Settings could be stored in a file with the following format:
```
autostart false
presets   C:\Users\My User\AppData\Local\tabd\presets
logs      C:\Users\My User\AppData\Local\tabd\logs
```

The file itself should be put in either `%APPDATA%\Local\tabd` folder or alongside the exe.
