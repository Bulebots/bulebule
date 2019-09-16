ENGINE = podman
RUN = ${ENGINE} run -v $$(pwd):/bulebule:Z bulebule

ifeq (, $(shell command -v ${ENGINE}))
	$(warning "Command `${ENGINE}` not found, falling back to Docker...")
	ENGINE = sudo docker
	RUN = ${ENGINE} run -v $$(pwd):/bulebule:Z -u $$(id -u):$$(id -g) bulebule
endif

default: src/main.elf

src/main.elf:
	${RUN} make -s -C src/

.PHONY: libopencm3
libopencm3:
	${RUN} scripts/setup_libopencm3.sh

.PHONY: clean
clean:
	${RUN} make -s -C src/ clean

.PHONY: image
image:
	${ENGINE} build -t bulebule .
