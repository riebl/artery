# Distribution tag
ARG TAG=base-devel

FROM archlinux:${TAG}

# User name for this container
ARG USER=devcontainer
# OMNeT version (github tag)
ARG OMNETPP_TAG=omnetpp-5.6.2
# SUMO version (github tag)
ARG SUMO_TAG=v1_21_0

SHELL [ "/bin/bash", "-c"]

RUN pacman -Syu --noconfirm     \
    cmake python3 python-pip    \
    xerces-c fox gdal proj      \
    gl2ps jre17-openjdk swig    \
    maven eigen flex bison git  \
    gcc sdl2 libsm openmp       \
    openscenegraph qt5-base     \
    pacman-contrib sudo

RUN groupadd sudo && useradd -m -G sudo ${USER}
USER ${USER}

###############
# Build stage #
###############

WORKDIR /home/${USER}/setup
RUN git clone --recurse --depth 1 --branch ${OMNETPP_TAG} https://github.com/omnetpp/omnetpp
WORKDIR /home/${USER}/setup/omnetpp
RUN mv configure.user.dist configure.user
RUN source setenv -f && ./configure WITH_OSGEARTH=no && make -j$(nproc --all)

WORKDIR /home/${USER}/setup
RUN git clone --recurse --depth 1 --branch ${SUMO_TAG} https://github.com/eclipse-sumo/sumo
WORKDIR /home/${USER}/setup/sumo
RUN cmake -B build . && cmake --build build --parallel $(nproc --all)

######################
# Installation stage #
######################

USER root

WORKDIR /
RUN mkdir -p /omnetpp
RUN cp -r /home/${USER}/setup/omnetpp/bin /omnetpp/bin
RUN cp -r /home/${USER}/setup/omnetpp/include /omnetpp/include
RUN cp -r /home/${USER}/setup/omnetpp/lib /omnetpp/lib
RUN cmake --install /home/${USER}/setup/sumo/build

###########
# Cleanup #
###########

RUN paccache -r -k 0
RUN rm -r /home/${USER}/setup

USER ${USER}

ENV PATH=/omnetpp/bin:$PATH
ENV SUMO_HOME=/sumo/share/sumo

CMD ["sleep", "infinity"]