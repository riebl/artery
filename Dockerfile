FROM archlinux:base-devel-20240804.0.251467

SHELL [ "/bin/bash", "-c"]

# OMNeT release tag
ARG OMNETPP_TAG=omnetpp-5.6.2
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

# for SUMO
RUN pacman -S --noconfirm xerces-c fox gdal proj gl2ps jre17-openjdk swig maven eigen

RUN paccache -r -k 0
RUN if [ "${REMOTE}" = "true" ]; then                                                   \
        git clone --recurse-submodules $REPOSITORY --branch $BRANCH --single-branch     \
    ; fi


ENV PROTO_PATH=/cavise/protos
ENV CONTAINER_ROOT_DIR=/cavise/artery
ENV CONTAINER_BUILD_DIR=container_build
ENV BUILD_CONFIG=Debug

ADD messages /cavise/messages
ADD artery /cavise/artery
ADD configure.sh /cavise/configure.sh

WORKDIR /cavise/artery
RUN ./tools/build.py -icb --config ${BUILD_CONFIG} --omnetpp-tag ${OMNETPP_TAG} --dir ${CONTAINER_BUILD_DIR} --local-conan

CMD ["echo", "'run this interactively'"]
