@echo off
setlocal enabledelayedexpansion

if "%1"=="" (
    echo Error: An argument is required. Please specify "terminal" or "robit".
    exit /b 1
)

if "%2"=="" (
    echo Error: A 2nd argument is required. Please specify the COM port.
    exit /b 1
)

if "%1"=="terminal" (
    arduino-cli upload --input-dir bin/terminal -p %2 --fqbn arduino:avr:uno terminal
    exit /b 0
)

if "%1"=="robit" (
    arduino-cli upload --input-dir bin/robit -p %2 --fqbn arduino:avr:uno robit
    exit /b 0
)

echo Error: Invalid argument. Please specify "terminal" or "robit".
exit /b 1