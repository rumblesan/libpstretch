FROM debian:8.6

MAINTAINER Guy John <slowradio@rumblesan.com>

RUN apt-get update
RUN apt-get install -y clang cmake make git
RUN apt-get install -y libfftw3-dev

ENV CC /usr/bin/clang

RUN git clone https://github.com/rumblesan/bclib.git /opt/bclib
RUN cd /opt/bclib/build && cmake .. && make && make install

COPY . /opt/libpstretch

WORKDIR /opt/bclib/build

RUN cmake ..; make; make test; make install

CMD ["true"]
