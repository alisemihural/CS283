#!/usr/bin/env bats

setup() {
  # Create a temporary directory for cd testing.
  mkdir -p testdir
}

teardown() {
  rm -rf testdir
}

@test "echo command prints correctly" {
  run bash -c './dsh <<< "echo hello\nexit\n"'
  [ "$status" -eq 0 ]
  [[ "$output" == *"hello"* ]]
}

@test "cd command changes directory" {
  # Run pwd, change directory to testdir, and run pwd again.
  run bash -c './dsh <<< "pwd\ncd testdir\npwd\nexit\n"'
  [ "$status" -eq 0 ]
  # Extract lines that look like absolute paths.
  result=$(echo "$output" | grep -E "^/")
  lines=($result)
  [ ${#lines[@]} -ge 2 ]
  # The second pwd output should contain "testdir".
  [[ "${lines[1]}" == *"testdir"* ]]
}

@test "cd with no arguments does not change directory" {
  run bash -c './dsh <<< "pwd\ncd\npwd\nexit\n"'
  [ "$status" -eq 0 ]
  # Capture the two pwd outputs.
  result=$(echo "$output" | grep -E "^/")
  lines=($result)
  [ ${#lines[@]} -ge 2 ]
  # They should be identical.
  [ "${lines[0]}" = "${lines[1]}" ]
}

@test "cd with too many arguments prints error" {
  run bash -c './dsh <<< "cd arg1 arg2\nexit\n"'
  [ "$status" -eq 0 ]
  [[ "$output" == *"cd: too many arguments"* ]]
}

@test "rc command returns exit code for a failed command" {
  run bash -c './dsh <<< "nonexistent_cmd\nrc\nexit\n"'
  [ "$status" -eq 0 ]
  # Expect an error message and rc output "2" (assuming ENOENT equals 2).
  [[ "$output" == *"Command not found in PATH"* ]]
  [[ "$output" == *"2"* ]]
}

@test "rc command returns exit code for a successful command" {
  run bash -c './dsh <<< "echo success\nrc\nexit\n"'
  [ "$status" -eq 0 ]
  [[ "$output" == *"success"* ]]
  [[ "$output" == *"0"* ]]
}

@test "quoted string parsing preserves inner spaces" {
  run bash -c './dsh <<< "echo \"hello,   world\"\nexit\n"'
  [ "$status" -eq 0 ]
  [[ "$output" == *"hello,   world"* ]]
}

# @test "dragon command prints ASCII art" {
#   run bash -c './dsh <<< "dragon\nexit\n"'
#   [ "$status" -eq 0 ]
#   [[ "$output" == *"DREXEL"* ]]  # Assuming the ASCII art contains "DREXEL"
# }