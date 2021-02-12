[[ ! -d "./cmake-mingw" ]] && mkdir cmake-mingw
TOP=$PWD
pushd cmake-mingw
	cmake .. -DCMAKE_TOOLCHAIN_FILE=$TOP/toolchains/mingw64.cmake -DEAGLE_DISABLE_QT=ON -DCMAKE_BUILD_TYPE=Release
popd
