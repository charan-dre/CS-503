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

@test "Pipeline execution: ls | grep .txt" {
    run ./dsh <<EOF
ls | grep .txt
EOF
    [ "$status" -eq 0 ]
}

@test "Directory change: cd" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/tmp" ]]
}

@test "Exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "exiting..." ]]
}

@test "Multiple pipes: ls | grep . | wc -l" {
    run ./dsh <<EOF
ls | grep . | wc -l
EOF
    [ "$status" -eq 0 ]
}

@test "Redirection with pipe: ls | grep . > output.txt" {
    run ./dsh <<EOF
ls | grep . > output.txt
cat output.txt
EOF
    [ "$status" -eq 0 ]
    [ -f "output.txt" ]
    rm output.txt
}

@test "Empty input handling" {
    run ./dsh <<EOF

EOF
    [ "$status" -eq 0 ]
}

@test "Command not found handling" {
    run ./dsh <<EOF
nonexistentcommand
EOF
    [ "$status" -eq 0 ]
}