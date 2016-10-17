
include_directories(include)

set(HEADERS
  "include/fft.h"
  "include/pstretch.h"
  "include/audiobuffer.h"
  "include/stretch.h"
  )

set(SOURCES
  "src/fft.c"
  "src/audiobuffer.c"
  "src/stretch.c"
  )

add_library(pstretch STATIC ${SOURCES})
set_property(TARGET pstretch PROPERTY C_STANDARD 99)

set_target_properties(pstretch PROPERTIES
    PUBLIC_HEADER "${HEADERS}")

target_link_libraries(pstretch bclib)
target_link_libraries(pstretch fftw3f)
target_link_libraries(pstretch m)

install(TARGETS pstretch
    DESTINATION lib
    EXPORT LibPStretchTargets
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/pstretch"
    COMPONENT dev
    )
