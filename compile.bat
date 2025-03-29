@echo off
if "%1"=="" (
    echo Error: An argument is required. Please specify "terminal" or "robit".
    exit /b 1
)

if "%1"=="terminal" (
    arduino-cli compile --output-dir ./bin/terminal --fqbn arduino:avr:uno terminal
    exit /b 0
)

if "%1"=="robit" (
    arduino-cli compile --output-dir ./bin/robit --fqbn arduino:avr:uno robit
    exit /b 0
)

echo Error: Invalid argument. Please specify "terminal" or "robit".
exit /b 1
