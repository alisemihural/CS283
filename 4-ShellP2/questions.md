1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  The fork/execvp combination is used instead of calling execvp directly because fork() creates a new child process, allowing the parent process to continue executing while the child replaces itself with the new program using execvp(). This separation provides benefits including process control and isolation. If execvp() were called directly without fork(), the current process would be replaced entirely, losing the ability to manage child processes or handle errors if execvp() fails. By forking first, the parent can track the child’s execution, wait for its completion, and handle errors appropriately.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it typically indicates that the system has exhausted its available process table entries or memory, making it impossible to create a new child process. In this case, the function returns -1, and no child process is created. My implementation handles this scenario by checking the return value of fork() and printing an error message to stderr if the call fails. Additionally, the program may exit with a failure code to indicate the error, ensuring that the system does not continue running in an inconsistent state. Proper error handling is crucial because failing to detect and respond to a fork() failure could lead to unexpected behavior in a multi-process application.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  The execvp() function searches for the specified command by looking through the directories listed in the PATH environment variable. If the command is provided with a relative or absolute path, execvp() attempts to execute it directly. Otherwise, it iterates through each directory in PATH, checking for an executable file that matches the command name. If a match is found, the process image is replaced with the new program. If no matching executable is found, execvp() returns an error. The PATH variable is crucial because it allows users to execute programs without specifying their full path, making command execution more convenient and efficient.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() system call ensures that the parent process waits for its child process to complete before continuing execution. This prevents the creation of zombie processes, which occur when a child terminates but its exit status is not collected by the parent. If wait() is not called, the child process remains in a “zombie” state, occupying system resources until the parent terminates. Additionally, in a shell or process manager, failing to wait for child processes could lead to unpredictable behavior, such as process IDs being reused before the parent acknowledges completion. Using wait() or waitpid() allows the parent to properly handle child process termination and retrieve its exit status.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  The wait() system call ensures that the parent process waits for its child process to complete before continuing execution. This prevents the creation of zombie processes, which occur when a child terminates but its exit status is not collected by the parent. If wait() is not called, the child process remains in a “zombie” state, occupying system resources until the parent terminates. Additionally, in a shell or process manager, failing to wait for child processes could lead to unpredictable behavior, such as process IDs being reused before the parent acknowledges completion. Using wait() or waitpid() allows the parent to properly handle child process termination and retrieve its exit status.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() ensures that quoted arguments are treated as single entities by properly parsing and storing them. When a user enters a command with quoted arguments (e.g., echo "Hello World"), the function detects the presence of quotes and groups all characters inside them into a single argument. This is necessary because without handling quotes, spaces within arguments would be interpreted as argument separators, leading to incorrect parsing. Proper handling of quoted arguments ensures that commands function as expected, particularly when dealing with filenames or strings containing spaces.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  My implementation of build_cmd_buff() ensures that quoted arguments are treated as single entities by properly parsing and storing them. When a user enters a command with quoted arguments (e.g., echo "Hello World"), the function detects the presence of quotes and groups all characters inside them into a single argument. This is necessary because without handling quotes, spaces within arguments would be interpreted as argument separators, leading to incorrect parsing. Proper handling of quoted arguments ensures that commands function as expected, particularly when dealing with filenames or strings containing spaces.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in a Linux system provide a mechanism for asynchronous communication between processes and the operating system. They differ from other forms of IPC, such as pipes, shared memory, and message queues, in that they are primarily used for handling exceptional events rather than data exchange. Signals can notify a process of events like termination requests (SIGTERM), user interrupts (SIGINT), or segmentation faults (SIGSEGV). Unlike traditional IPC mechanisms, which require explicit synchronization and data management, signals are lightweight and can be sent at any time, making them ideal for process control but less suitable for structured data communication.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  	SIGKILL (9): This signal forcefully terminates a process and cannot be caught, blocked, or ignored. It is commonly used when a process is unresponsive and needs to be forcibly stopped, such as when using kill -9 in Linux.
	SIGTERM (15): This signal requests a process to terminate gracefully, allowing it to perform cleanup before exiting. It is the preferred way to stop a process because it gives the application a chance to save data and release resources.
	SIGINT (2): This signal is sent when a user presses Ctrl+C in the terminal, interrupting a running process. It is typically used to stop interactive programs without forcefully killing them, allowing them to handle cleanup before exiting.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is immediately suspended and placed in a stopped state. Unlike SIGINT, SIGSTOP cannot be caught, blocked, or ignored by the process. This is because SIGSTOP is designed to be an absolute stop signal enforced by the kernel, ensuring that any process, regardless of its implementation, can be paused. This behavior is useful for debugging (SIGSTOP is often used by debuggers like gdb to pause execution) and process management (e.g., stopping a background job in a shell). To resume a stopped process, the SIGCONT signal must be sent to it.
