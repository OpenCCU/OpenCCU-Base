PLATFORMS ?= x86_64-linux-gnu aarch64-linux-gnu arm-linux-gnueabihf i686-linux-gnu
CMAKE ?= cmake
CMAKE_OPTIONS ?=
BUILD_TARGET ?= package

.PHONY: all build install clean

all: build

build: $(PLATFORMS:%=build-%)

build-%:
	$(CMAKE) --preset $* -DDEPLOY_TO_REPO=OFF -DHAS_USB_SUPPORT=ON $(CMAKE_OPTIONS)
	$(CMAKE) --build --preset $* --target $(BUILD_TARGET)

install: $(PLATFORMS:%=install-%)

install-%: build-%
	@set -e; rootfs_dir="build/$*/selected-install"; \
	rm -rf "$$rootfs_dir"; \
	$(CMAKE) --install "build/$*" --prefix "$(CURDIR)/$$rootfs_dir"; \
	$(CMAKE) -E make_directory "bin/$*" "lib/$*" "etc" "firmware" "www" "opt" "usr"; \
	if [ -d "$$rootfs_dir/bin" ]; then $(CMAKE) -E copy_directory "$$rootfs_dir/bin" "bin/$*"; fi; \
	if [ -d "$$rootfs_dir/lib" ]; then $(CMAKE) -E copy_directory "$$rootfs_dir/lib" "lib/$*"; fi; \
	for d in etc firmware www opt usr; do \
		if [ -d "$$rootfs_dir/$$d" ]; then $(CMAKE) -E copy_directory "$$rootfs_dir/$$d" "$$d"; fi; \
	done

clean:
	rm -rf $(PLATFORMS:%=build/%)
