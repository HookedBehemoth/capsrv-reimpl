all: sys

clean:
	$(MAKE) -C capsrv clean
	rm -rf dist/

sys:
	$(MAKE) -C capsrv
	-mkdir -p dist/0100000000000022
	cp capsrv/capsrv.nsp dist/0100000000000022/exefs.nsp

.PHONY: all