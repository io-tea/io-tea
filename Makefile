all: mbed-os/ BUILD/

mbed-os/:
	mbed deploy

BUILD/: mbed-os/
	mbed compile --profile profile.json --library

.PHONY: clean-build clean-mbed clean clean-all

clean-build:
	rm -rf BUILD

clean: clean-build

clean-mbed:
	rm -rf mbed-os

clean-all: clean-build clean-mbed
