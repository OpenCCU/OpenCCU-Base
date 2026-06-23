PLATFORMS ?= x86_64-linux-gnu aarch64-linux-gnu arm-linux-gnueabihf i686-linux-gnu
CMAKE ?= cmake
ROOTFS_SUBDIR ?= rootfs
BUILD_TARGET ?= package

.PHONY: all build install clean

all: build

build: $(PLATFORMS:%=build-%)

build-%:
	$(CMAKE) --preset $* -DDEPLOY_TO_REPO=OFF
	$(CMAKE) --build --preset $* --target $(BUILD_TARGET)

install: $(PLATFORMS:%=install-%)

install-%: build-%
	@rootfs_dir="build/$*/$(ROOTFS_SUBDIR)"; \
	if [ ! -d "$$rootfs_dir" ]; then echo "Missing staged rootfs: $$rootfs_dir"; exit 1; fi; \
	$(CMAKE) -E make_directory "bin/$*" "lib/$*" "etc" "firmware" "www" "opt" "usr"; \
	if [ -d "$$rootfs_dir/bin" ]; then $(CMAKE) -E copy_directory "$$rootfs_dir/bin" "bin/$*"; fi; \
	if [ -d "$$rootfs_dir/lib" ]; then $(CMAKE) -E copy_directory "$$rootfs_dir/lib" "lib/$*"; fi; \
	for d in etc firmware www opt usr; do \
		if [ -d "$$rootfs_dir/$$d" ]; then $(CMAKE) -E copy_directory "$$rootfs_dir/$$d" "$$d"; fi; \
	done

clean:
	rm -rf $(PLATFORMS:%=build/%)
