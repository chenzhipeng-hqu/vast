@echo off

::current drive(der Laufwerksname)
::echo current pan : %~d0
::current path
::echo current path : %cd%
::current bat file path
::echo the bat's path : %~dp0
::current bat file short path
::echo the bat's path : %~dp0


set SRC_PATH=.

set BIN_PATH=..\\bin

set PRJ_NAME=%1
::echo %PRJ_NAME%

set FLIE_PATH=%2
::echo %FLIE_PATH%

arm-none-eabi-objcopy.exe "%SRC_PATH%\\%PRJ_NAME%.elf" -O binary "%BIN_PATH%\\%PRJ_NAME%.bin"

for /f "delims=" %%a in ('findstr "VERSION_MAJOR" %FLIE_PATH%') do set major=%%a
set major=%major:/*VERSION_MAJOR*/=%
set major=%major:,=%
set major=%major:	=%
set major=%major: =%
set /a major=%major%+0

for /f "delims=" %%a in ('findstr "VERSION_MINOR" %FLIE_PATH%') do set minor=%%a
set minor=%minor:/*VERSION_MINOR*/=%
set minor=%minor:,=%
set minor=%minor:	=%
set minor=%minor: =%
set /a minor=%minor%+0

for /f "delims=" %%a in ('findstr "VERSION_REVISION" %FLIE_PATH%') do set revision=%%a
set revision=%revision:/*VERSION_REVISION*/=%
set revision=%revision:,=%
set revision=%revision:	=%
set revision=%revision: =%
set /a revision=%revision%+0

set VERSION=%major%.%minor%.%revision%
echo firmware version: %VERSION%

for /f "tokens=2 delims==" %%a in ('wmic path win32_operatingsystem get LocalDateTime /value') do (
  set t=%%a
)
set Today=%t:~0,4%%t:~4,2%%t:~6,2%

copy     %BIN_PATH%\\%PRJ_NAME%.bin 		%BIN_PATH%\\%PRJ_NAME%_%Today%_V%VERSION%.bin

del %BIN_PATH%\\%PRJ_NAME%.bin