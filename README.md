## SSXTools

SSXTools is my personal set of libraries for working with the SSX games, coupled with a Qt-based graphical user interface

The project's main goals are to work well while being very small and fast at the same time.

Tested games:

- SSX (2000)
- SSX Tricky (2001)
- SSX 3 (2003)


## Building SSXTools

You need to clone the repository with the --recursive flag.

For building the GUI, you need to install Qt 5.

```
mkdir build
cd build
cmake .. <your options here, install to a prefix on Windows>
make -j3
(for GUI on Windows/selfcontained build) make install
```

On Windows you can also use Visual Studio's CMake tools.

On Linux building for windows can be donw with the build-mingw.sh script.

## Running

Run `./ssxtools` on Linux, `ssxtools.exe` on Windows

## Using the SSXTools libraries in your own projects

TODO now that this will be bigger

### Checklist

- [x] SSH header loading
	- [x] SSH TOC loading
		- [x] 256color images load/conversion
		- [x] Get alpha working
		- [ ] 16bpp color images load/conversion (Is this even a possible format?)
		- [x] 32bpp images load/conversion
