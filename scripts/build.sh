#!/bin/bash

# Default values
BUILD_TYPE="Debug"
TARGET="stm32f103rb"

# Valid values
VALID_BUILD_TYPES=("Release" "Debug" "RelWithDebInfo" "MinSizeRel")
VALID_TARGETS=("stm32f103rb")

# Print help
function help() {
   echo "Usage: $0 [-b <build_type>] [-t <target>]"
   echo "  -b, --build     Build type (Release, Debug, RelWithDebInfo, MinSizeRel). Default: Release."
   echo "  -t, --target    Target (stm32f103rb). Default: stm32f103rb."
   exit 1
}

# Validate value in a list
function is_valid() {
    local value="${1,,}"  # Convert to lowercase
    shift
    local valid_values=("$@")
    for valid in "${valid_values[@]}"; do
        if [[ "$value" == "${valid,,}" ]]; then
            echo "$valid"  # Return the correctly capitalized version
            return 0
        fi
    done
    return 1
}

# Parse arguments
while [[ $# -gt 0 ]]; do
   case "${1,,}" in
      -h|--help)
         help
         ;;
      -b|--build)
         if [[ -z "$2" ]]; then
            echo "Error: Missing value for option '$1'."
            help
         fi
         input="${2,,}"  # Convert input to lowercase
         BUILD_TYPE=$(is_valid "$input" "${VALID_BUILD_TYPES[@]}")
         if [[ -z "$BUILD_TYPE" ]]; then
            echo "Error: Invalid build type '$2'. Valid types are: ${VALID_BUILD_TYPES[*]}"
            exit 1
         fi
         shift 2
         ;;
      -t|--target)
         if [[ -z "$2" ]]; then
            echo "Error: Missing value for option '$1'."
            help
         fi
         input="${2,,}"  # Convert input to lowercase
         TARGET=$(is_valid "$input" "${VALID_TARGETS[@]}")
         if [[ -z "$TARGET" ]]; then
            echo "Error: Invalid target '$2'. Valid targets are: ${VALID_TARGETS[*]}"
            exit 1
         fi
         shift 2
         ;;
      -*|--*)
         echo "Unknown option: $1"
         help
         ;;
      *)
         echo "Unknown argument: $1"
         help
         ;;
   esac
done

rm -Rf build/$BUILD_TYPE
mkdir -p build/$BUILD_TYPE

# Generate Makefiles
if cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=dev/cmake/$TARGET.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -S . -B build/$BUILD_TYPE ; then
   # Build CMake
   cmake --build build/$BUILD_TYPE --clean-first
fi
