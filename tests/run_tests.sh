#!/usr/bin/env bash
# Compiles and runs every tests/examples/*.acs with the acs compiler and
# compares its output with the matching .expected file.
#
# If NAME.input exists it is piped to the program's stdin (for scan()).
#
# Usage: tests/run_tests.sh [--update] [path/to/acs]
#   --update   overwrite the .expected files with the current output
set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXAMPLES="$ROOT/tests/examples"

UPDATE=0
if [ "${1:-}" = "--update" ]; then
    UPDATE=1
    shift
fi
ACS="$(cd "$(dirname "${1:-$ROOT/acs}")" && pwd)/$(basename "${1:-$ROOT/acs}")"

if [ ! -x "$ACS" ]; then
    echo "error: compiler not found at $ACS (run 'make' first)" >&2
    exit 1
fi

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

pass=0
fail=0
failed=()

for src in "$EXAMPLES"/*.acs; do
    name="$(basename "$src" .acs)"
    expected="$EXAMPLES/$name.expected"
    input="$EXAMPLES/$name.input"

    # acs writes its generated C++ and binary to ../build relative to the
    # working directory, so give every test a fresh run/ + build/ pair.
    case_dir="$WORK/$name"
    mkdir -p "$case_dir/run" "$case_dir/build"
    cp "$src" "$case_dir/run/"

    if [ -f "$input" ]; then
        stdin="$input"
    else
        stdin=/dev/null
    fi
    actual="$case_dir/actual"
    (cd "$case_dir/run" && "$ACS" "$name.acs" <"$stdin" >"$actual" 2>&1)

    if [ "$UPDATE" -eq 1 ]; then
        cp "$actual" "$expected"
        echo "updated $name"
        continue
    fi

    if [ ! -f "$expected" ]; then
        echo "FAIL $name (missing $name.expected)"
        fail=$((fail + 1))
        failed+=("$name")
    elif diff -u --label "$name.expected" --label "$name (actual)" "$expected" "$actual"; then
        echo "PASS $name"
        pass=$((pass + 1))
    else
        echo "FAIL $name"
        fail=$((fail + 1))
        failed+=("$name")
    fi
done

[ "$UPDATE" -eq 1 ] && exit 0

echo
echo "$pass passed, $fail failed"
if [ "$fail" -ne 0 ]; then
    printf '  %s\n' "${failed[@]}"
    exit 1
fi
