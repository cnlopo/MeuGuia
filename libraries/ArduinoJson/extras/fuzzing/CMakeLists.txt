# ArduinoJson - https://arduinojson.org
# Copyright © 2014-2026, Benoit BLANCHON
# MIT License

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	add_compile_options(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(msgpack_reproducer
	msgpack_fuzzer.cpp
	reproducer.cpp
)
target_link_libraries(msgpack_reproducer
	ArduinoJson
)

add_executable(json_reproducer
	json_fuzzer.cpp
	reproducer.cpp
)
target_link_libraries(json_reproducer
	ArduinoJson
)

macro(add_fuzzer name)
	set(FUZZER "${name}_fuzzer")
	set(CORPUS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${name}_corpus")
	set(SEED_CORPUS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${name}_seed_corpus")
	add_executable("${FUZZER}"
		"${name}_fuzzer.cpp"
	)
	target_link_libraries("${FUZZER}"
		ArduinoJson
	)
	set_target_properties("${FUZZER}"
		PROPERTIES
			COMPILE_FLAGS "-fprofile-instr-generate -fcoverage-mapping -fsanitize=fuzzer -fno-sanitize-recover=all"
			LINK_FLAGS "-fprofile-instr-generate -fcoverage-mapping -fsanitize=fuzzer -fno-sanitize-recover=all"
	)

	add_test(
		NAME "${FUZZER}"
		COMMAND "${FUZZER}" "${CORPUS_DIR}" "${SEED_CORPUS_DIR}" -max_total_time=5 -timeout=1
	)

	set_tests_properties("${FUZZER}"
		PROPERTIES
		LABELS "Fuzzing"
	)
endmacro()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
	if(DEFINED ENV{GITHUB_ACTIONS} AND CMAKE_CXX_COMPILER_VERSION MATCHES "^11\\.")
		message(WARNING "Fuzzing is disabled on GitHub Actions to workaround a bug in Clang 11")
		return()
	endif()

	add_fuzzer(json)
	add_fuzzer(msgpack)
endif()
