@echo off

set BATDIR=%~dp0

echo ----------------------------------------------------
echo Setup libmysql files
echo ----------------------------------------------------
if not exist "%BATDIR%ThirdFunc\libmysql\" (
    md %BATDIR%ThirdFunc\libmysql\
)
if not exist "%BATDIR%ThirdFunc\libmysql\libmysql.lib" (
    bitsadmin.exe /transfer libmysql /download /priority normal "https://downloads.mysql.com/archives/get/file/mysql-connector-c-6.1.11-win32.zip" "%BATDIR%ThirdFunc\libmysql\libmysql.zip"
    cd %BATDIR%ThirdFunc\libmysql
    expand libmysql.zip -F:libmysql.* .
    explorer libmysql.zip
)
color e4
echo Waitting for unzip libmysql.lib(.dll) to %BATDIR%ThirdFunc\libmysql.
pause
color 07
cd %BATDIR%

echo ----------------------------------------------------
echo Compile boost libraries
echo ----------------------------------------------------
cd lib-src
if not exist %BOOST% (
:set /p BOOST="set boost location:\n"
:if "%BOOST%"=="" set BOOST=%BATDIR%ThirdFunc\boost_1_60_0
    7za x boost_1_60_0.7z -o%BATDIR%ThirdFunc
)
set BOOST=%BATDIR%ThirdFunc\boost_1_60_0
cd %BOOST%
if not exist bjam (
call bootstrap.bat
)
bjam stage architecture=x86 address-model=32 --with-atomic --with-math --with-date_time --with-thread --with-filesystem --with-system link=static runtime-link=shared threading=multi
cd %BATDIR%

echo ----------------------------------------------------
echo Compile openssl library
echo ----------------------------------------------------
cd ThirdFunc\openssl-1.1.1a
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"

perl Configure VC-WIN32 --prefix=%BATDIR%ThirdFunc\openssl-1.1.1a
if errorlevel 1 (
    echo fail to set perl Configure.
    cd %BATDIR%
    goto :eof
)
nmake && nmake install
if errorlevel 1 (
    echo fail to make/install ntlib.
)
cd %BATDIR%

"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" ./AllServer.sln /t:build /p:configuration="Debug" /m
if errorlevel 1 (
    echo fail to exec Community MSBuild.
    goto :eof
)

echo ----------------------------------------------------
echo Copy files to Bin
echo ----------------------------------------------------
if not exist Bin md Bin
copy /y %BATDIR%Debug\*.exe %BATDIR%Bin\
copy /y %BATDIR%Lib\Win32\Debug\*.dll %BATDIR%Bin\
copy /y %BATDIR%ThirdFunc\libmysql\libmysql.dll %BATDIR%Bin\
copy /y %BATDIR%ThirdFunc\openssl-1.1.1a\libcrypto-1_1.dll %BATDIR%Bin\
goto :eof
