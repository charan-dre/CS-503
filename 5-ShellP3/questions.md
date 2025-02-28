1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_answer here_ My shell uses waitpid() to wait for each child process to finish before accepting new commands. Without waitpid(), child processes would become dead but still taking up system resources. The parent shell must wait for all children to properly clean up these resources.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_answer here_ Closing unused pipe ends is crucial because:

                Each pipe end uses a file descriptor
                Open pipes prevent programs from seeing end-of-file
                Too many open file descriptors can cause resource leaks
                Programs might hang waiting for data from pipes that will never close

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_answer here_ cd must be built-in because:
                It needs to change the shell's own working directory
                If cd was external, it would change only its own directory and exit
                The shell would stay in the original directory
                This would make cd useless for navigation

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_answer here_ To handle unlimited pipes, I would:
                Use a dynamic array instead of fixed size
                Start with a small initial size
                Double the array size when needed
                Free memory when commands complete Trade-off: More memory management overhead vs flexibility in number of pipes
