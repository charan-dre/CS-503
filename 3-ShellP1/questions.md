1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is ideal for this application because it safely reads user input, including spaces, and prevents buffer overflow by limiting the input size. It handles end-of-file (EOF) correctly, returning NULL when input ends, and ensures that the entire line is captured. This function is also easy to use with string manipulation functions like strcspn() to remove trailing newlines, making it a secure and efficient choice for reading commands in a shell program.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() for cmd_buff allows dynamic memory allocation, so the program can handle commands of varying lengths without wasting memory or being limited by a fixed size. Unlike a fixed-size array, malloc() provides flexibility to adjust the buffer size as needed, ensuring efficient memory usage for unpredictable input sizes.

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces ensures the shell correctly identifies the command and its arguments. Without trimming, extra spaces could cause misinterpretation, resulting in errors or incorrect command execution.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 
    Redirecting Standard Output to a File: Redirecting the output of a command to a file allows you to save the command's output for later use. This command lists directory contents and saves the output to file_list.txt. 
    Challenge: Implementing this requires opening the specified file for writing and ensuring that existing content is overwritten or appended based on the user's intent.

    Redirecting Standard Error to a File: Capturing error messages in a separate file helps in debugging and logging. This command attempts to list a non-existent directory and redirects the error message to error_log.txt.
    Challenge: Properly handling file descriptors to capture only error messages without affecting standard output requires careful management of file streams.

    Redirecting Both Standard Output and Standard Error to the Same File: Combining both outputs into a single file is useful for comprehensive logging. This command redirects both standard output and standard error to all_output.txt.
    Challenge: Understanding the order of redirection is crucial. The 2>&1 syntax first redirects standard error to standard output, and then standard output is redirected to the file. Misordering can lead to unintended results.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:
    Purpose: 
    Redirection is used to change the destination or source of input/output for a command. It involves sending the output of a command to a file or receiving input from a file, instead of the default streams (stdout or stdin).
    
    Piping is used to connect the output of one command directly to the input of another command, allowing the output to "flow" from one command into another.

    How They Work:
    Redirection involves using symbols like > (output redirection), < (input redirection), and 2> (error redirection) to either send output to a file or receive input from a file.
    Example: ls > file_list.txt sends the output of ls to a file.
    
    Piping uses the pipe symbol | to pass the output of one command as input to another. It doesn't deal with files directly.
    Example: ls | grep "txt" passes the output of ls to grep for further filtering.

    Handling Multiple Commands:
    Redirection typically works with one command at a time, changing its input or output behavior.

    Piping connects multiple commands, allowing for complex chains of commands where each command operates on the output of the previous one.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:   Separating STDERR (error messages) from STDOUT (regular output) is important for better error handling, easier debugging, and flexible logging. It allows for distinct management of normal output and errors, such as redirecting them to different files or processing them separately in scripts. This separation improves control and clarity when handling shell outputs.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:   Our shell should capture STDOUT and STDERR separately, but allow merging them with the 2>&1 syntax if needed. This provides flexibility for unified output while still enabling separate handling for better error tracking.