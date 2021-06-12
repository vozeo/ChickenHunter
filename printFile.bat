@echo off 
setlocal enabledelayedexpansion
for /r %%a in (*.cpp) do (
  set "x=%%a"
  echo !x:%cd%\=!
)