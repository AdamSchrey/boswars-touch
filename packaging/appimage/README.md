# How to create a Bos Wars appimage

To build the appimage, you only need to have
[docker](https://docs.docker.com/get-docker/) installed.

From the source root directory, run

    packaging/appimage/make-appimage.sh

The resulting appimage will be output to `packaging/appimage`.

For various options and variables, see the script comments.

# Dockerfile

You don't need to build the Dockerfile. An image from a docker registry will
be pulled when you run the script for the first time. If you want to use a
different image, you can set it in the script. For instructions on building a
docker image from a Dockerfile, see the [Docker
documentation](https://docs.docker.com/engine/reference/commandline/build/).

# AppImage testing

For various options to test the appimage, you can pass arguments to it. See
the [AppImage
docs](https://github.com/AppImage/AppImageKit#command-line-arguments) for
details.
