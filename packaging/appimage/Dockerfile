FROM ubuntu:focal
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update &&   \
    apt -y upgrade && \
    apt install -y  \
        build-essential \
        libogg-dev  \
        libsdl2-dev \
        liblua5.1-0-dev \
        libpng-dev  \
        libtheora-dev \
        libvorbis-dev \
        python3 \
        zlib1g-dev &&  \
    apt install -y  \
        curl    \
        wget && \
    rm -rf /var/lib/apt/lists

ENV TOOLS_DIR="/tools"
RUN mkdir -p $TOOLS_DIR
ARG ARCH=x86_64
RUN /bin/bash -c 'cd $TOOLS_DIR \
    && curl -LO https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20230711-2/linuxdeploy-${ARCH}.AppImage \
    && chmod +x linuxdeploy-${ARCH}.AppImage \
    && ./linuxdeploy-${ARCH}.AppImage --appimage-extract \
    && rm ./linuxdeploy-${ARCH}.AppImage \
    && cd -'

RUN useradd bosbuilder && passwd -d bosbuilder

CMD ["/bin/bash","-l"]
