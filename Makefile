BUILDTYPE ?= Debug
JOBS ?= 1
ARCH ?= x64

all: can.io

build:
	./io_gyp -Dosx_arch=$(ARCH) -f make

can.io: build tools/can2c
	$(MAKE) -j $(JOBS) -C out can.io
	ln -sf out/$(BUILDTYPE)/can.io can.io

clean:
	-rm -rf out
	-rm can.io

.PHONY: clean all build can.io
