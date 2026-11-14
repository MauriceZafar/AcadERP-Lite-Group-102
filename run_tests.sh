
#!/bin/bash
# group-102

cd "$(dirname "$0")" || exit 1

shopt -s nullglob

echo "Building AcadERP-Lite..."

make clean >/dev/null

if ! make; then
    echo "BUILD FAILED"
    exit 1
fi

echo ""
echo "Running test cases..."

passed=0
failed=0

inputs=(tests/*.in)

if [ ${#inputs[@]} -eq 0 ]; then
    echo "NO TEST CASES FOUND"
    exit 1
fi

for input in "${inputs[@]}"
do
    expected="${input%.in}.out"
    actual="/tmp/grp_102_actual.out"

    if [ ! -f "$expected" ]; then
        echo "MISSING EXPECTED OUTPUT: $expected"
        failed=$((failed + 1))
        continue
    fi

    if ! ./acaderp < "$input" > "$actual"; then
        echo "RUNTIME ERROR: $input"
        failed=$((failed + 1))
        continue
    fi

    if diff -u "$expected" "$actual" >/dev/null; then
        echo "PASS: $input"
        passed=$((passed + 1))
    else
        echo "FAIL: $input"
        diff -u "$expected" "$actual"
        failed=$((failed + 1))
    fi
done

echo ""
echo "=============================="
echo "Passed: $passed"
echo "Failed: $failed"
echo "=============================="

if [ "$failed" -ne 0 ]; then
    exit 1
fi

exit 0
