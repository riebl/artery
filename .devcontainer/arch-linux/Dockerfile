# Distribution tag
ARG TAG=base-devel

FROM archlinux:${TAG} AS setup

SHELL [ "/bin/bash", "-c"]

RUN pacman -Syu --noconfirm     \
    --disable-download-timeout  \
    cmake python3 python-pip    \
    xerces-c fox gdal proj      \
    gl2ps jre17-openjdk swig    \
    maven eigen flex bison git  \
    gcc sdl2 libsm openmp       \
    openscenegraph qt5-base     \
    pacman-contrib sudo         \
    boost crypto++ xorg         \
    ttf-ubuntu-font-family      \
    ninja                       \
    && pip install --break-system-packages conan \
    && paccache -r -k 0

###############
# Build stage #
###############

FROM setup AS build

# SUMO version (github tag)
ARG SUMO_TAG=v1_21_0
# OMNeT version (github tag)
ARG OMNETPP_TAG=omnetpp-5.6.2
# GeographicLib version (github tag)
ARG GEOGRAPHICLIB_TAG=v2.5

RUN git clone --recurse --depth 1 --branch ${OMNETPP_TAG} https://github.com/omnetpp/omnetpp
WORKDIR /omnetpp
RUN mv configure.user.dist configure.user
RUN source setenv -f && ./configure WITH_OSGEARTH=no && make -j$(nproc --all)

WORKDIR /
RUN git clone --recurse --depth 1 --branch ${SUMO_TAG} https://github.com/eclipse-sumo/sumo
WORKDIR /sumo
RUN cmake -B build . && cmake --build build --parallel $(nproc --all)

WORKDIR /
RUN git clone --recurse --depth 1 --branch ${GEOGRAPHICLIB_TAG} https://github.com/geographiclib/geographiclib
WORKDIR /geographiclib
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

COPY --from=build /geographiclib /geographiclib
RUN cmake --install /geographiclib/build

RUN groupadd sudo && useradd -m -G sudo ${USER}
RUN echo "${USER} ALL=(ALL:ALL) NOPASSWD: ALL" | sudo tee /etc/sudoers.d/${USER}

USER ${USER}

ENV PATH=/omnetpp/bin:$PATH
ENV SUMO_HOME=/usr/local/share/sumo
