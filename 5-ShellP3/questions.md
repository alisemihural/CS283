1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation stores each child process’s PID after calling fork() and then calls waitpid() in a loop for each child PID. This ensures the shell waits until every command in the pipeline completes before continuing to accept new input. If I forgot to call waitpid() on all child processes, zombie processes would accumulate, and it could exhaust system resources and lead to strange shell behavior.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

The dup2() function duplicates one file descriptor onto another, commonly used to redirect standard input/output streams for commands. It’s crucial to close the original pipe descriptors after using dup2() because leaving them open results in descriptor leaks, reducing the number of file descriptors available and potentially causing the shell or system to run out of available resources.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command changes the current working directory of the shell itself. If implemented as an external command, it would only change the directory of a separate child process, not affecting the shell’s directory. Thus, the shell’s state would remain unchanged, making the external implementation ineffective and confusing to users.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To support an arbitrary number of piped commands, I’d dynamically allocate and resize an array or linked list for commands instead of using a fixed-size array (CMD_MAX). This approach would use dynamic memory allocation (malloc() and realloc()) to grow the list as needed. The trade-off involves additional complexity for memory management and a small performance overhead due to dynamic allocations.
