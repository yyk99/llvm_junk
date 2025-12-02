Flex Location Tracking Example

This example demonstrates C++ Flex scanner with location tracking for error reporting.

Key Features:
- Location tracking using location_t struct (line and column numbers)
- YY_USER_ACTION macro for automatic location updates
- Type-safe semantic values using std::variant<std::string, int, double>
- Error reporting with precise location information
- Support for integers, floating-point numbers, and identifiers

Improvements over basic flex example:
- Uses std::variant instead of struct for type-safe semantic values
- Tracks both line and column positions for each token
- Provides detailed error messages with location context
- Supports floating-point number literals (FNUMBER)

Building:
  cd build
  cmake ..
  make

Running:
  ./scanner < test1.txt
  echo "begin x := 123.45; end" | ./scanner

The scanner will output tokens and report errors with line:column information
for any invalid characters encountered.
