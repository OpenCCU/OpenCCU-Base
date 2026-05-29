
include ../eq3version.txt

RFD_VERSION = "$(EQ3_VERSION_MAJOR).$(EQ3_VERSION_MINOR).$(EQ3_VERSION_BUGFIX).$(EQ3_VERSION_REVISION)"

all:
	@echo #define RFD_VERSION $(RFD_VERSION) > generated-rfd-version.h