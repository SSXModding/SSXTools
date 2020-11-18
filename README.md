## EAGLe

EAGLe is a tool for working with EAGL/Gimex formats, and (potentionally...) working with some BIG archives.


Currently, that means converting Gimex .SSH shape files (seen on EA Canada Playstation 2 titles) to individual .PNG files.


It's main goals are to work well while being very small and fast at the same time.

Tested games:

- SSX (2000)
- SSX Tricky (2001)
- SSX 3 (2003)

## Credits

- eldrinn for making SSX3Converter

## Building EAGLe

You need to clone the repository with the --recursive flag.

For building the EAGLe UI, you need to install Qt 5.

```
mkdir build
cd build
cmake .. <your options here, install to a prefix on Windows>
make -j3
(for UI on Windows/selfcontained build) make install
```

On Windows you can also use Visual Studio's CMake tools.

## Running

### CLI
Simply do `path/to/eagle-cli ssh_path dump_path/` and EAGLe will do its magic!

### GUI
just run the exe lol

## Using Core in your own projects

NOTE that the Core library API will change frequently until
a stable version...

Include SHPSCore like so...
```cmake
set(EAGLE_SHPSCORE_ONLY ON)
add_subdirectory(path/to/vendor/eagle)
```

and link with it like...
```cmake
target_link_libraries(project eagle-core)
```

### Checklist

- [x] SSH header loading
	- [x] SSH TOC loading
		- [x] 256color images load/conversion
		- [x] Get alpha working
		- [ ] 16bpp color images load/conversion (Is this even a possible format?)
		- [x] 32bpp images load/conversion