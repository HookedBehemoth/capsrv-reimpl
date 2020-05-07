all: sys

clean:
	$(MAKE) -C capsrv clean
	rm -rf dist/

sys:
	-rm capsrv/build/capsrv_main.*
	-rm capsrv/build/logger.*
	$(MAKE) -C capsrv
	-mkdir -p dist/0100000000000022/flags
	-touch dist/0100000000000022/flags/boot2.flag
	cp capsrv/capsrv.nsp dist/0100000000000022/exefs.nsp

.PHONY: all