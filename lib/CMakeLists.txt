
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

add_library(pstretch ${HEADERS} ${SOURCES})
set_property(TARGET pstretch PROPERTY C_STANDARD 99)

target_include_directories(pstretch
  PUBLIC include
  PRIVATE src)

target_link_libraries(pstretch bclib)
target_link_libraries(pstretch fftw3f)
target_link_libraries(pstretch m)

install(TARGETS pstretch EXPORT LibPStretchTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/pstretch)
