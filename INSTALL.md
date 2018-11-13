
# Installation #

1. 	Make a directory where the whole project will be, e.g.:

		mkdir PROPOSAL

2.	Create a build and src directory, e.g.:

		mkdir PROPOSAL/src PROPOSAL/build

3. 	Extract the sources from the
	[hompage](http://app.tu-dortmund.de/cms/de/Projekte/PROPOSAL/) or
	gitlab to the folder, e.g.:

		unzip PROPOSAL.zip PROPOSAL/

	or

		git clone ... PROPOSAL/src

4.	Move to the build directory and generate the Makefile with cmake:

		cd PROPOSAL/build
		cmake ../src

	If you don't want to compile the pybindings, call cmake with

		cmake ../src -DADD_PYTHON=OFF

	To specify an installation prefix other than `/usr/local` use

		cmake ../src -DCMAKE_INSTALL_PREFIX=/custom/prefix

	The prefix is used to install PROPOSAL on your system (See
	item 6).<br>
	To show further installation options use `ccmake ../src` and/or
	visit the [documentation](https://cmake.org/documentation/).

	#### **Note** ####

	The option `CMAKE_INSTALL_PREFIX` adds the given path also to the
	include directories. So if you have installed PROPOSAL with
	`CMAKE_INSTALL_PREFIX` and are modifying the header files, you will make
	sure to uninstall PROPOSAL before the next build otherwise your local
	changes won't be used.

6.  Compile the project:

		make

	or

		make -j#

	with `#` being the number of processors you can choose to compile
	the project on multiple processors simultaneously.

7.	Install the library on your system

		make install

	or for e.g.

		make install DESTDIR=$HOME

	The latter command will install PROPOSAL in `$HOME/<prefix>`, where
	the `prefix` was defined by `CMAKE_INSTALL_PREFIX` which defaults
	to `usr/local`.


# Uninstalling #

It is also possible to uninstall PROPOSAL with

	make uninstall

This will remove all files listed in `install_mainfest.txt` which should
have been created in your build directory after the installation.
