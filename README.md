# GraphViz
A repository for our scientific seminar 'Applied Visualization and Analysis of multivariate Datasets'

## Getting Started

Move to the directory that contains `CMakeLists.txt`:
```sh
$ cd GraphViz
```

Create `build` directory and move there:
```sh
$ mkdir build
$ cd build
```

Run CMake:
```sh
$ cmake -G "Unix Makefiles" ..
```

Build:
```sh
$ make
```

Run:
```sh
$ ./GraphViz
```

## Libraries

OpenGL should be available on your PC already

Building on a Linux System:

FreeGlut:
```sh
$ sudo apt-get install build-essential freeglut3-dev
```

GLEW:
```sh
$ sudo apt-get install libglew-dev
```

Building on a Windows System:

Download Freeglut and GLEW from available sources:

Freeglut : 	http://freeglut.sourceforge.net/index.php#download
GLEW : 		http://glew.sourceforge.net/

while running CMake, specify your pathes to LIB and INCLUDE directories.
