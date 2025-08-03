##################
###### Args ######
##################

# Distribution tag
ARG TAG=base-devel

FROM archlinux:${TAG}

# OMNeT release tag
ARG OMNETPP_TAG=omnetpp-5.6.2

# SUMO tag
ARG SUMO_TAG=v1_23_0

# Final non-root user
ARG USER=cavise

SHELL [ "/bin/bash", "-c" ]

RUN pacman -Syu --noconfirm pacman-contrib clang lld gdb bison flex perl qt5-base               \
    cmake python3 python-pip pyenv wget bison git gcc14 ninja jre17-openjdk swig maven eigen    \
    xorg nvidia-utils mesa sdl2 libsm openmp openscenegraph xerces-c fox gdal proj gl2ps        \
    && paccache -r -k 0

# Temporary: Using conan==2.17 to avoid errors with Pathlib
RUN pip install --no-cache-dir --break-system-packages conan==2.17

# OmnetPP
WORKDIR /
RUN git clone --recurse --depth 1 --branch ${OMNETPP_TAG} https://github.com/omnetpp/omnetpp
RUN cd /omnetpp                                                 \
    && mv configure.user.dist configure.user                    \
    && source setenv -f                                         \
    && ./configure WITH_OSGEARTH=no                             \
    && make -j$(nproc --all) base MODE=release

# SUMO
# reference https://sumo.dlr.de/docs/Installing/Linux_Build.html
WORKDIR /
RUN git clone --recurse --depth 1 --branch ${SUMO_TAG} https://github.com/eclipse-sumo/sumo
RUN cd /sumo                                            \
    && cmake -B build .                                 \
        -DCMAKE_BUILD_CONFIG=Release                    \
        -DENABLE_CS_BINDINGS=OFF                        \
        -DENABLE_JAVA_BINDINGS=OFF                      \
    && cmake --build build --parallel $(nproc --all)    \
    && cmake --install build

RUN cd /usr/local/bin && \
    curl -sSL -O https://raw.githubusercontent.com/llvm/llvm-project/main/clang-tools-extra/clang-tidy/tool/clang-tidy-diff.py && \
    chmod +x clang-tidy-diff.py

RUN useradd -m ${USER}
USER ${USER}

ADD --chown=${USER}:${USER} . /home/cavise/artery

ENV PATH=/omnetpp/bin:$PATH
ENV SUMO_HOME=/usr/local/share/sumo

WORKDIR /home/cavise/artery
RUN ./tools/build.py -icb --config Release --pr:a tools/profiles/container.ini
