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

run_process_group_cleanup_test() {
    marker="watcher_process_group_cleanup_$$"
    command="./bin/watcher --timeout 1 /bin/sh -c '/bin/sh -c \"while :; do sleep 5; done\" $marker & wait'"

    output="$(sh -c "$command" 2>&1)"
    code="$?"

    if [ "$code" -ne 143 ]; then
        echo "FAIL: process group cleanup - expected code 143, got $code"
        echo "$output"
        failures=$((failures + 1))
        return
    fi

    if ! printf '%s\n' "$output" | grep -F "Terminated by signal: 15" >/dev/null 2>&1; then
        echo "FAIL: process group cleanup - missing output: Terminated by signal: 15"
        echo "$output"
        failures=$((failures + 1))
        return
    fi

    sleep 1

    if ps -eo args | grep -F "$marker" | grep -v grep >/dev/null 2>&1; then
        echo "FAIL: process group cleanup - child process still running"
        ps -eo pid,args | grep -F "$marker" | grep -v grep
        failures=$((failures + 1))
        return
    fi

    echo "PASS: process group cleanup"
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
run_test "short timeout termination" 143 "./bin/watcher -t 1 /bin/sleep 5" "Terminated by signal: 15"
run_test "timeout kills process group" 143 "./bin/watcher --timeout 1 /bin/sh -c 'sleep 5 & wait'" "Terminated by signal: 15"
run_test "short timeout kills process group" 143 "./bin/watcher -t 1 /bin/sh -c 'sleep 5 & wait'" "Terminated by signal: 15"
run_process_group_cleanup_test
run_test "missing timeout value" 1 "./bin/watcher --timeout" "Usage:"
run_test "missing short timeout value" 1 "./bin/watcher -t" "Usage:"
run_test "invalid timeout value" 1 "./bin/watcher --timeout abc /bin/true" "Usage:"
run_test "invalid short timeout value" 1 "./bin/watcher -t abc /bin/true" "Usage:"
run_test "zero timeout value" 1 "./bin/watcher --timeout 0 /bin/true" "Usage:"
run_test "zero short timeout value" 1 "./bin/watcher -t 0 /bin/true" "Usage:"

if [ "$failures" -ne 0 ]; then
    echo "$failures test(s) failed"
    exit 1
fi

echo "All CLI tests passed"
