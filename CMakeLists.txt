cmake_minimum_required(VERSION 3.28...4.4)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)
project(${_name} VERSION ${_version} LANGUAGES C CXX)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)
find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${CMAKE_PROJECT_NAME} BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/compat/libobs")
target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE
    src/plugin-main.cpp
    src/filter-settings.cpp
    src/gaussian-blur.cpp
    src/obs-version-policy.cpp
    src/frosted-glass-filter.cpp
    src/frosted-glass-filter.hpp
    src/filter-settings.hpp
    src/gaussian-blur.hpp
    src/obs-version-policy.hpp
)
set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})

include(CTest)
if(BUILD_TESTING)
  add_executable(filter-settings-test tests/filter-settings-test.cpp src/filter-settings.cpp)
  target_include_directories(filter-settings-test PRIVATE src)
  target_compile_features(filter-settings-test PRIVATE cxx_std_17)
  add_test(NAME filter-settings COMMAND filter-settings-test)
  add_executable(shader-contract-test tests/shader-contract-test.cpp)
  target_compile_features(shader-contract-test PRIVATE cxx_std_17)
  target_compile_definitions(shader-contract-test PRIVATE EFFECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/effects")
  add_test(NAME shader-contract COMMAND shader-contract-test)
  add_executable(obs-version-policy-test tests/obs-version-policy-test.cpp src/obs-version-policy.cpp)
  target_include_directories(obs-version-policy-test PRIVATE src)
  target_compile_features(obs-version-policy-test PRIVATE cxx_std_17)
  add_test(NAME obs-version-policy COMMAND obs-version-policy-test)
  add_executable(shader-runtime-test tests/shader-runtime-test.cpp)
  target_link_libraries(shader-runtime-test PRIVATE OBS::libobs)
  target_compile_features(shader-runtime-test PRIVATE cxx_std_17)
  target_include_directories(shader-runtime-test BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/compat/libobs")
endif()
