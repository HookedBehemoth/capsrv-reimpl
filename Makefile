all: nro

nro:
	$(MAKE) -C capsrv

clean: sys-clean
	$(MAKE) -C capsrv clean

sys:
	$(MAKE) -C capsrv -f sys-Makefile
	rm -rf dist
	mkdir dist
	mkdir dist/0100000000000022
	mkdir dist/0100000000000022/flags
	touch dist/0100000000000022/flags/boot2.flag
	cp capsrv/capsrv.nsp dist/0100000000000022/exefs.nsp

sys-clean:
	$(MAKE) -C capsrv -f sys-Makefile clean
	rm -rf dist/

.PHONY: all