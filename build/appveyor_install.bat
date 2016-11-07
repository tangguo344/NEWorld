:: Init AppVeyor
echo Installing dependiencies...
mkdir dependencies\
cd dependencies\
appveyor DownloadFile https://raw.githubusercontent.com/Infinideastudio/NEWorld-binaries/master/dependencies.exe
dependencies.exe > nul
cd ..