#!/bin/bash
# Generate TAGS for LLVM project

cd "$(dirname "$0")"

echo "Generating TAGS file..."

find . -type f \( \
     -name "*.cpp" -o \
     -name "*.h" -o \
     -name "*.c" -o \
     -name "*.hpp" -o \
     -name "*.cc" \
     \) \
     -not -path "*/build/*" \
     -not -path "*/.git/*" \
     -not -path "*/out/*" \
    | xargs etags

echo "TAGS file generated successfully"
