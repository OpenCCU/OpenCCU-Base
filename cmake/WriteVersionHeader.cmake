if(NOT DEFINED MACRO_NAME)
  message(FATAL_ERROR "MACRO_NAME must be set")
endif()

file(READ "${INPUT}" VERSION_TEXT)
string(STRIP "${VERSION_TEXT}" VERSION_TEXT)
file(WRITE "${OUTPUT}" "#define ${MACRO_NAME} \"${VERSION_TEXT}\"\n")
