
include_directories(include)

set(SOURCES "src/audiofile.c" "wavstretch.c" "src/filechunk.c" "src/ogg_encoder.c")

add_executable(stretch ${SOURCES})
set_property(TARGET stretch PROPERTY C_STANDARD 99)
target_link_libraries(stretch pstretch)

target_link_libraries(stretch sndfile)

target_link_libraries(stretch vorbis vorbisfile ogg vorbisenc)
