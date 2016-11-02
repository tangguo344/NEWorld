@echo off
pushd %~dp0
call .\copy_dlls_64.bat
call .\build_fbs.bat
popd