add_executable(Examples
	ExampleContributions.cpp
	ExampleFunctionSymbols.cpp
	ExampleMain.cpp
	ExampleMemoryMappedFile.cpp
	ExampleMemoryMappedFile.h
	Examples_PCH.cpp
	Examples_PCH.h
	ExampleSymbols.cpp
	ExampleTimedScope.cpp
	ExampleTimedScope.h
)

target_link_libraries(Examples
PRIVATE
	raw_pdb
)

target_precompile_headers(Examples
PRIVATE
	Examples_PCH.h
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(Examples
	PRIVATE
		-fdeclspec -fms-extensions
	)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(Examples
	PRIVATE
		/MP # Build with Multiple Processes.
	)
endif()
