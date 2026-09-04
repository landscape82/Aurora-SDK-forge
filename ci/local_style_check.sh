#!/bin/bash

set -euo pipefail

if ! command -v clang-format >/dev/null 2>&1; then
    echo "clang-format is required to run style checks."
    exit 1
fi

mapfile -t files < <(rg --files include Examples ci -g '*.[ch]' -g '*.cpp' -g '*.hpp')

if [ ${#files[@]} -eq 0 ]; then
    echo "No source files found for style validation."
    exit 0
fi

clang-format --dry-run --Werror "${files[@]}"
