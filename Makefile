PLATFORMS ?= x86_64-linux-gnu aarch64-linux-gnu arm-linux-gnueabihf i686-linux-gnu
CMAKE ?= cmake
CMAKE_OPTIONS ?=
BUILD_TARGET ?= package

.PHONY: all build install deploy clean

all: build

build: $(PLATFORMS:%=build-%)

build-%:
	$(CMAKE) --preset $* -DDEPLOY_TO_REPO=OFF -DHAS_USB_SUPPORT=ON $(CMAKE_OPTIONS)
	$(CMAKE) --build --preset $* --target $(BUILD_TARGET)

install: $(PLATFORMS:%=install-%)

install-%: build-%
	@set -e; rootfs_dir="build/$*/selected-install"; \
	rm -rf "$$rootfs_dir"; \
	$(CMAKE) --install "build/$*" --prefix "$(CURDIR)/$$rootfs_dir"

deploy: $(PLATFORMS:%=deploy-%)

deploy-%: install-%
	@set -e; rootfs_dir="build/$*/selected-install"; \
	$(CMAKE) -E make_directory "bin/$*" "lib/$*" "etc" "firmware" "www" "opt" "usr"; \
	if [ -d "$$rootfs_dir/bin" ]; then cp -a "$$rootfs_dir/bin/." "bin/$*/"; fi; \
	if [ -d "$$rootfs_dir/lib" ]; then cp -a "$$rootfs_dir/lib/." "lib/$*/"; fi; \
	for d in etc firmware www opt usr; do \
		if [ -d "$$rootfs_dir/$$d" ]; then cp -a "$$rootfs_dir/$$d/." "$$d/"; fi; \
	done

clean:
	rm -rf $(PLATFORMS:%=build/%)
