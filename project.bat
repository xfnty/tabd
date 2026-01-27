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
    ) else if "%%a"=="build-gcc" (
        call :BuildGCC || exit /b 1
    ) else if "%%a"=="run" (
        call :Run || exit /b 1
    ) else (
        echo Unknown command !esc![1;91m%%a!esc![0m
        exit /b 1
    )
)
exit /b 0


:PrintUsage
echo Usage: project [command...]
echo.
echo Available commands:
echo   clean      Remove temporary files.
echo   build-msvc Build using MSVC toolchain (https://visualstudio.microsoft.com/downloads).
echo   build-gcc  Build using MinGW toolchain (https://github.com/skeeto/w64devkit).
echo   run        Run the executable.
echo.
echo Temporary directory: %temp_dir%
exit /b 0


:Clean
rmdir /q /s %output_dir% %temp_dir% 2> nul
exit /b 0

:BuildMSVC
echo !esc![1;90mCompiling using !esc![1;34mMSVC!esc![0m
set temp_dir_cc=%temp_dir%msvc\
mkdir %output_dir% !temp_dir_cc! 2> nul
set links=
for /r "%source_dir%" %%a in (*.def) do (
    set links=%%~na.lib !links!
    call :Generate "%%a" "!temp_dir_cc!%%~na.lib" && (
        lib /nologo /wx /def:"%%a" /out:"!temp_dir_cc!%%~na.lib" /machine:x64 > "!temp_dir_cc!%%~na.txt" || (
            type "!temp_dir_cc!%%~na.txt"
            exit /b 1
        )
    )
)
call :Generate "%root_dir%res\icon.rc" "!temp_dir_cc!icon.res" && (
    rc /nologo /i "%source_dir%." /fo "!temp_dir_cc!icon.res" "%root_dir%res\icon.rc" || exit /b 1
)
set unity=!temp_dir_cc!unity
set cmd=cl /nologo /std:c11 /GS- /Z7 /Wall /wd4820 /wd4255 /wd5250 /wd5045 ^
/I "%source_dir%." "%unity%.c" "!temp_dir_cc!icon.res" !links! /Fo:"%unity%.obj" /Fe:"%exe%" ^
/link /LIBPATH:"!temp_dir_cc!." /debug /entry:_start /INCREMENTAL:NO /NODEFAULTLIB /subsystem:windows
call :GenerateUnity "%unity%.c" "!cmd!"
%cmd% || exit /b 1
exit /b 0


:BuildGCC
echo !esc![1;90mCompiling using !esc![1;34mGCC!esc![0m
set temp_dir_cc=%temp_dir%gcc\
mkdir %output_dir% !temp_dir_cc! 2> nul
set links=
for /r "%source_dir%" %%a in (*.def) do (
    set links="!temp_dir_cc!%%~na.a" !links!
    call :Generate "%%a" "!temp_dir_cc!%%~na.a" && (
        dlltool -d "%%a" -l "!temp_dir_cc!%%~na.a" > "!temp_dir_cc!%%~na.txt" || exit /b 1
    )
)
call :Generate "%root_dir%res\icon.rc" "!temp_dir_cc!icon.o" && (
    windres -I"%source_dir%." "%root_dir%res\icon.rc" -o "!temp_dir_cc!icon.o"
)
set unity=!temp_dir_cc!unity
set cc=gcc -nostdlib -nostdinc -std=c90 -Wall -Wextra -pedantic -pedantic-errors -Wl,-e,_start ^
-L"!temp_dir_cc!." -I"%source_dir%." -o "%exe%" "%unity%.c" "!temp_dir_cc!icon.o" !links!
call :GenerateUnity "%unity%.c" "%cc%"
echo unity.c && %cc% || exit /b 1
exit /b 0


:Run
echo !esc![1;90mRunning !esc![1;34m%project_name%.exe!esc![0m
start "" /d "%output_dir%." /b /wait "%exe%"
if !errorlevel! neq 0 (
    echo !esc![1;90mExited with code !esc![1;31m!errorlevel!!esc![0m
)
exit /b 0


:IsFileNewerThan :: source destination
if not exist "%2" exit /b 0
xcopy /D /L /Y "%1" "%2" | FINDSTR /E /C:"%~nx1" >nul && exit /b 0
exit /b 1


:GenerateUnity :: destination "compile_command"
echo /* %~2 */> %1
for /r "%source_dir%" %%a in (*.c) do echo #include "%%a">> %1
exit /b 0


:Generate :: source destination
call :IsFileNewerThan %1 %2 || exit /b 1
echo !esc![1;90mGenerating %~nx2!esc![0m
exit /b 0
