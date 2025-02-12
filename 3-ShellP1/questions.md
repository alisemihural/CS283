1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  It is a good choice because it reads a line of input from the user including the newline character and stores it in a buffer. This makes it easy to handle user input line by line which is essential for a command-line shell. Also it allows us to specify the maximum number of characters to read preventing buffer overflow and safe input handling.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  In this assignment I did not actually use malloc() to allocate memory for cmd_buff, instead I used a fixed-size array. However, if we were to use malloc(), it would be to dynamically allocate memory based on the actual input size, which can be more flexible and efficient in handling varying input lengths. Using malloc() allows us to allocate just enough memory needed at runtime, rather than reserving a potentially large fixed-size buffer that may not always be fully utilized.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces from each command is necessary to ensure that the commands are correctly parsed and executed. If we didn't trim spaces, the shell might interpret the commands incorrectly, leading to errors or unexpected behavior. For example, extra spaces could cause the shell to fail to recognize a command or pass incorrect arguments to a command, resulting in execution failures or incorrect output.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  1-Output Redirection (>): Redirects the standard output (STDOUT) of a command to a file. Example: ls > output.txt. Challenge: Implementing file handling and ensuring the file is created or overwritten correctly. 2-Input Redirection (<): Redirects the standard input (STDIN) of a command from a file. Example: sort < input.txt. Challenge: Reading input from a file and passing it to the command correctly. 3-Append Output Redirection (>>): Appends the standard output (STDOUT) of a command to a file. Example: echo "Hello" >> output.txt. Challenge: Ensuring the file is opened in append mode and handling file permissions.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection involves directing the input or output of a command to or from a file, whereas piping involves passing the output of one command directly as input to another command. Redirection is used for file operations, while piping is used for chaining commands together to process data streams.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: Keeping STDERR and STDOUT separate is important because it allows users and scripts to distinguish between regular output and error messages. This separation helps in debugging and error handling, as error messages can be redirected or logged separately from the regular output, making it easier to identify and address issues.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: Our custom shell should handle errors by capturing and displaying STDERR separately from STDOUT. However, we should also provide an option to merge them if needed. This can be done using redirection operators like 2>&1 to merge STDERR into STDOUT. For example, command > output.txt 2>&1 would redirect both STDOUT and STDERR to the same file. This flexibility allows users to choose how they want to handle and view command output and errors.