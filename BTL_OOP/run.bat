@echo off
setlocal
pushd "%~dp0WMS"
if errorlevel 1 exit /b 1
call run.bat
set "RESULT=%ERRORLEVEL%"
popd
exit /b %RESULT%

