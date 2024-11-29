MAKE = make
CONAN = conan
CMAKE = cmake

BUILD_TYPE ?= Debug
PROFILE ?= armv8-macos
ASAN ?= OFF
TSAN ?= OFF
UBSAN ?= OFF

BUILD_IMAGE = pdft-build-image

TARGET_DIR = ./target/$(PROFILE)/$(BUILD_TYPE)
DEPS_DIR = $(TARGET_DIR)/deps
BUILD_DIR = $(TARGET_DIR)/build

all:
	:

clean:
	rm -rf $(TARGET_DIR)

_prepare_target_dirs:
	mkdir -p $(DEPS_DIR)
	mkdir -p $(BUILD_DIR)

deps: _prepare_target_dirs
	$(CONAN) install .                             \
		--output-folder=$(DEPS_DIR)                \
		--build=missing                            \
		--profile:host=./conan/$(PROFILE).profile  \
		--profile:build=./conan/$(PROFILE).profile \
		--settings:host=build_type=$(BUILD_TYPE)   \
		--settings:build=build_type=$(BUILD_TYPE)

configure: deps
	cd $(BUILD_DIR) && $(CMAKE) ../../../../                       \
		-G "Unix Makefiles"                                        \
		-DCMAKE_TOOLCHAIN_FILE="../deps/conan_toolchain.cmake"     \
		-DCMAKE_POLICY_DEFAULT_CMP0091=NEW                         \
		-DCMAKE_C_COMPILER=${CC}                                   \
		-DCMAKE_CXX_COMPILER=${CXX}                                \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE)                           \
		-DPDFT_BUILD_TESTS=ON                                      \
		-DASAN=$(ASAN)                                             \
		-DTSAN=$(TSAN)                                             \
		-DUBSAN=$(UBSAN)

build: configure
	cd $(BUILD_DIR) && $(CMAKE) --build . --config $(BUILD_TYPE)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

run-lldb: build
	cd $(BUILD_DIR) && lldb -- ./src/app/app $(path)

run-gdb: build
	cd $(BUILD_DIR) && gdb --args ./src/app/app $(path)

run: build
	cd $(BUILD_DIR) && ./src/app/app $(path)

install-dev: build
	cd $(BUILD_DIR) && cp ./src/app/app ~/.bin/pdft-dev

install-local: build
	cd $(BUILD_DIR) && cp ./src/app/app ~/.bin/pdft

docker-build:
	docker build                           \
		-t $(BUILD_IMAGE)                  \
		-f docker/build.Dockerfile         \
		--platform linux/amd64,linux/arm64 \
		.
