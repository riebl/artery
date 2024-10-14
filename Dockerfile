FROM archlinux:base-devel-20240804.0.251467

SHELL [ "/bin/bash", "-c"]

# add sumo to deps
ARG SUMO=true
# OMNeT release tag
ARG OMNET_TAG=omnet-5.6.2
# SUMO tag
ARG SUMO_TAG=v1_20_0
# use remote (to be used in pipelines)
ARG REMOTE=false
# repo for remote
ARG REPOSITORY='https://github.com/CAVISE/artery.git'
# branch to build from
ARG BRANCH='master'

WORKDIR /cavise

RUN pacman -Syu --noconfirm pacman-contrib
RUN pacman -S --noconfirm cmake python3 python-pip pyenv wget bison git gcc
RUN pacman -S --noconfirm xorg nvidia-utils mesa sdl2 libsm openmp qt5-base openscenegraph

RUN pip install --break-system-packages conan

RUN ls -s /bin/python3 /usr/bin/python

########################################################
# Install SUMO
########################################################
RUN if [ "${SUMO}" = "true" ]; then                                                         \
        # reference https://sumo.dlr.de/docs/Installing/Linux_Build.html
        pacman -S --noconfirm xerces-c fox gdal proj gl2ps jre17-openjdk                    \
            swig maven eigen &&                                                             \
        git clone --recurse --depth 1 https://github.com/eclipse-sumo/sumo &&               \
        cd sumo && cmake -B build . && cmake --build build -j$(nproc --all) &&              \
        cmake --install build                                                               \ 
    ; else                                                                                  \
        echo "Installation without SUMO"                                                    \
    ; fi
ENV SUMO_HOME="/usr/local/share/sumo"

RUN paccache -r -k 0
RUN if [ "${REMOTE}" = "true" ]; then                                                   \
        git clone --recurse-submodules $REPOSITORY --branch $BRANCH --single-branch     \
    ; fi

# BUILD STEP
ENV PROTO_PATH=/cavise/protos

ENV CONTAINER_ROOT_DIR=/cavise/artery
ENV CONTAINER_BUILD_DIR=container_build
ENV BUILD_CONFIG=Debug

ENV CONAN_HOME=${CONTAINER_ROOT_DIR}/${CONTAINER_BUILD_DIR}/${BUILD_CONFIG}/conan2
ENV CONAN_ARGS="-pr:a=container"
ENV PATH=${CONTAINER_ROOT_DIR}/${CONTAINER_BUILD_DIR}/${BUILD_CONFIG}/omnetpp-5.6.2/bin:$PATH

ADD . /cavise

# should do something like this:
# TODO: Make conmpatible with Release
WORKDIR /cavise/artery
RUN ./tools/setup/configure.sh --build-dir ${CONTAINER_ROOT_DIR}/${CONTAINER_BUILD_DIR}/${BUILD_CONFIG}
RUN ./tools/setup/build.py -c -b --config ${BUILD_CONFIG} --dir ${CONTAINER_BUILD_DIR}

CMD ["echo", "'run this interactively'"]