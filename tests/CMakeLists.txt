add_executable(confind_characterize characterize.cpp)
add_executable(confind_export export.cpp)
foreach(target confind_characterize confind_export)
  target_link_libraries(${target} PRIVATE CONFIND::CONFIND)
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
    target_compile_options(${target} PRIVATE -ffp-contract=off)
  endif()
endforeach()
add_test(NAME historical_serial COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:confind_characterize> -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/serial.tsv
  -DREFERENCE=${CMAKE_CURRENT_SOURCE_DIR}/reference/serial.tsv -P ${CMAKE_CURRENT_SOURCE_DIR}/compare.cmake)
add_test(NAME historical_export COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:confind_export> -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/export
  -DREFERENCE=${CMAKE_CURRENT_SOURCE_DIR}/reference -P ${CMAKE_CURRENT_SOURCE_DIR}/export.cmake)

foreach(workers 1 2 4 8)
  add_test(NAME historical_samples_workers_${workers} COMMAND ${CMAKE_COMMAND}
    -DPROGRAM=$<TARGET_FILE:confind_characterize> -DWORKERS=${workers}
    -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/samples-${workers}.tsv
    -DREFERENCE=${CMAKE_CURRENT_SOURCE_DIR}/reference/serial.tsv -P ${CMAKE_CURRENT_SOURCE_DIR}/compare.cmake)
endforeach()
add_executable(confind_safety safety.cpp)
target_link_libraries(confind_safety PRIVATE CONFIND::CONFIND)
set_target_properties(confind_safety PROPERTIES CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
  target_compile_options(confind_safety PRIVATE -ffp-contract=off)
endif()
add_test(NAME ownership_inputs_exceptions COMMAND confind_safety ${CMAKE_CURRENT_BINARY_DIR}/safety-output)
add_test(NAME scheduler_stress_80_runs COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:confind_characterize> -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}
  -DREFERENCE=${CMAKE_CURRENT_SOURCE_DIR}/reference/serial.tsv -P ${CMAKE_CURRENT_SOURCE_DIR}/stress.cmake)
set_tests_properties(scheduler_stress_80_runs PROPERTIES TIMEOUT 240)
option(CONFIND_BUILD_BENCHMARK "Build the deterministic expensive-evaluator benchmark" OFF)
if(CONFIND_BUILD_BENCHMARK)
  add_executable(confind_benchmark benchmark.cpp)
  target_link_libraries(confind_benchmark PRIVATE CONFIND::CONFIND)
  set_target_properties(confind_benchmark PROPERTIES CXX_EXTENSIONS OFF)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
    target_compile_options(confind_benchmark PRIVATE -ffp-contract=off)
  endif()
endif()
configure_file(consumer.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/consumer.cmake @ONLY)
add_test(NAME installed_package_consumer COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/consumer.cmake)

# Keep evaluator/sample arithmetic independent of the library optimization level.
# Both oracle and candidate harnesses use -O0 and contraction-off; the library
# remains built at the requested Debug/Release optimization level.
add_executable(confind_m1_characterize m1_characterize.cpp)
target_compile_definitions(confind_m1_characterize PRIVATE CONFIND_MODERN)
add_executable(confind_m1_differential m1_differential.cpp)
foreach(target confind_m1_characterize confind_m1_differential)
  target_link_libraries(${target} PRIVATE CONFIND::CONFIND)
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
    target_compile_options(${target} PRIVATE -O0 -ffp-contract=off)
  endif()
endforeach()
foreach(mode small stellar)
  add_test(NAME m1_historical_${mode} COMMAND ${CMAKE_COMMAND}
    -DPROGRAM=$<TARGET_FILE:confind_m1_characterize> -DMODE=${mode}
    -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/m1-${mode}.tsv
    -DREFERENCE=${CMAKE_CURRENT_SOURCE_DIR}/reference/m1-${mode}.tsv
    -P ${CMAKE_CURRENT_SOURCE_DIR}/m1_compare.cmake)
endforeach()
foreach(mode log linear)
  if(mode STREQUAL "log")
    set(mask 1)
  else()
    set(mask 2)
  endif()
  add_test(NAME m1_randomized_${mode}_600 COMMAND ${CMAKE_COMMAND}
    -DPROGRAM=$<TARGET_FILE:confind_m1_differential> -DMASK=${mask}
    -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/m1-${mode}.tsv
    -DREFERENCE=${CMAKE_CURRENT_SOURCE_DIR}/reference/m1-${mode}.tsv
    -P ${CMAKE_CURRENT_SOURCE_DIR}/m1_differential.cmake)
endforeach()
