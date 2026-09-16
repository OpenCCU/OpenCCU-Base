#!/usr/bin/env bash

set -o errexit
set -o nounset
set -o pipefail

repository=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)
stripper="${repository}/build-tools/bidcos-devicetype-strip"
fixtures="${repository}/tests/build-tools/bidcos-devicetype-strip"
temporary=$(mktemp -d)
trap 'rm -rf "$temporary"' EXIT

"$stripper" "$fixtures/input.xml" -o "$temporary/output.xml"
head -c -1 "$fixtures/expected.xml" > "$temporary/expected.xml"
cmp "$temporary/expected.xml" "$temporary/output.xml"

printf "<device><docu>unfinished</device>\n" > "$temporary/malformed.xml"
if "$stripper" "$temporary/malformed.xml" -o "$temporary/malformed-output.xml"; then
  echo "malformed input unexpectedly succeeded" >&2
  exit 1
fi
[[ ! -e "$temporary/malformed-output.xml" ]]

if "$stripper" "$fixtures/input.xml" -o "$temporary"; then
  echo "writing to a directory unexpectedly succeeded" >&2
  exit 1
fi
