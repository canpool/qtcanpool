@echo off
ECHO %1
set PWD=%~dp0

echo Setting up a Qtitan environment...
echo -- QTITANDIR set to %PWD%
set QTITANDIR=%PWD%
CALL :SetLibPath

if not "%VS71COMNTOOLS%" == "" CALL :SetLibPath _msvc2003
if not "%VS80COMNTOOLS%" == "" CALL :SetLibPath _msvc2005
if not "%VS90COMNTOOLS%" == "" CALL :SetLibPath _msvc2008
goto :Return

:SetLibPath
SET PrefixLib=
echo -- Adding %PWD%lib%PrefixLib% to PATH
set PATH=%PATH%;%PWD%lib%PrefixLib%
:Return