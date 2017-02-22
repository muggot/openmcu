FROM ubuntu:14.04

# Install dependenties

ENV MAKE_OPTS="-j 3"
ENV PREFIX="/opt"

RUN apt-get update \
 && apt-get install -y \
 build-essential libssl-dev flex bison autoconf automake pkg-config \
 libfreetype6-dev libjpeg62-dev md5deep libtool libavformat-dev libvpx-dev \
 libavcodec-dev libavutil-dev libswscale-dev libsamplerate0-dev git wget \
 && cd /tmp && export PATH=$PREFIX/bin:$PATH \
 && echo "--------YASM---------" \
 && wget http://www.tortall.net/projects/yasm/releases/yasm-1.2.0.tar.gz \
 && tar xvfz ./yasm-1.2.0.tar.gz && mv ./yasm-1.2.0 ./yasm && rm ./yasm-1.2.0.tar.gz \
 && cd /tmp/yasm && ./configure --prefix=$PREFIX \
 && make $MAKE_OPTS && make install \
 && echo "--------SOFIA-SIP---------" \
 && cd .. && wget https://videoswitch.ru/public/dependencies/stable/sofia-sip-1.12.11.tar.gz \
 && tar xvfz ./sofia-sip-1.12.11.tar.gz && mv ./sofia-sip-1.12.11 ./sofia-sip && rm ./sofia-sip-1.12.11.tar.gz \
 && cd /tmp/sofia-sip && ./configure --prefix=$PREFIX \
 && make $MAKE_OPTS && make install \
 && echo "--------OPUS---------" \
 && cd .. && wget https://videoswitch.ru/public/dependencies/stable/opus-1.1.tar.gz \
 && tar xvfz ./opus-1.1.tar.gz && mv ./opus-1.1 ./libopus && rm ./opus-1.1.tar.gz \
 && cd /tmp/libopus && ./configure --enable-shared --prefix=$PREFIX \
 && make clean && make $MAKE_OPTS && make install \
 && echo "--------LIBVPX---------" \
 && cd .. && wget https://videoswitch.ru/public/dependencies/stable/libvpx-v1.1.0.tar.bz2 \
 && tar xvfj ./libvpx-v1.1.0.tar.bz2 && mv ./libvpx-v1.1.0 ./libvpx && rm ./libvpx-v1.1.0.tar.bz2 \
 && cd /tmp/libvpx && ./configure --prefix=$PREFIX --enable-shared \
 && make clean && make $MAKE_OPTS && make install \
 && echo "--------X264---------" \
 && cd .. && git clone git://git.videolan.org/x264.git ./x264 \
 && cd /tmp/x264 && ./configure --prefix=$PREFIX --enable-shared \
 && make clean && make $MAKE_OPTS && make install \
 &&  echo "--------FFMPEG---------" \
 && cd .. && wget https://videoswitch.ru/public/dependencies/stable/ffmpeg-0.10.4.tar.gz \
 && tar xvfz ./ffmpeg-0.10.4.tar.gz && mv ./ffmpeg-0.10.4 ./ffmpeg && rm ./ffmpeg-0.10.4.tar.gz && cd /tmp/ffmpeg \
 && ./configure --enable-libx264 --enable-libvpx --enable-gpl --disable-static \
 --enable-shared --prefix=$PREFIX --extra-cflags=-I$PREFIX/include/ --extra-ldflags=-L$PREFIX/lib/ \
 && make clean && make $MAKE_OPTS && make install \
 && rm -rf /tmp/* && rm -rf /var/cache/apt/*
