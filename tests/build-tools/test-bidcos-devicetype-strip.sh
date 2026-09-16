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
[[ $(stat -c %a "$temporary/output.xml") == 644 ]]

"$stripper" "$fixtures/input.xml" -ccu2 -o "$temporary/ccu2.xml"
head -c -1 "$fixtures/expected-ccu2.xml" > "$temporary/expected-ccu2.xml"
cmp "$temporary/expected-ccu2.xml" "$temporary/ccu2.xml"

for malformed in \
  "<device><docu>unfinished</device>" \
  "<device broken></device>" \
  "<device/><device/>" \
  "<device value=\"&broken;\"/>"; do
  printf "%s\n" "$malformed" > "$temporary/malformed.xml"
  if "$stripper" "$temporary/malformed.xml" -o "$temporary/malformed-output.xml"; then
    echo "malformed input unexpectedly succeeded: $malformed" >&2
    exit 1
  fi
  [[ ! -e "$temporary/malformed-output.xml" ]]
done

if "$stripper" "$fixtures/input.xml" -o "$temporary"; then
  echo "writing to a directory unexpectedly succeeded" >&2
  exit 1
fi
