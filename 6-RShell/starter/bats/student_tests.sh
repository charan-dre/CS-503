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

@test "Exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"exiting"* ]]
}

@test "Command with append redirection" {
    run ./dsh <<EOF
echo line1 > test_append.txt
echo line2 >> test_append.txt
cat test_append.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"line1"* ]]
    [[ "$output" == *"line2"* ]]
    rm test_append.txt
}

@test "Simple pipe: ls | grep test" {
    run ./dsh <<EOF
touch test_file
ls | grep test
rm test_file
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"test_file"* ]]
}

@test "Multiple pipes: ls /usr/bin | grep zip | wc -l" {
    run ./dsh <<EOF
ls /usr/bin | grep zip | wc -l
EOF
    [ "$status" -eq 0 ]
    [ -n "$output" ]  # There should be some output (a number)
}

@test "Background process: sleep (if supported)" {
    run ./dsh <<EOF
sleep 2 &
echo "Immediate output"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Immediate output"* ]]
}

@test "Multiple commands on one line" {
    run ./dsh <<EOF
echo first; echo second
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"first"* ]]
    [[ "$output" == *"second"* ]]
}