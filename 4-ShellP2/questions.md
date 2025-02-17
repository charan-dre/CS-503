1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  fork is used to create a new process, and execvp is used to replace the child process with a different program. The value of using fork is that it allows the parent process to remain unaffected, provides process control, and enables concurrent execution.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it returns -1, indicating an error due to resource limitations or memory issues. You should check this return value and, if negative, print an error with perror() and exit gracefully using exit(1). If the return value is 0, you're in the child process, and if positive, you're in the parent process. Proper handling ensures the program remains stable even with system resource issues.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() finds the command by searching through directories listed in the PATH environment variable. If the command isn’t provided with a path, it checks each directory in PATH for a matching executable. If no match is found, it returns an error.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Calling wait() in the parent process ensures it waits for the child to finish, collects the child's exit status, and prevents the child from becoming a zombie process. Without wait(), the parent might continue without knowing the child's completion status, and the child’s resources wouldn’t be cleaned up.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() retrieves the child process's exit status, indicating whether it terminated successfully or with an error. It's important for the parent to know the child's termination result and respond accordingly.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  build_cmd_buff() handles quoted arguments to treat spaces within quotes as part of a single argument. This is necessary to avoid splitting arguments incorrectly when they contain spaces, ensuring accurate command parsing.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I updated the parsing logic to properly handle quoted arguments and special characters, which weren't addressed in the previous assignment. A challenge was managing edge cases like nested quotes and escape characters while maintaining efficiency.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  In a Linux system, signals are used to notify processes of events, like termination or interruption. Unlike other IPC methods, signals are lightweight and asynchronous, focusing on control actions rather than data exchange.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:

    SIGKILL: This signal immediately terminates a process without allowing it to handle the termination. It's used when a process needs to be forcefully stopped and cannot ignore or catch the signal. It’s typically used to kill unresponsive or stuck processes.

    SIGTERM: This signal requests a process to terminate gracefully, allowing it to clean up resources before exiting. It can be caught and handled by the process, making it useful for orderly shutdowns, such as when stopping a server or application.

    SIGINT: Triggered by the user pressing Ctrl+C in the terminal, SIGINT interrupts a process and asks it to terminate. It’s often used to stop a running command or process in the terminal in an interactive session.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is paused and cannot be caught, blocked, or ignored, unlike SIGINT. This ensures the process is reliably suspended until it receives SIGCONT to resume.
