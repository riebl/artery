# Distribution tag
ARG TAG=bookworm

FROM debian:${TAG} AS setup

SHELL [ "/bin/bash", "-c"]

RUN apt-get update && apt-get upgrade -y &&     \
    apt-get install -y --no-install-recommends  \
    cmake python3 python3-pip                   \
    libxerces-c-dev libfox-1.6-dev              \
    libgdal-dev libproj-dev g++ qtbase5-dev     \
    libgl2ps-dev openjdk-17-jdk swig            \
    maven libeigen3-dev flex bison git          \
    gcc libsdl2-dev libsm-dev libomp-dev        \
    libopenscenegraph-dev qtbase5-dev           \
    apt-utils sudo libboost-all-dev             \
    libcrypto++-dev xorg libgeographiclib-dev   \
    qtchooser qt5-qmake qtbase5-dev-tools 	    \
    libqt5opengl5-dev ninja-build               \
    && pip install --break-system-packages conan    \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

###############
# Build stage #
###############

FROM setup AS build

# SUMO version (github tag)
ARG SUMO_TAG=v1_21_0
# OMNeT version (github tag)
ARG OMNETPP_TAG=omnetpp-5.6.2

RUN git clone --recurse --depth 1 --branch ${OMNETPP_TAG} https://github.com/omnetpp/omnetpp
WORKDIR /omnetpp
RUN mv configure.user.dist configure.user
RUN source setenv -f && ./configure WITH_OSGEARTH=no && make -j$(nproc --all)

WORKDIR /
RUN git clone --recurse --depth 1 --branch ${SUMO_TAG} https://github.com/eclipse-sumo/sumo
WORKDIR /sumo
RUN cmake -B build . && cmake --build build --parallel $(nproc --all)

######################
# Installation stage #
######################

FROM setup AS final

# User name for this container
ARG USER=devcontainer

COPY --from=build /omnetpp/bin /omnetpp/bin
COPY --from=build /omnetpp/include /omnetpp/include
COPY --from=build /omnetpp/lib /omnetpp/lib
COPY --from=build /omnetpp/images /omnetpp/images
COPY --from=build /omnetpp/Makefile.inc /omnetpp

COPY --from=build /sumo /sumo
RUN cmake --install /sumo/build

RUN useradd -m -G sudo ${USER}
RUN echo "${USER} ALL=(ALL:ALL) NOPASSWD: ALL" | sudo tee /etc/sudoers.d/${USER}

USER ${USER}

ENV PATH=/omnetpp/bin:$PATH
ENV SUMO_HOME=/usr/local/share/sumo
