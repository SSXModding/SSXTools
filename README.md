## EAGLe

EAGLe is a tool that converts EAGL .SSH textures (commonly seen on EA Playstation 2 titles) to .PNG files.

I've only tested it with files from SSX (2000) so other games may not work currently.

## Compiling

```
mkdir build
cd build
cmake .. <your options here>
make -j3
```

On Windows you can also use Visual Studio's CMake tools.

## Running

Simply do `path/to/eagle ssh_path` and EAGLe will do its magic! (TODO this will change)

### Checklist

- [x] SSH header loading
	- [x] SSH TOC loading
		- [x] 256color images load/conversion
			- [x] Get alpha working (this is probably common)
		- [ ] 16bpp color images load/conversion (Is this even a possible format?)
		- [ ] 32bpp images load/conversion


## Thanks/Credits

Thanks to [Sean Barrett](https://github.com/nothings/stb) for his wonderful stb_image_write library.