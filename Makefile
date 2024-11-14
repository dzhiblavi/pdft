MAKE = make
CONAN = conan
CMAKE = cmake

depsdir = ./target/$(1)/deps
builddir = ./target/$(1)/build

all:
	:

clean-%:
	rm -rf $(call builddir,$*)

clean: clean-Debug clean-Release clean-RelWithDebInfo
	$(MAKE) clean-Debug
	$(MAKE) clean-Release
	$(MAKE) clean-RelWithDebInfo

_prepare_target-%:
	mkdir -p $(call depsdir,$*)
	mkdir -p $(call builddir,$*)

deps-%: _prepare_target-%
	$(CONAN) install .                     \
		--output-folder=$(call depsdir,$*) \
		--build=missing                    \
		-s build_type=$*

configure-%: deps-%
	cd $(call builddir,$*) && $(CMAKE) ../../../               \
		-G "Unix Makefiles"                                    \
		-DCMAKE_TOOLCHAIN_FILE="../deps/conan_toolchain.cmake" \
		-DFRAMEWORK_BUILD_TESTS=Yes

build-%: configure-%
	cd $(call builddir,$*) && $(CMAKE) --build . --config $*

test: build-Debug
	cd $(call builddir,Debug) && ctest --output-on-failure
