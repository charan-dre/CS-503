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

    [ "$status" -eq 0 ]
}

@test "Background execution" {
    run "./dsh" <<EOF                
sleep 1 &
EOF

    sleep 0.5
    ps aux | grep "sleep" > /dev/null

    [ "$status" -eq 0 ]
}

@test "Exit command with argument" {
    run "./dsh" <<EOF                
exit 0
EOF

    [ "$status" -eq 0 ]
}

@test "RC command after valid command" {
    run "./dsh" <<EOF
ls
EOF

    [ "$status" -eq 0 ]

    run "./dsh" <<EOF
rc
EOF

    echo "Raw rc output: $output"
    echo "Exit status: $status"

    stripped_output=$(echo "$output" | sed -n 's/.*dsh2> *\([0-9]*\).*/\1/p')

    echo "Stripped rc output: $stripped_output"

    [ "$stripped_output" -eq 0 ]
}

@test "Command not found returns 2" {
    run "./dsh" <<EOF
not_exists
rc
EOF

    [[ "$output" =~ "2" ]]
}

@test "RC command after invalid command" {
    run "./dsh" <<EOF
invalid_command
rc
EOF

    [[ "$output" =~ "127" ]]
}

@test "RC command after successful command" {
    run "./dsh" <<EOF
ls
rc
EOF

    [[ "$output" =~ "0" ]]
}

@test "RC command after permission denied" {
    run "./dsh" <<EOF
.
rc
EOF

    [[ "$output" =~ "126" ]]
}
