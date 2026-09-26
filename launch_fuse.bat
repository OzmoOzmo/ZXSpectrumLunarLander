@echo off
setlocal
set OUTPUT=%~dp0LLander.tap
if not exist "%OUTPUT%" (
  echo ERROR: LLander.tap not found in %~dp0
  exit /b 1
)
echo Launching Fuse with %OUTPUT%
fuse.exe "%OUTPUT%"
exit /b %ERRORLEVEL%
