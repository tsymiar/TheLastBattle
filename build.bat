@echo off

"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" ./AllServer.sln /t:build /p:configuration="Debug" /m

echo ----------------------------------------------------
echo Copy Files to target directory
echo ----------------------------------------------------

if not exist Bin md Bin
pause

copy /y %~dp0Debug\*.exe %~dp0Bin\
copy /y %~dp0Lib\Win32\Debug\*.dll %~dp0Bin\
copy /y %~dp0ThirdFunc\libmysql\libmysql.dll %~dp0Bin\
copy /y %~dp0ThirdFunc\openssl-1.1.1a\libcrypto-1_1.dll %~dp0Bin\
pause