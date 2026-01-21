@echo off
setlocal enableextensions enabledelayedexpansion
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set esc=%%b

for %%a in (%*) do set %%a=1
if "%msvc%"=="1" set cc=msvc
if "%tcc%"=="1" set cc=tcc
if "%gcc%"=="1" set cc=gcc
if "%cc%"=="" set cc=msvc
set project_name=tabd
set root_dir=%~dp0
set output_dir=%~dp0out\
set temp_dir=%root_dir%.ignore\
set temp_dir_cc=%temp_dir%%cc%\
set source_dir=%root_dir%src\
set exe=%output_dir%%project_name%.exe
if "%clean%"=="1" (call :Clean || exit /b 1)
if "%build%"=="1" (call :Build || exit /b 1)
if "%run%"=="1"   (call :Run   || exit /b 1)
if "%1"=="" (call :PrintUsage && exit /b 0)
exit /b 0


:PrintUsage
echo Usage: project [commands] [compiler]
echo.
echo Available commands:
echo   clean   Remove temporary files.
echo   build   Build the project.
echo   run     Run the executable.
echo.
echo Supported compilers:
echo   tcc, gcc and msvc (default).
echo.
echo Temporary directory: %temp_dir%
exit /b 0


:Clean
rmdir /q /s %output_dir% %temp_dir% 2> nul
exit /b 0


:Build
mkdir %temp_dir_cc% %output_dir% > nul 2>&1
set "compile_start=%time%"
if "%cc%"=="msvc" (
    call :BuildMSVC || exit /b 1
) else if "%cc%"=="tcc" (
    call :BuildTCC || exit /b 1
) else if "%cc%"=="gcc" (
    call :BuildGCC || exit /b 1
)
set "compile_end=%time%"
for /f "tokens=1-4 delims=:.," %%a in ("%compile_start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f "tokens=1-4 delims=:.," %%a in ("%compile_end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100
set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%ms% lss 100 set ms=0%ms%
set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%
for %%g in (%exe%) do ^
for /f "tokens=1,2" %%s in ('robocopy %%~dpg. %%~dpg. %%~nxg /l /nocopy /is /njh /njs /ndl /nc') do (
    echo Compiled down to !esc![1;96m%%s!esc![0m bytes in !esc![1;96m%totalsecs%.%ms%!esc![0m seconds.
)
exit /b 0


:BuildMSVC
set links=
for /r "%source_dir%" %%a in (*.def) do (
    set links=%%~na.lib !links!
    if not exist "%temp_dir_cc%%%~na.lib" (
        echo %%~na.lib
        lib /nologo /wx /def:"%%a" /out:"%temp_dir_cc%%%~na.lib" /machine:x64 > "%temp_dir_cc%%%~na.txt" || (
            type "%temp_dir_cc%%%~na.txt"
            exit /b 1
        )
    )
)
set unity=%temp_dir_cc%unity
set cflags=/std:c11 /Z7 /I "%source_dir%." "%unity%.c"
set oflags=/Fo:"%unity%.obj" /Fe:"%exe%"
set lflags=/link /LIBPATH:"%temp_dir_cc%" /debug /entry:_start /INCREMENTAL:NO /subsystem:windows
set compile_command=cl /nologo %cflags% !links! %oflags% %lflags%
echo /* !compile_command! */> "%unity%.c"
for /r "%source_dir%" %%a in (*.c) do echo #include "%%a">> "%unity%.c"
%compile_command% || exit /b 1
exit /b 0


:BuildTCC
set links=
for /r "%source_dir%" %%a in (*.def) do set links=-l%%~na !links!
set unity=%temp_dir_cc%unity
set compile_command=tcc -nostdlib -nostdinc -L"%source_dir%." -I"%source_dir%." -o "%exe%" "%unity%.c" !links!
echo /* %compile_command% */> "%unity%.c"
for /r "%source_dir%" %%a in (*.c) do echo #include "%%a">> "%unity%.c"
echo unity.c && %compile_command% || exit /b 1
exit /b 0


:BuildGCC
set links=
for /r "%source_dir%" %%a in (*.def) do (
    set links="%temp_dir_cc%%%~na.a" !links!
    if not exist "%temp_dir_cc%%%~na.a" (
        echo %%~na.a
        dlltool -d "%%a" -l "%temp_dir_cc%%%~na.a" > "%temp_dir_cc%%%~na.txt" || exit /b 1
    )
)
set unity=%temp_dir_cc%unity
set compile_command=gcc -nostdlib -nostdinc -std=c90 -pedantic-errors -Wl,-e,_start -L"%temp_dir_cc%." ^
-I"%source_dir%." -o "%exe%" "%unity%.c" !links!
echo /* %compile_command% */> "%unity%.c"
for /r "%source_dir%" %%a in (*.c) do echo #include "%%a">> "%unity%.c"
echo unity.c && %compile_command% || exit /b 1
exit /b 0


:Run
start "" /d "%output_dir%." /b /wait "%exe%"
if !errorlevel! neq 0 (
    echo Exited with code !esc![1;91m!errorlevel!!esc![0m
    exit /b 1
)
exit /b 0
