FROM archlinux:base-devel-20240804.0.251467

SHELL [ "/bin/bash", "-c"]

# OMNeT release tag
ARG OMNETPP_TAG=omnetpp-5.6.2
# SUMO tag (UNUSED FOR NOW)
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

# for SUMO
RUN pacman -S --noconfirm xerces-c fox gdal proj gl2ps jre17-openjdk swig maven eigen

RUN paccache -r -k 0
RUN if [ "${REMOTE}" = "true" ]; then                                                   \
        git clone --recurse-submodules $REPOSITORY --branch $BRANCH --single-branch     \
    ; fi

# Взято из configure.sh, стоит обновлять при изменении общей конфигурации 
ENV PROTO_PATH=/cavise/protos
ENV CONTAINER_ARTERY_DIR=/cavise/artery

ENV CONTAINER_BUILD_DIR=container_build
ENV BUILD_CONFIG=Release
ENV CONAN_PROFILE=tools/profiles/container.ini

COPY artery/ ${CONTAINER_ARTERY_DIR}
COPY protos /cavise/protos
COPY cavise /cavise/cavise

WORKDIR ${CONTAINER_ARTERY_DIR}
RUN ./tools/build.py -icb --dir ${CONTAINER_BUILD_DIR} --config ${BUILD_CONFIG} --local-conan --profile ${CONAN_PROFILE}

CMD ["echo", "'run this interactively'"]
