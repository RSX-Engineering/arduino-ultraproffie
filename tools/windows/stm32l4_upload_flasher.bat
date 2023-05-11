echo off
set count=1
:wait
    "%~dp0\stmcube_programmer\bin\STM32_Programmer_CLI.exe" -c port=%1 br=115200 p=even db=8 sb=1 fc=off | findstr "Activating device: OK" >NUL 2>&1
    if %errorlevel% == 0 ( 
	"%~dp0\stmcube_programmer\bin\STM32_Programmer_CLI.exe" -c port=%1 br=115200 p=even db=8 sb=1 fc=off -w %2 0x08000000
    ) else (
        if %count% gtr 10 goto break 
        echo %count%
        set /A count+=1
        goto :wait
    )
)

:break