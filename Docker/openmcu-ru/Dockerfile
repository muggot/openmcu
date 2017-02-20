FROM openmcu/base-image

RUN echo "--------Install OPENMCU---------" \
 && cd /tmp && git clone git://github.com/muggot/openmcu.git ./openmcu-ru \
 && cd /tmp/openmcu-ru \
 && ./autogen.sh \
 && ./configure \
 && make && make install \
 && rm -rf /tmp/* 

EXPOSE 5060 5061 1420 1554 1720
CMD ["/opt/openmcu-ru/bin/openmcu-ru"]