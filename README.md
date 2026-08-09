# Cross package manager
Modern source based package manager for GNU/Linux written in C++ 20.

Cross has been heavenly inspired by [kiss](https://github.com/kisslinux/kiss) package manager. It has similar commands, packages syntax to it but written in c++ and with additional features, such as:

- Advanced information about packages
- Auto fetching new package versions (currently supports only GitHub and ftp.gnu.org)
- Safe installing (probability of you breaking something is extremely low)
- Additional arguments to commands, you can enable verbose input, do not install dependencies etc

## Compiling
Cross requires:

- Cmake >= 3.20 (older versions can still work but are not tested)
- C++ 20
- ninja

The building is as simple as

	cmake -S . -B build
	cmake --build build

And for installing

	sudo cmake --install build


## Using cross-pkg
After you compiled and installed cross you can run `cross-pkg h` to get all commands and `cross-pkg h [cmd] `to get information about command arguments

## Making package
Package is made from one <package name>.crs file with a couple of fields:

### -desc-
`Required` Description for package (**must be in one line**)

### -version-
`Required` Package version (**must be in one line**)

### -sources-
`Optional but pretty much required` Package source with url+ or git+ prefix, one source per line

After source you can specify directory to move source info, _directory will be created relevant to cache/pkg/source directory_

Also git+ prefix can have @ symbol with commit has after it to checkout to required commit
`git+https://github.com/het-best/cross-pkg.git@e409b4beaac991fe5c702070862f406e0a7cc3d9 /some/dir/`

### -depends-
`Optional` Package dependencies, one package per line

### -before-build-
`Optional` Script to run before building

### -build-
`Required` Build script, first argument when calling this script is set to package install dir

### -after-install-
`Optional` Script to run after installing

Example package script

    -desc-
        Modern source based package manager for GNU/Linux written in C++ 20.
    -version-
        1.0.0
    -sources-
        url+https://github.com/het-best/cross-pkg
    -depends-
        cmake
        gcc
    -before-build-
        echo "This is before-build script!"
    -build-
	    cmake -S . -B build \
		    -D CMAKE_INSTALL_PREFIX=/usr \
		    -D CMAKE_BUILD_TYPE=Release
        
        	cmake --build build
	        cmake --install build --prefix "$1"
    -after-install-
        echo "This is after-install script!"