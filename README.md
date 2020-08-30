## EAGLe

EAGLe is a tool for working with EAGL formats, and working with some BIG archives.

Currently, that means converting EAGL .SSH texture banks (seen on EA Canada Playstation 2 titles) to individual .PNG files.

Tested games:

- SSX (2000)
- SSX Tricky (2001)
- SSX 3 (2003)
  - NOTE: 8bpp images are broken.

## Building EAGLe

You need to clone the repository with the --recursive flag.

```
mkdir build
cd build
cmake .. <your options here>
make -j3
```

On Windows you can also use Visual Studio's CMake tools.

## Running

Simply do `path/to/eagle ssh_path` and EAGLe will do its magic! (TODO this will change)

### Using SHPSCore in your own projects

Include SHPSCore like so...
```cmake
set(EAGLE_SHPSCORE_ONLY ON)
add_subdirectory(path/to/vendor/eagle)
```

and link with it like...
```cmake
target_link_libraries(project shpscore)
```

### Checklist

- [x] SSH header loading
	- [x] SSH TOC loading
		- [x] 256color images load/conversion
		- [x] Get alpha working
		- [ ] 16bpp color images load/conversion (Is this even a possible format?)
		- [x] 32bpp images load/conversion

- [ ] Work on a UI for EAGLe
	- [ ] Offer selection of images to convert along with a preview of a selected image


## Thanks/Credits

Thanks to [Sean Barrett](https://github.com/nothings/stb) for his wonderful stb_image_write library.