cmake_minimum_required(VERSION 3.10)
project(examples)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

macro (add_llvm_example toy)
	# Now build our tools
	add_executable(${toy} ${toy}.cpp)

	# Find the libraries that correspond to the LLVM components
	# that we wish to use
	llvm_map_components_to_libnames(llvm_libs core mcjit native)

	# Link against LLVM libraries
	target_link_libraries(${toy} ${llvm_libs})
endmacro (add_llvm_example)

add_subdirectory(Fibonacci)
