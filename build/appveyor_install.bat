:: Init AppVeyor
mkdir release

echo Installing dependiencies...
mkdir dependencies\
cd dependencies\
appveyor DownloadFile https://raw.githubusercontent.com/Infinideastudio/NEWorld-binaries/master/dependencies.exe
dependencies.exe > nul
cd ..
:: Link
echo Creating links...
mklink /D dependencies\include\boost C:\Libraries\boost\boost
