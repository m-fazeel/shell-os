# Mav Shell (msh)

Mav shell (msh) is a shell utility, similar to bourne shell (bash), c-shell (csh), or korn shell (ksh). It accepts commands, forks a child process, and executes those commands. The shell, like csh or bash, will run and accept commands until the user exits the shell.

# Table of Contents

- [Mav Shell (msh)](#mav-shell-msh)
- [Table of Contents](#table-of-contents)
- [Usage](#usage)
- [Running the Program](#running-the-program)
- [Supported Commands](#supported-commands)
- [Command Parameters](#command-parameters)
- [History and Re-running Commands](#history-and-re-running-commands)
- [Implementation Details](#implementation-details)
- [Exit Status](#exit-status)
- [Error Handling](#error-handling)
- [Copyright](#copyright)

# Usage

The program will print out a prompt of `msh>` when it is ready to accept input. It reads a line of input and, if the command given is a supported shell command, it shall execute the command and display the output.

```
msh> [command]
```

# Running the Program

To run the Mav Shell program, you can use the following command:

```shell
gcc [filename]
./msh
```
Ensure that the program has executable permissions. If not, you can add them using the following command:
```
chmod +x msh
```

# Supported Commands

Mav shell supports any command present in `/bin`, `/usr/bin/`, `/usr/local/bin/` and the current working directory.

Additionally, the shell supports the following built-in commands:

- `cd`: Change directories. Also supports cd .. to navigate to the parent directory.
- `history`: List the last 15 commands entered by the user.

```
msh> cd [directory]
msh> history
```

# Command Parameters
The shell can support up to 10 command line parameters in addition to the command.

# History and Re-running Commands
Mav shell keeps track of the last 15 commands entered by the user. These can be viewed using the `history` command.

```
msh> history
```
You can rerun any of the previous commands by typing `!n`, where `n` is the index number of the command in the history (between 0 and 14).
```
msh> !n
```
If the nth command does not exist, the shell will state “Command not in history.”

The `history` command also supports a `-p` parameter, which will list the associated PID with each command.

```
msh> history -p
````

# Implementation Details
Mav shell is implemented using `fork()`, `wait()` and one of the `exec` family of functions. The shell does not use `system()`.

# Exit Status

Mav shell will exit with status zero if the command is `quit` or `exit`.
```
msh> quit
msh> exit
```

# Error Handling

If a command is not supported, the shell will print the invalid command followed by “: Command not found.” After each command completes, the shell will print the `msh>` prompt and accept another line of input.

If the user types a blank line, the shell will, quietly and with no other output, print another prompt and accept a new line of input.

If there are less than 15 commands in the history, the `history` command will only list the commands the user has entered up to that point.

# Copyright

Copyright © 2023. All rights reserved.

This software is provided for educational purposes only. It is prohibited to use this shell, Mav Shell (msh), for any college assignment or personal use. Unauthorized distribution, modification or commercial usage is strictly forbidden. Please respect the rights of the author.
