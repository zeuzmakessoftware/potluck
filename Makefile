.DEFAULT_GOAL := run

.PHONY: configure build run test release release-run clean help

configure:
	cmake --preset dev

build: configure
	cmake --build --preset dev

run: build
	./build/dev/potluck

test: build
	ctest --preset dev --output-on-failure

release:
	cmake --preset release
	cmake --build --preset release

release-run: release
	./build/release/potluck

clean:
	cmake --build --preset dev --target clean
	@if [ -d build/release ]; then cmake --build --preset release --target clean; fi

help:
	@printf '%s\n' \
		'make              Build and run the debug game' \
		'make build        Build the debug game' \
		'make run          Build and run the debug game' \
		'make test         Build and run the test suite' \
		'make release      Build the release game' \
		'make release-run  Build and run the release game' \
		'make clean        Clean generated targets'
