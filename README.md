# Sigma C driver

A PTP comunication library for a Sigma camera. It is only tested on a Sigma FP but should mostly work on other Sigma camera.
A third-party camera control library for the [SIGMA fp series](https://www.sigma-global.com/en/cameras/fp/).

This code is based on the [sigma-ptpy](https://github.com/makanikai/sigma-ptpy) repository.

The SIGMA fp series is a set of full-size mirrorless cameras developed by [SIGMA Corp](https://www.sigma-global.com/en/).
The cameras supports PTP (Picture Transfer Protocol; ISO15740) for access to functionality of them
from computers such as

- getting and setting parameters (shutter speed, aperture, white balance, color mode, etc.),
- taking pictures,
- live view,

and so on. This C library provides a part of the opration on the fp series.

## Getting started

The source code can be compiled by :

```sh
mkdir build
cd build
cmake ..
make
```

After the compilation, connect your SIGMA fp to your computer with "Camera Control" mode and
type the following command. If the camera is detected, a message is printed.

```console
./sigma_camera_test
```

## Related work

SIGMA Corp distributes the [SIGMA Camera Control SDK](https://www.sigma-global.com/en/news/2020/07/02/10916/) for the SIGMA fp series. The official library fully supports the functionality of the cameras, and includes API documents, C/Objective-C headers, and compiled binary files for Windows and Mac.

[libgphoto2](http://www.gphoto.org/) is a open-source framework designed to allow access to digital cameras by external programs. It provides operations on a lot of cameras such as Canon, Nikon, and Sony, but SIGMA fp is less supported at the current version v2.5.27.

[sigma-ptpy](https://github.com/makanikai/sigma-ptpy) is a python camera control library for the Sigma FP. It extends the (ptpy)[https://github.com/Parrot-Developers/sequoia-ptpy] which allow to control other cameras.

## Contribution

Fork and create a PR, please.
