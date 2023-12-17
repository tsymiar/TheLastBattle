@echo off
setlocal enabledelayedexpansion

echo ----------------------------------------------------
echo Clear File
echo Delete all files with ending:
echo *.aps *.idb *.ncp *.obj *.pch *.tmp *.sbr *.suo *.sdf *.suo
echo Visual c++/.Net junk 
echo ----------------------------------------------------
echo waitting 6s...
ping 127.0.0.1 -n 6 >nul

echo deleting complied files...
rmdir /s/q %~dp0ThirdFunc\boost_1_60_0
rmdir /s/q %~dp0ThirdFunc\openssl-1.1.1a

del /F /Q /S *.aps *.idb *.ncp *.obj *.pch *.sbr *.user *.tmp *.pdb *.bsc *.ilk *.res *.ncb *.opt *.suo *.manifest *.dep *.sdf *.suo

set b=.*/-\ /-\ *
set ËÙ¶È=1
set ÍË¸ñ=
:b
for /l %%i in (100,-1,1) do (
    call :a %%i
)
:a
set/a a=%1%%10
set/a c=%a%%%4
if %a% EQU 0 set/p=.<nul
if %c% EQU 3 (set/p=^|<nul) else (set/p=!b:~%a%,1!<nul)
ping -n %ËÙ¶È% 127.1>nul
set/p=%ÍË¸ñ%<nul
if errorlevel 1 (
	goto :eof
)
