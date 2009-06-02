all-local:
	if test ! -d build; then \
	mkdir build; \
	cd build && cmake ../libpgm-1.0.0/openpgm/pgm/ -DLIBPGM_ONLY=YES; \
	make; \
	else :; fi
clean-local:
	-rm -rf build

