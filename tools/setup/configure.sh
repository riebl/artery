#!/bin/bash

set -e

ROOT_DIR=$PWD

if [ ! -e .gitignore ]; then 
    echo "$ROOT_DIR does not look like root artery directory, aborting..."
    exit 2
fi

while [[ $# -gt 0 ]]; do
  case $1 in
    -p|--profile)
      PROFILE="$2"
      shift && shift
      ;;
    --omnetpp-tag)
      OMNETPP_TAG="$2"
      shift && shift
      ;;
    --build-dir)
      BUILD_DIR="$2"
      shift && shift
      ;;
    --remove-build)
      REMOVE_BUILD=true
      shift
      ;;
    -*|--*)
      echo "unknown arg $1"
      exit 1
      ;;
    *)
  esac
done

if [ -z $OMNETPP_TAG ]; then
    OMNETPP_TAG=omnetpp-5.6.2
fi
echo "using Omnet++ revision: $OMNETPP_TAG" 

if [ -z $PROFILE ]; then 
    PROFILE=default.ini
fi
echo "using conan profile: $PROFILE" 

if [ ! -e $ROOT_DIR/tools/setup/build.py ]; then
    echo "missing build utility, aborting..."
    exit 2
fi

# handle build dir creation
if [ -z $BUILD_DIR ]; then
    BUILD_DIR=$ROOT_DIR/build/Debug
fi

if [ "$REMOVE_BUILD" = "true" ] && [ -d $BUILD_DIR ]; then
    echo "removing build directorry..."
    rm -r $BUILD_DIR
elif [ -d $BUILD_DIR ]; then
    echo "build directory exists, use --remove-build if you want to start all over."
fi

mkdir -p $BUILD_DIR

if [ -d $BUILD_DIR/$OMNETPP_TAG ]; then
    echo "Omnet++ directory exists, skipping setup..."
else
    cd $BUILD_DIR \
        && wget -c https://github.com/omnetpp/omnetpp/releases/download/$OMNETPP_TAG/$OMNETPP_TAG-src-linux.tgz -O source.tgz \
        && tar xzf source.tgz \
        && rm source.tgz
    # see https://askubuntu.com/questions/1035220/error-while-installing-omnet-on-ubuntu-16-04-cannot-find-osgearth
    cd $BUILD_DIR/$OMNETPP_TAG \
        && sed -i "s/WITH_OSGEARTH=yes/WITH_OSGEARTH=no/g" configure.user \
        && source setenv -f && ./configure && make -j$(nproc --all)
fi

if [ -d $BUILD_DIR/conan2 ]; then 
    echo "Conan cache exists, skipping setup..."
else 
    mkdir -p $BUILD_DIR/conan2/profiles \
        && cp --remove-destination $ROOT_DIR/tools/setup/$PROFILE $BUILD_DIR/conan2/profiles \
        && cd $BUILD_DIR/conan2/profiles \
        && mv $PROFILE container \
        && cd $ROOT_DIR
fi

SUMO_HOME="/usr/local/share/sumo"
if [ -d $SUMO_HOME ]; then
    echo "SUMO installed already, skipping installation..."
else
	# reference https://sumo.dlr.de/docs/Installing/Linux_Build.html
	cd $BUILD_DIR && git clone --recurse --depth 1 https://github.com/eclipse-sumo/sumo
	cd $BUILD_DIR/sumo && cmake -B build . && cmake --build build -j$(nproc --all)
	# requires root priviligies
	cd $BUILD_DIR/sumo && sudo cmake --install build
fi

export CONAN_HOME=$ROOT_DIR/build/Debug/conan2
export CONAN_ARGS="-pr:a=container"
export PATH=$ROOT_DIR/build/Debug/$OMNETPP_TAG/bin:$PATH
