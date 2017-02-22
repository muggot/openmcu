FROM videoswitch/base-image:3.4x

RUN echo "--------УСТАНОВКА OPENMCU---------" \
 && cd /tmp && git clone git://github.com/muggot/openmcu.git ./openmcu-ru \
 && cd /tmp/openmcu-ru \
 && git branch -r \
 && git checkout 3.48 \
 && ./autogen.sh \
 && ./configure \
 && make && make install \
 && rm -rf /tmp/* 

