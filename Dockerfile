FROM fedora:30

ENV LANG C.UTF-8

RUN dnf update -y \
    && dnf install -y \
	arm-none-eabi-gcc-cs-7.4.0 \
	arm-none-eabi-newlib-3.1.0-2.fc30 \
        curl \
        findutils \
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
