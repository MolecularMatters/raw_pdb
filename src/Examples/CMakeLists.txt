project(Examples)

set(SOURCES
	ExampleContributions.cpp
	ExampleFunctionSymbols.cpp
	ExampleFunctionVariables.cpp
	ExampleIPI.cpp
	ExampleLines.cpp
	ExampleMain.cpp
	ExampleMemoryMappedFile.cpp
	ExampleMemoryMappedFile.h
	ExamplePDBSize.cpp
	Examples_PCH.cpp
	Examples_PCH.h
	ExampleSymbols.cpp
	ExampleTimedScope.cpp
	ExampleTimedScope.h
	ExampleTypes.cpp
	ExampleTypeTable.cpp
	ExampleTypeTable.h
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