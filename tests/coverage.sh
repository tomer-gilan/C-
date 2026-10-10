#!/bin/sh
# Written by Claude on 10-Oct-26.
#
# Coverage report for the compiler's own sources (testing-roadmap.md, Step 9): builds with -DENABLE_COVERAGE=ON,
# runs the whole suite, then lists the lines that never ran and the branches that were never taken.
#
# Usage: tests/coverage.sh [--fail-on-uncovered-lines] [source-file ...]
#          source-file: relative to the project root (default: Lexer.c)
#          --fail-on-uncovered-lines: exit 1 if any line of those files never ran (CTest uses this; never-taken
#          branches are only reported, since some can't be reached, e.g. defensive checks)
# Env:   BUILD_DIR   the build directory (default: cmake-build-coverage, which .gitignore already covers)
#
# Needs gcc, cmake and gcov. Run it where the tests run (the WSL/Ubuntu side).

set -eu

root=$(cd "$(dirname "$0")/.." && pwd)
build_dir=${BUILD_DIR:-"$root/cmake-build-coverage"}
fail_on_uncovered_lines=false
if [ "${1:-}" = "--fail-on-uncovered-lines" ]; then
    fail_on_uncovered_lines=true
    shift
fi
[ "$#" -gt 0 ] || set -- Lexer.c

cmake -S "$root" -B "$build_dir" -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON -DENABLE_SANITIZERS=OFF > /dev/null
# Counters add up across runs; delete the old ones so the report describes this run only.
find "$build_dir" -name '*.gcda' -delete
cmake --build "$build_dir" > /dev/null
(cd "$build_dir" && ctest --output-on-failure)

report_dir="$build_dir/coverage"
mkdir -p "$report_dir"
cd "$report_dir"
status=0
for source in "$@"; do
    # -o <object file>: CMake names the objects Lexer.c.o, so gcov has to be told where the .gcno/.gcda are.
    # -b: branches. -c: counts instead of percentages, so "taken 0" is exact.
    gcov -b -c -o "$build_dir/CMakeFiles/c_compiler_core.dir/$source.o" "$root/$source" > "$source.summary"
    echo
    echo "==== $source ===="
    grep -A3 "^File '.*/$source'" "$source.summary" | grep -v '^--'
    # assert(...) always has a failure branch that a correct run never takes, so those lines are skipped.
    report=$(awk -v file="$source" '
        /^[ 0-9#=*-]+: *[0-9]+:/ {
            split($0, parts, ":")
            line = parts[2] + 0
            text = substr($0, length(parts[1]) + length(parts[2]) + 3)
            gsub(/^ +/, "", text)
            if (line > 0 && parts[1] ~ /#####/)
                printf "%s:%d: line never run:      %s\n", file, line, text
            # the branch lines that follow belong to this line; none for a line that never ran or has no code
            current = (line > 0 && parts[1] !~ /[#=]/ && parts[1] !~ /-/) ? line : 0
            next
        }
        /^branch/ && current > 0 {
            total[current]++
            if ($0 ~ /never executed/ || $0 ~ / taken 0$/)
                missed[current]++
            source_text[current] = text
        }
        END {
            for (n in missed)
                if (missed[n] > 0 && source_text[n] !~ /assert\(/)
                    printf "%s:%d: branch never taken:  %d of %d  %s\n", file, n, missed[n], total[n], source_text[n]
        }' "$source.gcov" | sort -t: -k2 -n)
    [ -z "$report" ] || echo "$report"
    if $fail_on_uncovered_lines && echo "$report" | grep -q 'line never run'; then
        echo "FAIL: $source has lines that no test runs (the 'line never run' lines above): add a test for them."
        status=1
    fi
done
exit "$status"
