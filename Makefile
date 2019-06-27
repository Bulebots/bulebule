DOCKER_RUN = sudo docker run -v $$(pwd):/bulebule:Z -u $$(id -u):$$(id -g) bulebule

default: src/main.elf

src/main.elf:
	${DOCKER_RUN} make -s -C src/

.PHONY: libopencm3
libopencm3:
	${DOCKER_RUN} scripts/setup_libopencm3.sh

.PHONY: clean
clean:
	${DOCKER_RUN} make -s -C src/ clean

.PHONY: docker
docker:
	sudo docker build -t bulebule .
