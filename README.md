# C-projects
Some projects written in C.

## 1. File Utility Program Documentation
--------------------------------------------------------------------------------
## HOW TO COMPILE:
This program an be compiled from terminal using GCC:
$ gcc fileutil.c -o fileutil
--------------------------------------------------------------------------------
## HOW TO RUN AND USE THE PROGRAM:
To run the program, use the following command:
$./fileutil [absolute source file path](*) [extra parameters](**)
(*): If no source file given, it will defaults to 'logfile.txt' from current
      working directory, './'.
(**): All parameters can be in any given order. The source file path must always
      be the first argument if once is given. '-d' must not be the last argument,
      as the target directory must always be the argument after '-d'.

Parameters:
'-d': Gives a directory to copy the source file to.
      Will not executes if a file with the same name already existed in the
      directory.
      e.g. $./fileutil -d /home/student/Documents/storage
           -> Copies 'logfile.txt' to `storage`

'-M': Must be used in conjunction with -d. Signifies 'Move' mode, deletes
      source file after successfully making the copy.
      Will not executes if a file with the same name already existed in the
      directory.
      e.g. $./fileutil -d /home/student/Documents/storage -M
           -> Moves 'logfile.txt' to `storage`

'-F': Must be used in conjunction with -d (and optionally -M). Signifies
     'Force' mode which makes the program executes even if a file with the
      same name has already existed in the directory.
      e.g. $./fileutil -d /home/student/Documents/storage -M -F
           -> Moves 'logfile.txt' to `storage`, regardless of whether
             'logfile.txt' has existed in `storage`.
