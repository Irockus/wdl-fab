if exist CmakeCache.txt del CMakeCache.txt
cmake -DCMAKE_PREFIX_PATH="%VORBIS_DIR%" -G "Visual Studio 12"
