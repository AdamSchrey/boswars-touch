#!/usr/bin/env -S bash --posix

if [ -z "$UID" ]; then
  echo "Could not detect UID."
  exit 1
fi

if [ ! -e "engine/game/game.cpp" ];then
  echo "You need to be in the rmw source root directory when you run this script."
  exit 1
fi

# This is used by the secondary workflow script
export WORKSPACE="/boswars-src-root"

# default version. linuxdeploy uses this to set the filename
# of the output AppImage
# To change it at script run-time, precede the scriptname with 'VERSION=<version>'
export VERSION=${VERSION:-"git-$(git rev-parse --short HEAD)"}

# change this if you want to use a different image from the registry
# or a local one you've created
DOCKER_IMAGE="andy5995/boswars-build-env:focal"

echo "Version is set to '$VERSION'"
echo "use 'VERSION=<version> $0' to change it."
echo "Waiting 10 seconds to start, hit CTRL-C now to cancel..."

read -t 10

set -ev

docker run -it --rm \
  -e VERSION=$VERSION  \
  -e ARCH=x86_64 \
  -e WORKSPACE \
  -e HOSTUID=$UID \
  -v $PWD:$WORKSPACE \
  "$DOCKER_IMAGE" \
    /bin/bash -c 'usermod -u $HOSTUID bosbuilder \
    && su bosbuilder --command "$WORKSPACE/packaging/appimage/workflow.sh"'
