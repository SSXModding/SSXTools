./gen-mingw.sh

pushd cmake-mingw
	make -j$(($(nproc)+1))
popd

[[ ! -d "mingw-bin" ]] && mkdir mingw-bin

cp cmake-mingw/src/eaglecli/eagle-cli.exe mingw-bin/

# this is for john debian
cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll mingw-bin
cp /usr/lib/gcc/x86_64-w64-mingw32/9.3-posix/libgcc_s_seh-1.dll mingw-bin/
cp /usr/lib/gcc/x86_64-w64-mingw32/9.3-posix/libstdc++-6.dll mingw-bin/

# do a strip
x86_64-w64-mingw32-strip mingw-bin/eagle-cli.exe
