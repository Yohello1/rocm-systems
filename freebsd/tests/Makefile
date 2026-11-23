HIPCC ?= /opt/rocm/bin/hipcc
ARCH ?= gfx1151
TESTS = hipvadd hostmem streams shared
all: $(TESTS)
hipvadd: hipvadd.cpp
	$(HIPCC) --offload-arch=$(ARCH) -O2 hipvadd.cpp -o $@
hostmem: hostmem.cpp common.h
	$(HIPCC) --offload-arch=$(ARCH) -O2 hostmem.cpp -o $@
streams: streams.cpp common.h
	$(HIPCC) --offload-arch=$(ARCH) -O2 streams.cpp -o $@
shared: shared.cpp common.h
	$(HIPCC) --offload-arch=$(ARCH) -O2 shared.cpp -o $@
run: all
	@for t in $(TESTS); do echo "== $$t"; timeout 120 ./$$t 2>&1 | grep -v 'LoadLib\|xnack\|CUID'; pkill -9 -x $$t || true; done
clean:
	rm -f $(TESTS)
.PHONY: all run clean
