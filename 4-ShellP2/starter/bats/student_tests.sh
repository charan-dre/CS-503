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

@test "exit command" {
    run "./dsh" <<EOF                
exit
EOF

    # Check that the exit status is 0, as the shell should exit cleanly
    [ "$status" -eq 0 ]
}

@test "Background execution" {
    run "./dsh" <<EOF                
sleep 1 &
EOF

    # Check that the background process is running
    sleep 0.5
    ps aux | grep "sleep" > /dev/null

    # Check exit status
    [ "$status" -eq 0 ]
}

@test "Exit command with argument" {
    run "./dsh" <<EOF                
exit 0
EOF

    # The exit status should be 0 for successful execution
    [ "$status" -eq 0 ]
}

@test "RC command after valid command" {
    # Run the valid command (ls)
    run "./dsh" <<EOF
ls
EOF

    # Ensure that the status for the valid command is 0 (success)
    [ "$status" -eq 0 ]

    # Now, capture the output from the rc command to check the return code
    run "./dsh" <<EOF
rc
EOF

    # Output the raw output for debugging
    echo "Raw rc output: $output"
    echo "Exit status: $status"

    # Strip everything except for the return code
    stripped_output=$(echo "$output" | sed -n 's/.*dsh2> *\([0-9]*\).*/\1/p')

    # Output the stripped result to debug what we're checking
    echo "Stripped rc output: $stripped_output"

    # Now check if the output is '0' as expected (return code of ls)
    [ "$stripped_output" -eq 0 ]
}

@test "Command not found returns 2" {
    run "./dsh" <<EOF
not_exists
rc
EOF

    # Ensure that the last return code is 2 (command not found)
    [[ "$output" =~ "2" ]]
}

@test "RC command after invalid command" {
    run "./dsh" <<EOF
invalid_command
rc
EOF

    # Ensure that the last return code is 127 (command not found)
    [[ "$output" =~ "127" ]]
}

@test "RC command after successful command" {
    run "./dsh" <<EOF
ls
rc
EOF

    # Ensure that the last return code is 0 (success)
    [[ "$output" =~ "0" ]]
}

@test "RC command after permission denied" {
    run "./dsh" <<EOF
.
rc
EOF

    # Ensure that the last return code is 126 (permission denied)
    [[ "$output" =~ "126" ]]
}
