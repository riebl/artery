FROM archlinux:base-devel

SHELL [ "/bin/bash", "-c"]

# OMNeT release tag
ARG OMNETPP_TAG=omnetpp-5.6.2
# SUMO tag
ARG SUMO_TAG=v1_22_0

WORKDIR /cavise

RUN pacman -Syu --noconfirm pacman-contrib &&\
    pacman -S --noconfirm cmake python3 python-pip pyenv wget bison git gcc ninja &&\
    pacman -S --noconfirm xorg nvidia-utils mesa sdl2 libsm openmp openscenegraph &&\
    pacman -Sc --noconfirm && \
    rm -rf /var/cache/pacman/pkg/* /tmp/*

RUN pip install --no-cache-dir --break-system-packages conan

# OmnetPP
ENV OMNETPP_ROOT=/cavise/${OMNETPP_TAG}/bin \
    PATH=/cavise/${OMNETPP_TAG}/bin:${PATH}
RUN pacman -S --noconfirm clang lld gdb bison flex perl qt5-base && \
    rm -rf /var/cache/pacman/pkg/* /tmp/*
RUN wget -c https://github.com/omnetpp/omnetpp/releases/download/${OMNETPP_TAG}/${OMNETPP_TAG}-src-linux.tgz -O source.tgz &&\
    tar -xvzf source.tgz && rm source.tgz
RUN cd ${OMNETPP_TAG} && sed -i 's/^WITH_OSGEARTH=yes$/WITH_OSGEARTH=no/' configure.user
RUN cd ${OMNETPP_TAG} && source setenv -f && ./configure
RUN cd ${OMNETPP_TAG} && make -j$(nproc --all)

# SUMO
# reference https://sumo.dlr.de/docs/Installing/Linux_Build.html
ENV SUMO_HOME="/usr/local/share/sumo"
RUN pacman -S --noconfirm xerces-c fox gdal proj gl2ps jre17-openjdk swig maven eigen && \
        git clone --recurse --depth 1 --branch ${SUMO_TAG} https://github.com/eclipse-sumo/sumo && \
        cd sumo && cmake -B build . && cmake --build build -j$(nproc --all) && \
        cmake --install build && \
        rm -rf /cavise/sumo /var/cache/pacman/pkg/* /tmp/*

RUN paccache -r -k 0

# Artery build params
ARG ARTERY_DIR=/cavise/artery
ARG BUILD_CONFIG=Release
ARG CONAN_PROFILE=container.ini

COPY artery/ ${ARTERY_DIR}
COPY cavise/ /cavise/cavise

WORKDIR ${ARTERY_DIR} 
RUN ./tools/build.py -cbi --config ${BUILD_CONFIG} --profile ${ARTERY_DIR}/tools/profiles/${CONAN_PROFILE}

CMD ["echo", "'run this interactively'"]
