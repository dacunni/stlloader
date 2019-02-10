# stlloader
Simple loader for STL 3D model files

This is a C++ header-only library. To use it, #include "stlloader.h" wherever you intend to use
the library. You must #define STLLOADER_IMPLEMENTATION before including it in one of your files
for the implementation to be defined for the linker.

Expand the implementation for the linker in one of your files:
```
#define STLLOADER_IMPLEMENTATION
#include "stlloader.h"
```

Otherwise, just include the header and call the functions:
```
#include "stlloader.h"
[...]
    stlloader::Mesh mesh;
    stlloader::parse_file(filename, mesh);
    stlloader::print(mesh);
```
