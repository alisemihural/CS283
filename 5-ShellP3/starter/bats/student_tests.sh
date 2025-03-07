#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipeline: echo and tr" {
  run bash -c 'printf "echo hello world | tr a-z A-Z\nexit\n" | ./dsh'
  [ "$status" -eq 0 ]
  echo "$output" | grep -q "HELLO WORLD"
}

@test "Pipeline: echo, tr, and rev" {
  run bash -c 'printf "echo hello world | tr a-z A-Z | rev\nexit\n" | ./dsh'
  [ "$status" -eq 0 ]
  echo "$output" | grep -q "DLROW OLLEH"
}

@test "Builtin: rc returns last exit code" {
  run bash -c 'printf "false\nrc\nexit\n" | ./dsh'
  [ "$status" -eq 0 ]
  echo "$output" | grep -q "1"
}

@test "Builtin: cd with too many arguments" {
  run bash -c 'printf "cd dir1 dir2\nexit\n" | ./dsh'
  [ "$status" -eq 0 ]
  echo "$output" | grep -q "cd: too many arguments"
}

@test "Redirection Overwrite: >" {
  run bash -c 'printf "echo hello, class > out.txt\ncat out.txt\nexit\n" | ./dsh'
  [ "$status" -eq 0 ]
  echo "$output" | grep -q "hello, class"
}

@test "Redirection Append: >>" {
  run bash -c 'printf "echo hello, class > file.txt\necho this is line 2 >> file.txt\ncat file.txt\nexit\n" | ./dsh'
  [ "$status" -eq 0 ]
  echo "$output" | grep -q "hello, class"
  echo "$output" | grep -q "this is line 2"
}

@test "Redirection Input: <" {
  echo "input redirection works" > temp.txt
  run bash -c 'printf "cat < temp.txt\nexit\n" | ./dsh'
  [ "$status" -eq 0 ]
  echo "$output" | grep -q "input redirection works"
}
