FROM debian:bullseye-slim as base

FROM base as omnetpp-build
ARG VERSION=5.6.2
WORKDIR /root
RUN apt-get update && apt-get install -y \
    bison \
    build-essential \
    flex \
    libxml2-dev \
    wget \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*
RUN wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-$VERSION/omnetpp-$VERSION-src-core.tgz \
    --progress=bar:force:noscroll -O omnetpp-src-core.tgz && \
    tar xf omnetpp-src-core.tgz && \
    rm omnetpp-src-core.tgz && \
    mv omnetpp-$VERSION /omnetpp
WORKDIR /omnetpp
ENV PATH /omnetpp/bin:$PATH
RUN ./configure WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no && \
    make -j $(nproc) base MODE=release

FROM omnetpp-build as omnetpp-debug
RUN make -j $(nproc) base MODE=debug

FROM base as artery-build
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost1.74-dev \
    libboost-date-time1.74-dev \
    libboost-system1.74-dev \
    libcrypto++-dev \
    libgeographic-dev \
    libpython3.9-dev \
    libssl-dev \
    libzmq3-dev \
    pkg-config \
    python3-distutils \
    && rm -rf /var/lib/apt/lists/*
COPY --from=omnetpp-build /omnetpp/bin /omnetpp/bin
COPY --from=omnetpp-build /omnetpp/include /omnetpp/include
COPY --from=omnetpp-build /omnetpp/lib /omnetpp/lib
COPY --from=omnetpp-build /omnetpp/Makefile.inc /omnetpp/Version /omnetpp/
COPY . /artery/source
ENV PATH /omnetpp/bin:$PATH
RUN cmake -S /artery/source -B /artery/build -DCMAKE_BUILD_TYPE=Release -DWITH_OTS=ON -DWITH_SIMULTE=ON \
    -DCMAKE_INSTALL_PREFIX=/artery -DCMAKE_INSTALL_RPATH=/artery/lib -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
    && cmake --build /artery/build --parallel $(nproc) --target install

FROM base as sumo-build
ARG VERSION=1_6_0
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libproj-dev \
    libxerces-c-dev \
    python3 \
    python3-setuptools \
    wget \
    && rm -rf /var/lib/apt/lists/*
RUN wget https://github.com/eclipse/sumo/archive/v$VERSION.tar.gz \
    --progress=bar:force:noscroll -O sumo-src.tar.gz && \
    tar xfz sumo-src.tar.gz && \
    rm sumo-src.tar.gz
RUN cmake -S sumo-$VERSION -B build-sumo -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/sumo && \
    cmake --build build-sumo --parallel $(nproc) --target install

FROM base as run
RUN apt-get update && apt-get install -y \
    libboost-date-time1.74 \
    libboost-system1.74 \
    libcrypto++ \
    libgeographic19 \
    libproj19 \
    libpython3.9 \
    libssl1.1 \
    libxerces-c3.2 \
    libxml2 \
    libzmq5 \
    python3 \
    make \
    && rm -rf /var/lib/apt/lists/*
COPY --from=omnetpp-build /omnetpp/bin /omnetpp/bin
COPY --from=omnetpp-build /omnetpp/lib /omnetpp/lib
COPY --from=sumo-build /sumo/bin/sumo /sumo/bin/sumo
COPY --from=sumo-build /sumo/share/sumo/data /sumo/share/sumo/data
COPY --from=artery-build /artery/bin /artery/bin
COPY --from=artery-build /artery/lib /artery/lib
COPY --from=artery-build /artery/share/ned /artery/share/ned
ENV SUMO_HOME /sumo/share/sumo
ENV PATH /sumo/bin:/omnetpp/bin:$PATH
RUN ln -s /usr/bin/python3 /usr/bin/python
RUN useradd -m artery
RUN mkdir -p /scenario /results && chown -R artery:users /scenario /results
USER artery
VOLUME /scenario /results
WORKDIR /scenario
ENTRYPOINT ["/artery/bin/run_artery.sh", "--result-dir=/results"]
