# Description
This is my implementation of the shell for Linux in C. Support for basic terminal commands has been
provided, along with some additional functions.
This was done as part of a project for the Operating Systems and Networks course at IIIT Hyderabad in the Monsoon 2024 semester.

## Commands

### hop
**Syntax:**
```bash
hop [flags] [path]
```
**Flags:**
- `.`: Current directory
- `..`: Parent directory
- `~`: Home directory
- `-`: Previous directory

**Description:**
‘hop’ command changes the directory that the shell is currently in.

### reveal
**Syntax:**
```bash
peek [flags] [path]
```
**Flags:**
- `-a`: Show hidden files
- `-l`: Display extra information

**Description:**
‘reveal’ command lists all the files and directories in the specified directories in lexicographic order (default reveal does not show hidden files).

### log
**Syntax:**
```bash
log
log purge
log execute <index>
```

**Description:**
'log' shows the 15 most recent commands. 'log purge' clears the log. 'log execute <index>'  runs a command from the log.

### proclore
**Syntax:**
```bash
proclore
proclore [pid]
```
**Description:**
'proclore' is used to obtain information regarding a process. If an argument is missing, information of the shell is printed.

### seek
**Syntax:**
```bash
proclore [flags] [path] [[ath]
```
**Flags:**
- `d`:  Only look for directories.
- `f.`:  Only look for files.
- `e`: This flag is effective only when a single file or a single directory with the name is found. If only one file (and no directories) is found, then print it’s output. If only one directory (and no files) is found, then change current working directory to it. Otherwise, the flag has no effect. This flag should work with -d and -f flags.

**Description:**
‘seek’ command looks for a file/directory in the specified target directory (or current if no directory is specified).

### I/O Redirection
Input and Output redirection is supported in the following manner.
```bash
command > output_file
command >> output_file
command < input_file
```
**Description:**
- > : Outputs to the filename following “>”.
- >> : Similar to “>” but appends instead of overwriting if the file already exists.
- < : Reads input from the filename following “<”.

### Pipes
Pipes have been supported in the manner of a general shell in Linux.
I/O Redirection along with pipes has also been handled.

### activities
**Syntax:**
```bash
activities
```
**Description:**
- Prints a list of all the processes currently running that were spawned by your shell in lexicographic order, along with some other useful information about the processes.

### ping
**Syntax:**
```bash
ping <pid> <signal_number>
```
**Description:**
- 'ping' command is used to send signals to processes.

### Signals
Ctrl+C, Ctrl+D, Ctrl+Z have been implemented according to their signals.
- C-> Interrupt any currently running foreground process by sending it the SIGINT signal. It has no effect if no foreground process is currently running.
- D-> Log out of your shell (after killing all processes) while having no effect on the actual terminal.
- Z-> Push the (if any) running foreground process to the background and change it’s state from “Running” to “Stopped”. It has no effect on the shell if no foreground process is running.
- \ -> Same as D.

### fg and bg
**Syntax:**
```bash
fg <pid>
bg <pid>
```
**Description:**
- - 'fg' brings the running or stopped background process with corresponding pid to foreground, handing it the control of terminal.
- - 'bg' changes the state of a stopped background process to running (in the background).
 
### neonate
**Syntax:**
```bash
neonate -n [time_arg]
```
**Description:**
- The command prints the Process-ID of the most recently created process on the system (you are not allowed to use system programs), this pid will be printed every [time_arg] seconds until the key ‘x’ is pressed.

### iMan
**Syntax:**
```bash
iMan <command_name>
```
**Description:**
- Fetches man pages from the website- http://man.he.net/ using sockets and outputs it to the terminal.

- ### myshrc
- Aliases of functions can be made. Open .myshrc file to find out how to do so.
