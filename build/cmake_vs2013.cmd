set ROOTDIR=%~dp0
if not exist %ROOTDIR%win md %ROOTDIR%win
pushd .

cd %ROOTDIR%win

if exist CmakeCache.txt del CMakeCache.txt

cmake ../.. -G "Visual Studio 12"
pause

popd
