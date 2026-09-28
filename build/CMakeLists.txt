cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(CONFIND VERSION 1.0 DESCRIPTION "CONFIND: A package for finding contours")

string(TIMESTAMP CONFIND_RELEASE_DATE "%m, %d, %Y")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-pthread)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)

# if(${_host_name} STREQUAL "dm.itp.ac.cn")
#     set(ROOT_LIB_DIR /opt/gentoo/usr/lib/root/6.18/lib)
#     set(ROOT_INC_DIR /opt/gentoo/usr/lib/root/6.18/include)
#     set(ZAKI_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/Zaki/linux)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# elseif(${_host_name} STREQUAL "Mohammadrezas-MacBook-Pro.local")
#     set(ROOT_LIB_DIR /opt/local/lib/root6)
#     set(ROOT_INC_DIR /opt/local/include/root6)
#     set(ZAKI_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/Zaki/darwin)
#     set(CMAKE_CXX_COMPILER clang++-mp-9.0)
#     set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")


# else()
#     message("Error: Hostname unknown!")
# endif()

# if(${_host_name} STREQUAL "Lumiere.local")
#     set(ROOT_LIB_DIR /opt/local/lib/root6)
#     set(ROOT_INC_DIR /opt/local/libexec/root6/include/root)
#     # set(ZAKI_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/Zaki/darwin)
#     # set(OS_NAME darwin)

# else()
#     message("Error: Hostname unknown!")
#     message("Error: Setup the include and library paths in 'CMakeLists.txt'.")
#     message("Your host name: " ${_host_name})
# endif()

# set(root_cmake_path "/opt/local/libexec/root6/share/root/cmake")
# set(CMAKE_PREFIX_PATH ${root_cmake_path})

# find_package(ROOT REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GSL REQUIRED)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Python3 COMPONENTS NumPy REQUIRED)

# list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})


set(DEP_INC_DIR dependencies/include)
set(DEP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib)

set(ZAKI_LIB ${DEP_LIB_DIR}/Zaki/${CMAKE_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR}/libZaki.a)


# set(ROOT_LIBS
#   ${ROOT_LIB_DIR}/libCore.so
#   ${ROOT_LIB_DIR}/libGpad.so
#   ${ROOT_LIB_DIR}/libGraf.so
#   ${ROOT_LIB_DIR}/libHist.so
# )

# set(ROOT_LIBS
#     ROOT::Core
#     ROOT::Gpad
#     ROOT::Graf
#     ROOT::Hist
# )

# set(ZAKI_LIB ${ZAKI_LIB_DIR}/libZaki.a)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(ConfindConfig.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Confind/ConfindConfig.h)
# install(FILES ${PROJECT_BINARY_DIR}/ConfindConfig.h
#         DESTINATION include/Confind
# )

find_package(OpenMP REQUIRED)
IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

# set(OPENMP_LIB "/opt/local/lib/libomp.dylib")
# set(OPENMP_INC "/opt/local/include/libomp")

add_subdirectory(include)
add_subdirectory(src)

add_library(Confind STATIC ${Confind_SRC_Files})
set_target_properties(Confind PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(Confind 
                            PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/include
                            # ${OPENMP_INC}
                            ${ZLIB_INCLUDE_DIRS}
                            # ${ROOT_INCLUDE_DIRS}
                            ${DEP_INC_DIR}
                            )

target_link_libraries(Confind PUBLIC 
                        # ${ROOT_LIBS} 
                        ${ZAKI_LIB} 
                        # ${OPENMP_LIB}
                        GSL::gsl
                        GSL::gslcblas
                        ZLIB::ZLIB
                        OpenMP::OpenMP_CXX
                        Python3::Python # for matplotlib
                        Python3::Module # for matplotlib
                        Python3::NumPy) 

# target_include_directories( Zaki 
#                             INTERFACE
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Modules>
#                             $<INSTALL_INTERFACE:include>
#                             /opt/local/include/root6
# )
                            
install(TARGETS Confind DESTINATION lib)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/include"
        DESTINATION dependencies)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib" 
        DESTINATION dependencies)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" DESTINATION README)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION README)

# Option for generating the doxygen docs
option(GEN_DOCS "Generate documents using doxygen." OFF)

if(GEN_DOCS)
    execute_process (
        COMMAND
        doxygen ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doxygen/html" DESTINATION README)

add_subdirectory(Examples)

# # enable_testing()

# # enable dashboard scripting
# include(CTest)

# # does the application run
# add_test(NAME Runs COMMAND Tutorial 25)

# # does the usage message work?
# add_test(NAME Usage COMMAND Tutorial)
# set_tests_properties(Usage
#   PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number"
#   )

# # define a function to simplify adding tests
# function(do_test target arg result)
#   add_test(NAME Comp${arg} COMMAND ${target} ${arg})
#   set_tests_properties(Comp${arg}
#     PROPERTIES PASS_REGULAR_EXPRESSION ".*${result}"
#     )
# endfunction(do_test)

# # do a bunch of result based tests
# do_test(Tutorial 4 "2")
# do_test(Tutorial 9 "3")
# do_test(Tutorial 5 "2.236")
# do_test(Tutorial 7 "2.645")
# do_test(Tutorial 25 "5")
# do_test(Tutorial -25 "-1")
# do_test(Tutorial 0.0001 "0.01")

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${CONFIND_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CONFIND_VERSION_MINOR}")
include(CPack)


# include(CMakePackageConfigHelpers)
# # generate the config file that is includes the exports
# configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
#   "${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsConfig.cmake"
#   INSTALL_DESTINATION "lib/cmake/example"
#   NO_SET_AND_CHECK_MACRO
#   NO_CHECK_REQUIRED_COMPONENTS_MACRO
#   )
# # generate the version file for the config file
# write_basic_package_version_file(
#   "${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsConfigVersion.cmake"
#   VERSION "${Tutorial_VERSION_MAJOR}.${Tutorial_VERSION_MINOR}"
#   COMPATIBILITY AnyNewerVersion
# )

# # install the configuration file
# install(FILES
#   ${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsConfig.cmake
#   DESTINATION lib/cmake/MathFunctions
#   )

# export(EXPORT MathFunctionsTargets
# FILE "${CMAKE_CURRENT_BINARY_DIR}/MathFunctionsTargets.cmake"
# )