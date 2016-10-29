@echo off
pushd %~dp0\..\src\protocol\
python make_fb.py ..\..\dependencies\tools\
popd