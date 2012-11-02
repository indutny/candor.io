BUILDTYPE ?= Debug
JOBS ?= 1
ARCH ?= x64

all: can.io

build:
	./io_gyp -Dosx_arch=$(ARCH) -f make

BINDINGS = src/bindings/can.cc

src/bindings/%.cc: lib/%.can
	./tools/can2c $< $@

can.io: build tools/can2c $(BINDINGS)
	$(MAKE) -j $(JOBS) -C out can.io
	ln -sf out/$(BUILDTYPE)/can.io can.io

tools/can2c: build
	$(MAKE) -j $(JOBS) -C out can2c
	ln -sf ../out/$(BUILDTYPE)/can2c tools/can2c

clean:
	-rm -rf out
	-rm can.io
	-rm tools/can2c

.PHONY: clean all build can.io
