#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
build_dir=$(mktemp -d "${TMPDIR:-/tmp}/zavepower-tests.XXXXXX")
trap 'rm -rf "$build_dir"' EXIT
${CXX:-c++} -std=c++17 -Wall -Wextra -Werror -I. \
  tests/confirmed_commands_test.cpp -o "$build_dir/commands"
"$build_dir/commands"
${CXX:-c++} -std=c++17 -Wall -Wextra -Wno-vla-cxx-extension -Itests/stubs -I. \
  tests/uart_commands_test.cpp components/balboa_spa/balboaspa.cpp \
  -o "$build_dir/uart"
"$build_dir/uart"
