@echo off
if "%~1" == "/C" goto clean

:gen
rem Generate solution
call .\Arbour\ThirdParty\Premake\premake5.exe vs2022 --file="premake5.lua"
goto :eof

:clean
rem Clear intermediate Visual Studio files
del /S .vs
del /S *.sln
del /S *.vcxproj
goto gen