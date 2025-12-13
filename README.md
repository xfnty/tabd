**tabd** – minimal userspace driver for drawing tablets.

Compiling using MSVC:
```bat
rc /nologo /i src /fo icon.res res\icon.rc
cl /nologo src\tabd.c src\tablet.c src\util.c src\preset.c icon.res /link /subsystem:windows /entry:_start
```

Running:
```bat
start /b /wait tabd.exe
```

Clean:
```bat
del /q /s /f *.exe *.obj *.zip *.ilk *.res *.pdb *.rdi 1> nul
```
