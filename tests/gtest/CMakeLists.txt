cmake_minimum_required(VERSION 3.9)

# Set policies for Google Test compatibility
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

# Disable Google Test installation and examples
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Silence TR1 deprecation warnings for Google Test
add_compile_definitions(_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)

# Only build Google Mock (it will include Google Test automatically)
add_subdirectory(gtest/googletest/googlemock EXCLUDE_FROM_ALL)

# Add the actual tests
add_subdirectory(tests)