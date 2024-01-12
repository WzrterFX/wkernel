@echo off

set "wkerneldir=%~dp0"

>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if %errorlevel% neq 0 exit

sc.exe delete wkernel
sc.exe create wkernel binpath="%wkerneldir%wkernel.sys" type=kernel start=auto
"%wkerneldir%kernelloader.exe" "%wkerneldir%license.sys" "%wkerneldir%wkernel.sys"