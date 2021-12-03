Project Members: Duncan Proctor, Anika Patel

The TAR File Contains:

Makefile                 | contains commands to compile and make executable
README.md                | docmentation of project and distribution of labor
cd.c                     | implementation of "cd" command ; changes the current working directory to DIRNAME 
info.c                   | implementation of "info" command ; prints out field names and corresponding values
ls.c                     | implementation of "ls" command ; prints the name field for the directories within the contents of DIRNAME
size.c                   | implementation of "size" command ; prints the size of the file FILENAME in the current working directory in bytes
command.c                | obtains + manipulates commands
command.h                | header file for command.c
dispatch.c               | contains method to call functions for commands
dispatch.h               | header file for dispatch.h
fat32.c                  | implementatoin of helper functions to access fat32 file
fat32.h                  | header file for fat32.h
main.c                   | implementation of main file 
path.c                   | obtains + manipulates paths
path.h                   | header file for
repl.c                   | repl command handlers
repl.h                   | header file for repl.h
string_utils.c           | helper functions to manipulate strings
string_utils.h           | header file for string_utils.h
tool_context.c           |  
tool_context.h           | header file for tool_context.h
/test/command.c          | more functions to obtain + manipulate commands
/test/fat32.c            | further implementatoin of helper functions to access fat32 file
/test/fat32_test_tools.c | 
/test/path.c             | more functions to obtain + manipulate paths
/test/string_utils.c     | more helper functions to manipulate strings
/test/test_tools.c       | 

Makefile Details:
- make         | compiles files and generates fat32_tools.o within a directory called build

Known Bugs + Unfinished Portions:
- unfinished commands: creat FILENAME, mkdir DIRNAME, mv FROM TO, open FILENAME MODE, close FILENAME, lseek FILENAME OFFSET, read FILENAME SIZE, write FILENAME SIZE "STRING", rm FILENAME, cp FILENAME TO

Division of Labor: (Tasks were done on one computer)

- main.c | Duncan Proctor & Anika Patel
- fat32.c / fat32.h | Duncan Proctor & Anika Patel
- path.c / path.h | Duncan Proctor
- repl.c / repl.h | Duncan Proctor
- string_utils.c / string_utils.h | Duncan Proctor & Anika Patel
- tool_context.c / tool_context.h | Duncan Proctor
- dispatch.c / dispatch.h | Duncan Proctor
- command.c / command.h | Duncan Proctor
- info.c | Duncan Proctor
- ls.c | Duncan Proctor
- cd.c | Duncan Proctor
- size.c | Duncan Proctor
- Makefile | Duncan Proctor & Anika Patel
- README | Duncan Proctor & Anika Patel
- /test/command.c          | Duncan Proctor
- /test/fat32.c            | Duncan Proctor & Anika Patel
- /test/fat32_test_tools.c | Duncan Proctor
- /test/path.c             | Duncan Proctor
- /test/string_utils.c     | Duncan Proctor & Anika Patel
- /test/test_tools.c       | Duncan Proctor



 
