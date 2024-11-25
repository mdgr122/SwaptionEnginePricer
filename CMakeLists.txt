# CMakeList.txt : CMake project for SwaptionEnginePricer, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("SwaptionEnginePricer")

# Add source to this project's executable.
add_executable(SwaptionEnginePricer "SwaptionEnginePricer.cpp" "SwaptionEnginePricer.h" "YieldCurveFuncs.h" "YieldCurveFuncs.cpp" "SwaptionPricer.h" "SwaptionPricer.cpp")

# Set C++ standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SwaptionEnginePricer PROPERTY CXX_STANDARD 20)
endif()

# ----------------------------------------
# Boost Configuration
# ----------------------------------------

# Set Boost root and library directory before calling find_package
set(BOOST_ROOT "C:/local/boost_1_86_0")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib") # Ensure this points to the 32-bit libs

# Inform CMake where to look for packages
list(APPEND CMAKE_PREFIX_PATH "${BOOST_ROOT}" "${BOOST_LIBRARYDIR}")

# Boost linking preferences
set(Boost_USE_STATIC_LIBS ON)      # Use static libraries
set(Boost_USE_MULTITHREADED ON)    # Enable multithreading
set(Boost_USE_STATIC_RUNTIME ON)   # Link the runtime library statically

# Find Boost libraries
find_package(Boost 1.86.0 REQUIRED COMPONENTS filesystem system)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

    # Include Boost headers
    include_directories(${Boost_INCLUDE_DIRS})

    # Link Boost libraries using imported targets if available
    if(TARGET Boost::filesystem AND TARGET Boost::system)
        target_link_libraries(SwaptionEnginePricer PRIVATE Boost::filesystem Boost::system)
    else()
        # Fallback to linking libraries directly
        target_link_libraries(SwaptionEnginePricer PRIVATE ${Boost_LIBRARIES})
    endif()
else()
    message(FATAL_ERROR "Boost not found!")
endif()

# ----------------------------------------
# QuantLib Configuration
# ----------------------------------------

# Define QuantLib as an imported library
add_library(quantlib STATIC IMPORTED)

# Set the location of the QuantLib library file
set_target_properties(quantlib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libs/QuantLib-1.36/lib/QuantLib-mt-gd.lib" # Adjust the path if necessary
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/libs/QuantLib-1.36/"
)

# Check if the QuantLib library file exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/QuantLib-1.36/lib/QuantLib-mt-gd.lib")
    message(FATAL_ERROR "QuantLib library not found at ${CMAKE_CURRENT_SOURCE_DIR}/libs/QuantLib-1.36/lib/QuantLib-mt-gd.lib")
endif()

# Link QuantLib to your executable
target_link_libraries(SwaptionEnginePricer PRIVATE quantlib)

# If QuantLib depends on Boost, link Boost libraries to QuantLib as well
# This ensures that any dependencies are correctly resolved
# However, since Boost is already linked to SwaptionEnginePricer, this might be redundant
# Uncomment if necessary
# target_link_libraries(quantlib INTERFACE Boost::filesystem Boost::system)
