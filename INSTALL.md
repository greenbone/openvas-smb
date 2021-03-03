INSTALLATION INSTRUCTIONS FOR OPENVAS-SMB
=========================================

Please note: The reference system used by most of the developers is Debian
GNU/Linux 'Buster' 10. The build might fail on any other system. Also, it is
necessary to install dependent development packages.

Prerequisites for openvas-smb
-----------------------------

* gcc-mingw32 | gcc-mingw-w64
* libgnutls-dev >= 3.2.15
* perl-base
* heimdal-dev | heimdal-multidev >= 1.6.0
* libpopt-dev
* libglib2.0-dev
* libunistring-dev

Install prerequisites on Debian GNU/Linux 'Stretch' 9:

    apt-get install gcc cmake pkg-config gcc-mingw-w64 libgnutls28-dev \
                    perl-base heimdal-dev libpopt-dev libglib2.0-dev \
                    libunistring-dev

Compiling openvas-smb
---------------------

Create a build directory and change into it with

    mkdir build
    cd build

Configure the build with

    cmake -DCMAKE_INSTALL_PREFIX=/path/to/your/installation ..

or (if you want to use the default installation path /usr/local)

    cmake ..

This only needs to be done once.

Thereafter, the following commands are useful.

    make                # build the libraries
    make doc            # build the documentation
    make doc-full       # build more developer-oriented documentation
    make install        # install the build
    make rebuild_cache  # rebuild the cmake cache

Please note that you may have to execute `make install` as root, especially if
you have specified a prefix for which your user does not have full permissions.

To clean up the build environment, simply remove the contents of the `build`
directory you created above.
