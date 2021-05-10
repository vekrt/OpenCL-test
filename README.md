[![C/C++ CI](https://github.com/vekrt/OpenCL-test/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/vekrt/OpenCL-test/actions/workflows/c-cpp.yml)
# OpenCL-test

Monte-Carlo simulation of Bachelier/Normal process on the GPU using OpenCL. The simulation computes the value of vanilla call and put options.

## Quick start

On Ubuntu:

````console
$ sudo apt-get install ocl-icd-opencl-dev opencl-headers
$ make 
$ ./main
````

The size of the simulation is hardcoded therefore the simulation parameters must be adapted to your hardware.

## Result

In the Figure below, the black lines represent the analytical prices of vanilla options under the Bachelier/Normal model. The circles with errorbars show the result of a Monte-Carlo simulation with 2<sup>20</sup> trajectories, 2<sup>15</sup> timesteps, S<sub>0</sub>=100, σ<sub>0</sub>=0.1 and T = 0.5. 
![result](https://user-images.githubusercontent.com/49154901/117677951-37e3c800-b1af-11eb-90f8-57e1c1b2f6ad.png)
