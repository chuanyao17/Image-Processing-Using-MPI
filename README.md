# Image Processing Using MPI - the final project in 2022 Spring at Hunter College
This repository contains the various parallelizable image processing functions.  
***Authors: Chuanyao Lin***
### Installation

The repository has the following requirements:

 - `MPI`
 - `CMake`
 - `OpenCV`
 
Type the following command line to build the program

```
~$ sh install.sh
```
### Usage

The program is designed to be called via the command line under the /build directory.

The basic command line to run the MPI program is as following
```
~$ mpirun -np <number of tasks> <executable> <input_image> -t(optional)
```

This is an example to run the program with 2 processors(tasks)
```
~$ mpirun -np 2 main women.png
```

The program has the following *optional* command line arguments:

- `-t` *Default* `False`
   - A flag to output the specific image processing executing time. 
