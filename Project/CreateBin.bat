@echo off
@echo Y | del output\* /s
@for /f "tokens=1-3 delims=/ " %%a in ('echo %date%') do set dateFile=%%a%%b%%c
@for /f "tokens=1-3 delims=:." %%a in ('echo %time%') do set timeFile=%%a%%b%%c
@echo F |xcopy "bin\%1" "output\%1%dateFile%%timeFile%.bin"
