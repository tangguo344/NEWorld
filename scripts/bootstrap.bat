@echo off
pushd %~dp0
call .\copy_dlls.bat
call .\build_fbs.bat
popd