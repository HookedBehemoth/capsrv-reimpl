all: sys

nro:
	$(MAKE) -C capsrv

clean:
	$(MAKE) -C capsrv clean

sys:
	$(MAKE) -C capsrv -f sys-Makefile

sys-clean:
	$(MAKE) -C capsrv -f sys-Makefile clean

.PHONY: all