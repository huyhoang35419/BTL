@echo off
setlocal
pushd "%~dp0WMS"
if errorlevel 1 exit /b 1
call build.bat
set "RESULT=%ERRORLEVEL%"
popd
exit /b %RESULT%

