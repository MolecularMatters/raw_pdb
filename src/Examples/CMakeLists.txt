project(Examples)

set(SOURCES
	ExampleContributions.cpp
	ExampleFunctionSymbols.cpp
	ExampleLines.cpp
	ExampleMain.cpp
	ExampleMemoryMappedFile.cpp
	ExampleMemoryMappedFile.h
	Examples_PCH.cpp
	Examples_PCH.h
	ExampleSymbols.cpp
	ExampleTypes.cpp
	ExampleTimedScope.cpp
	ExampleTimedScope.h
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