#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

# Test basic command execution
@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# Test built-in command: cd
@test "Builtin: cd without arguments" {
    run ./dsh <<EOF
cd
pwd
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "$HOME"
}

@test "Builtin: cd with valid directory" {
    run ./dsh <<EOF
cd /tmp
pwd
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "/tmp"
}

@test "Builtin: cd with invalid directory" {
    run ./dsh <<EOF
cd /invalid_directory
exit
EOF
    echo "DEBUG: Output was: $output"
    echo "DEBUG: Status was: $status"
    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -E "cd error: No such file or directory" | head -n 1
}

# Test remote command execution
@test "Remote: check ls runs without errors" {
    run ./dsh -c <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# Test remote built-in command: cd
@test "Remote Builtin: cd without arguments" {
    run ./dsh -c <<EOF
cd
pwd
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "$HOME"
}

@test "Remote Builtin: cd with valid directory" {
    run ./dsh -c <<EOF
cd /tmp
pwd
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "/tmp"
}

@test "Remote Builtin: cd with invalid directory" {
    run ./dsh -c <<EOF
cd /invalid_directory
exit
EOF

    echo "DEBUG: Output was: $output"
    echo "DEBUG: Status was: $status"
    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "cd: No such file or directory"
}

# Test remote command with arguments
@test "Remote: echo with arguments" {
    run ./dsh -c <<EOF
echo Hello, World!
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "Hello, World!"
}

# Test remote command with pipes
@test "Remote: pipeline echo and tr" {
    run ./dsh -c <<EOF
echo hello world | tr a-z A-Z
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "HELLO WORLD"
}

# Test remote command with redirection
@test "Remote: redirection overwrite" {
    run ./dsh -c <<EOF
echo hello, class > out.txt
cat out.txt
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "hello, class"
}

@test "Remote: redirection append" {
    run ./dsh -c <<EOF
echo hello, class > file.txt
echo this is line 2 >> file.txt
cat file.txt
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "hello, class"
    echo "$output" | grep -q "this is line 2"
}

# Test remote built-in command: exit
@test "Remote Builtin: exit command" {
    run ./dsh -c <<EOF
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "client exited: getting next connection..."
}

# Test remote built-in command: stop-server
@test "Remote Builtin: stop-server command" {
    run ./dsh -c <<EOF
stop-server
EOF

    # Assertions
    [ "$status" -eq 0 ]
    echo "$output" | grep -q "client requested server to stop, stopping..."
}
