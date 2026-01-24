@echo off
setlocal enableextensions enabledelayedexpansion
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set esc=%%b

set project_name=tabd
set root_dir=%~dp0
set output_dir=%~dp0out\
set temp_dir=%TEMP%\%project_name%\
set source_dir=%root_dir%src\
set exe=%output_dir%%project_name%.exe

if "%1"=="" (call :PrintUsage && exit /b 0)
for %%a in (%*) do (
    if "%%a"=="clean" (
        call :Clean || exit /b 1
    ) else if "%%a"=="build-msvc" (
        call :BuildMSVC || exit /b 1
    ) else if "%%a"=="build-tcc" (
        call :BuildTCC || exit /b 1
    ) else if "%%a"=="build-gcc" (
        call :BuildGCC || exit /b 1
    ) else if "%%a"=="run" (
        call :Run || exit /b 1
    )
)
exit /b 0


:PrintUsage
echo Usage: project [command...]
echo.
echo Available commands:
echo   clean      Remove temporary files.
echo   build-msvc Build using MSVC Compiler (https://visualstudio.microsoft.com/downloads).
echo   build-tcc  Build using Tiny C Compiler (https://bellard.org/tcc).
echo   build-gcc  Build using GNU C Compiler (https://github.com/skeeto/w64devkit).
echo   run        Run the executable.
echo.
echo Temporary directory: %temp_dir%
exit /b 0


:Clean
rmdir /q /s %output_dir% %temp_dir% 2> nul
exit /b 0

:BuildMSVC
set temp_dir_cc=%temp_dir%msvc\
mkdir %output_dir% !temp_dir_cc! 2> nul
set links=
for /r "%source_dir%" %%a in (*.def) do (
    set links=%%~na.lib !links!
    set should_generate=0
    if not exist "!temp_dir_cc!%%~na.lib" set should_generate=1
    if "!should_generate!"=="0" (
        xcopy /D /L /Y "%%a" "!temp_dir_cc!%%~na.lib" | FINDSTR /E /C:".def" >nul
        if !errorlevel! equ 0 set should_generate=1
    )
    if "!should_generate!"=="1" (
        echo %%~na.lib
        lib /nologo /wx /def:"%%a" /out:"!temp_dir_cc!%%~na.lib" /machine:x64 > "!temp_dir_cc!%%~na.txt" || (
            type "!temp_dir_cc!%%~na.txt"
            exit /b 1
        )
    )
)
set unity=!temp_dir_cc!msvc-unity
set cflags=/std:c11 /GS- /Z7 /I "%source_dir%." "%unity%.c"
set oflags=/Fo:"%unity%.obj" /Fe:"%exe%"
set lflags=/link /LIBPATH:"!temp_dir_cc!" /debug /entry:_start /INCREMENTAL:NO /NODEFAULTLIB ^
/subsystem:windows
set compile_command=cl /nologo %cflags% !links! %oflags% %lflags%
echo /* !compile_command! */> "%unity%.c"
for /r "%source_dir%" %%a in (*.c) do echo #include "%%a">> "%unity%.c"
%compile_command% || exit /b 1
exit /b 0


:BuildTCC
set temp_dir_cc=%temp_dir%tcc\
mkdir %output_dir% !temp_dir_cc! 2> nul
set links=
for /r "%source_dir%" %%a in (*.def) do set links=-l%%~na !links!
set unity=!temp_dir_cc!tcc-unity
set compile_command=tcc -nostdlib -nostdinc -L"%source_dir%." -I"%source_dir%." -o "%exe%" "%unity%.c" !links!
echo /* %compile_command% */> "%unity%.c"
for /r "%source_dir%" %%a in (*.c) do echo #include "%%a">> "%unity%.c"
echo tcc-unity.c && %compile_command% || exit /b 1
exit /b 0


:BuildGCC
set temp_dir_cc=%temp_dir%gcc\
mkdir %output_dir% !temp_dir_cc! 2> nul
set links=
for /r "%source_dir%" %%a in (*.def) do (
    set links="!temp_dir_cc!%%~na.a" !links!
    set should_generate=0
    if not exist "!temp_dir_cc!%%~na.lib" set should_generate=1
    if "!should_generate!"=="0" (
        xcopy /D /L /Y "%%a" "!temp_dir_cc!%%~na.lib" | FINDSTR /E /C:".def" >nul
        if !errorlevel! equ 0 set should_generate=1
    )
    if "!should_generate!"=="1" (
        echo %%~na.a
        dlltool -d "%%a" -l "!temp_dir_cc!%%~na.a" > "!temp_dir_cc!%%~na.txt" || exit /b 1
        lib /nologo /wx /def:"%%a" /out:"!temp_dir_cc!%%~na.lib" /machine:x64 > "!temp_dir_cc!%%~na.txt" || (
            type "!temp_dir_cc!%%~na.txt"
            exit /b 1
        )
    )
)
set unity=!temp_dir_cc!gcc-unity
set compile_command=gcc -nostdlib -nostdinc -std=c90 -pedantic-errors -Wl,-e,_start -L"!temp_dir_cc!." ^
-I"%source_dir%." -o "%exe%" "%unity%.c" !links!
echo /* %compile_command% */> "%unity%.c"
for /r "%source_dir%" %%a in (*.c) do echo #include "%%a">> "%unity%.c"
echo gcc-unity.c && %compile_command% || exit /b 1
exit /b 0


:Run
start "" /d "%output_dir%." /b /wait "%exe%"
if !errorlevel! neq 0 (
    echo Exited with code !esc![1;91m!errorlevel!!esc![0m
    exit /b 1
)
exit /b 0
