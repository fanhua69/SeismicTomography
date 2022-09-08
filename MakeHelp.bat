@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by NEWMOUNT.HPJ. >"hlp\NewMount.hm"
echo. >>"hlp\NewMount.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\NewMount.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\NewMount.hm"
echo. >>"hlp\NewMount.hm"
echo // Prompts (IDP_*) >>"hlp\NewMount.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\NewMount.hm"
echo. >>"hlp\NewMount.hm"
echo // Resources (IDR_*) >>"hlp\NewMount.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\NewMount.hm"
echo. >>"hlp\NewMount.hm"
echo // Dialogs (IDD_*) >>"hlp\NewMount.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\NewMount.hm"
echo. >>"hlp\NewMount.hm"
echo // Frame Controls (IDW_*) >>"hlp\NewMount.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\NewMount.hm"
REM -- Make help for Project NEWMOUNT


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\NewMount.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\NewMount.hlp" goto :Error
if not exist "hlp\NewMount.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\NewMount.hlp" Debug
if exist Debug\nul copy "hlp\NewMount.cnt" Debug
if exist Release\nul copy "hlp\NewMount.hlp" Release
if exist Release\nul copy "hlp\NewMount.cnt" Release
echo.
goto :done

:Error
echo hlp\NewMount.hpj(1) : error: Problem encountered creating help file

:done
echo.
