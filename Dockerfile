FROM fedora:30

ENV LANG C.UTF-8

RUN dnf update -y \
    && dnf install -y \
	arm-none-eabi-gcc \
	arm-none-eabi-newlib \
        curl \
        git \
	make \
	openocd \
	python \
        wget \
        which \
    && dnf clean all
RUN alias python='python3'

VOLUME /bulebule
WORKDIR /bulebule
