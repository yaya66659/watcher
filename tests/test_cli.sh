#!/bin/sh

set -u

failures=0

run_test() {
    name="$1"
    expected_code="$2"
    command="$3"
    expected_output="$4"

    output="$(sh -c "$command" 2>&1)"
    code="$?"

    if [ "$code" -ne "$expected_code" ]; then
        echo "FAIL: $name - expected code $expected_code, got $code"
        echo "$output"
        failures=$((failures + 1))
        return
    fi

    if ! printf '%s\n' "$output" | grep -F "$expected_output" >/dev/null 2>&1; then
        echo "FAIL: $name - missing output: $expected_output"
        echo "$output"
        failures=$((failures + 1))
        return
    fi

    echo "PASS: $name"
}

run_test "no argument" 1 "./bin/watcher" "Usage:"
run_test "help" 0 "./bin/watcher -h" "Watcher - User Manual"
run_test "french help" 0 "./bin/watcher -hfr" "Watcher - Manuel d'utilisation"
run_test "success command" 0 "./bin/watcher /bin/true" "Child exited with status 0"
run_test "failing command" 1 "./bin/watcher /bin/false" "Child exited with status 1"
run_test "specific exit code" 42 "./bin/watcher /bin/sh -c 'exit 42'" "Child exited with status 42"
run_test "missing command" 127 "./bin/watcher /no/such/command" "execve: No such file or directory"
run_test "signal termination" 143 "./bin/watcher /bin/sh -c 'kill -TERM \$\$'" "Terminated by signal: 15"
run_test "argument forwarding" 0 "./bin/watcher /bin/echo hello watcher test" "hello watcher test"
run_test "command without path" 127 "./bin/watcher true" "execve: No such file or directory"
run_test "timeout termination" 143 "./bin/watcher --timeout 1 /bin/sleep 5" "Terminated by signal: 15"
run_test "timeout kills process group" 143 "./bin/watcher --timeout 1 /bin/sh -c 'sleep 5 & wait'" "Terminated by signal: 15"
run_test "missing timeout value" 1 "./bin/watcher --timeout" "Usage:"
run_test "invalid timeout value" 1 "./bin/watcher --timeout abc /bin/true" "Usage:"
run_test "zero timeout value" 1 "./bin/watcher --timeout 0 /bin/true" "Usage:"

if [ "$failures" -ne 0 ]; then
    echo "$failures test(s) failed"
    exit 1
fi

echo "All CLI tests passed"
