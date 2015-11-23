# GraphViz
A repository for our scientific seminar 'Applied Visualization and Analysis of multivariate Datasets'

## Getting Started

Build:
```sh
$ make
```

Build and run:
```sh
$ make run
```

Build from scratch:
```sh
$ make clean && make
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
