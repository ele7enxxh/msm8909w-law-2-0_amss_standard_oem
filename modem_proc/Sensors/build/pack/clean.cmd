@echo off

:LOOP
for /D %%d in (HY11_1 .) do if EXIST %%~nd (
  echo Cleaning %%~nd %1
  cd %~dp0%%~nd\dsps_proc\core\bsp\build
  call build.cmd sensorsimg BUILD_ID=%1 --clean
  cd %~dp0
)
shift
if not %1. == . goto LOOP

for /D %%d in (HY11_1 .) do if EXIST %%~nd (
  echo Removing .pyc files from %%~nd
  cd %~dp0%%~nd\dsps_proc
  del /S *.pyc
  cd %~dp0
)


