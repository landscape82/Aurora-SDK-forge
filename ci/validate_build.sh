#!/bin/bash

set -euo pipefail

./ci/build_libs.sh
python ./ci/build_examples.py
