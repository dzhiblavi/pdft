MAKE = make
CONAN = conan
CMAKE = cmake

BUILD_TYPE ?= Debug
CONAN_PRESET = conan-$(shell echo "$(BUILD_TYPE)" | tr '[:upper:]' '[:lower:]')
PROFILE ?= armv8-macos
ASAN ?= OFF
TSAN ?= OFF
UBSAN ?= OFF
TESTS ?= ON

BUILD_IMAGE = pdft-build-image
TARGET = target

TARGET_DIR = ./$(TARGET)/$(PROFILE)/$(BUILD_TYPE)

all:
	:

clean:
	rm -rf $(TARGET_DIR)

_prepare_target_dirs:
	mkdir -p $(TARGET_DIR)

deps: _prepare_target_dirs
	$(CONAN) install .                             \
		--output-folder=$(TARGET_DIR)              \
		--build=missing                            \
		--profile:host=./conan/$(PROFILE).profile  \
		--profile:build=./conan/$(PROFILE).profile \
		--settings:host=build_type=$(BUILD_TYPE)   \
		--settings:build=build_type=$(BUILD_TYPE)

configure: deps
	cmake --preset $(CONAN_PRESET)                        \
		-DPDFT_BUILD_TESTS=$(TESTS)                       \
		-DPDFT_MUPDF_LOCAL_SOURCE_DIR=${MUPDF_SOURCE_DIR} \
		-DPDFT_USE_CUSTOM_LIBCXX=$(LIBCXX_PATH)           \
		-DASAN=$(ASAN)                                    \
		-DTSAN=$(TSAN)                                    \
		-DUBSAN=$(UBSAN)

build: configure
	cmake --build --preset $(CONAN_PRESET)                \
		-DPDFT_BUILD_TESTS=$(TESTS)                       \
		-DPDFT_MUPDF_LOCAL_SOURCE_DIR=${MUPDF_SOURCE_DIR} \
		-DPDFT_USE_CUSTOM_LIBCXX=$(LIBCXX_PATH)           \
		-DASAN=$(ASAN)                                    \
		-DTSAN=$(TSAN)                                    \
		-DUBSAN=$(UBSAN)

gen-doc: deps
	cd $(TARGET_DIR) && $(CMAKE) --build . --target documentation

test: build
	cd $(TARGET_DIR) && ctest --output-on-failure

check-tidy: configure
	run-clang-tidy                   \
		-quiet                       \
		-use-color                   \
		-j `nproc`                   \
		-p $(TARGET_DIR)             \
		-header-filter=include/pdft/ \
		`find src/pdft -name '*.cpp'`

check-format:
	find include/ src/pdft/ -type f -name '*.h' -o -name '*.cpp' \
		| xargs clang-format --dry-run --Werror

do-format:
	find include/ src/pdft/ -type f -name '*.h' -o -name '*.cpp' \
		| xargs clang-format -i

run-lldb: build
	cd $(TARGET_DIR) && lldb -- ./src/pdft/cli/pdft $(args)

run-gdb: build
	cd $(TARGET_DIR) && gdb --args ./src/pdft/cli/pdft $(args)

run: build
	cd $(TARGET_DIR) && ./src/pdft/cli/pdft $(args)

install-local: build
	cd $(TARGET_DIR) && cp ./src/pdft/cli/pdft ~/.bin/pdft

build-docker-image:
	docker build                   \
		-t $(BUILD_IMAGE)          \
		-f docker/build.Dockerfile \
		.

docker-run:
	docker run -it --rm             \
		-v `pwd`:/pdft              \
		--name pdft-build-container \
		$(BUILD_IMAGE):latest       \
		/bin/bash
