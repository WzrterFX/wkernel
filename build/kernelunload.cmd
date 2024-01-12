@echo off

set "wkerneldir=%~dp0"

>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if %errorlevel% neq 0 exit

"%wkerneldir%kernelloader.exe" "%wkerneldir%wkernel.sys"