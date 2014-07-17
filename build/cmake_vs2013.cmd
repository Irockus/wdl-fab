set ROOTDIR=%~dp0/win
if not exist %ROOTDIR% mkdir %ROOTDIR%
pushd .

cd %ROOTDIR%

if exist CmakeCache.txt del CMakeCache.txt

cmake ../.. -G "Visual Studio 12"
pause

popd
