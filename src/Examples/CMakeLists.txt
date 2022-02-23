project(Examples)

set(SOURCES
    ExampleMemoryMappedFile.cpp
    ExampleMemoryMappedFile.h
    ExampleSymbols.cpp
    Examples_PCH.cpp
    Examples_PCH.h
)

source_group(src FILES
    ${SOURCES}
)

add_executable(Examples
    ${SOURCES}
)

target_link_libraries(Examples
  PUBLIC
    raw_pdb
)

target_precompile_headers(Examples
  PUBLIC
    Examples_PCH.h
)
