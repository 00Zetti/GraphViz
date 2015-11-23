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

Building on a Linux System:

OpenGl should be available

FreeGlut:
$ sudo apt-get install build-essential freeglut3-dev

GLEW:
$ sudo apt-get install libglew-dev

Building on a Windows System:

Download Freeglut and GLEW from available sources:

Freeglut : 	http://freeglut.sourceforge.net/index.php#download
GLEW : 		http://glew.sourceforge.net/

while running CMake, change your pathes to LIB and INCLUDE directories.
