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
