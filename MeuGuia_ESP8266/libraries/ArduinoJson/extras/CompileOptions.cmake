if(NOT DEFINED COVERAGE)
	set(COVERAGE OFF)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
	add_compile_options(
		-pedantic
		-Wall
		-Wcast-align
		-Wcast-qual
		-Wconversion
		-Wctor-dtor-privacy
		-Wdisabled-optimization
		-Werror
		-Wextra
		-Wformat=2
		-Winit-self
		-Wmissing-include-dirs
		-Wnon-virtual-dtor
		-Wold-style-cast
		-Woverloaded-virtual
		-Wparentheses
		-Wredundant-decls
		-Wshadow
		-Wsign-conversion
		-Wsign-promo
		-Wstrict-aliasing
		-Wundef
	)

	if(${COVERAGE})
		set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9) AND(NOT ${COVERAGE}))
		add_compile_options(-g -Og)
	else()
		add_compile_options(
			-g
			-O0
			-Wno-shadow
			-Wp,-w
		)
		add_definitions(
			-DARDUINOJSON_ENABLE_STD_STRING=1
			-DARDUINOJSON_ENABLE_STD_STREAM=1
		)
	endif()

	add_compile_options(
		-Wstrict-null-sentinel
		-Wno-vla
	)
	add_definitions(-DHAS_VARIABLE_LENGTH_ARRAY)

	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.5)
		add_compile_options(-Wlogical-op)
	endif()

	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.6)
		add_compile_options(-Wnoexcept)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(
		-Wc++11-compat
		-Wdeprecated-register
		-Wno-vla-extension
	)
	add_definitions(
		-DHAS_VARIABLE_LENGTH_ARRAY
		-DSUBSCRIPT_CONFLICTS_WITH_BUILTIN_OPERATOR
	)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-stdlib=libc++)
	link_libraries(c++ m)

	if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.0) AND(NOT ${COVERAGE}))
		add_compile_options(-g -Og)
	else()
		add_compile_options(-g -O0)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.0) AND(NOT ${COVERAGE}))
		add_compile_options(-g -Og)
	else()
		add_compile_options(-g -O0)
	endif()
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_compile_options(
		/W4
		/WX
		/Zc:__cplusplus
	)
endif()

if(MINGW)
	add_link_options(-static)
endif()
