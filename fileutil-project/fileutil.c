#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

void check_directory(char *argv[], int d_mode);
void write_file(int src_file, int dest);
void process_parameters(int argc, char *argv[], int *d_mode, int *M_mode, int *F_mode);
void copy_to_file(char *d_path, int src_file, int F_mode);
void add_filename_to_path(char *d_path, char *filename);
void print_success(char *d_path, int mode);
int get_last_index_of(char *str, char target);

/*
 * Function: main
 * --------------
 * Main function of fileutil.
 *
 * Can be compiled from terminal using:
 * $gcc fileutil.c -o fileutil
 *
 * To run the program, use the following command:
 * $./fileutil [absolute source file path](*) [extra parameters]
 * (*): If no source file given, it will defaults to 'logfile.txt' from current
 *      working directory, './'.
 *
 * Parameters:
 * '-d': Gives a directory to copy the source file to.
 *       Will not executes if a file with the same name already existed in the
 *       directory.
 *       e.g. $./fileutil -d /home/student/Documents/storage
 *            -> Copies 'logfile.txt' to `storage`
 *
 * '-M': Must be used in conjunction with -d. Signifies 'Move' mode, deletes
 *       source file after successfully making the copy.
 *       Will not executes if a file with the same name already existed in the
 *       directory.
 *       e.g. $./fileutil -d /home/student/Documents/storage -M
 *            -> Moves 'logfile.txt' to `storage`
 *
 * '-F': Must be used in conjunction with -d (and optionally -M). Signifies
 *       'Force' mode which makes the program executes even if a file with the
 *       same name has already existed in the directory.
 *       e.g. $./fileutil -d /home/student/Documents/storage -M -F
 *            -> Moves 'logfile.txt' to `storage`, regardless of whether
 *               'logfile.txt' has existed in `storage`.
 */
int main(int argc, char *argv[])
{
  // Descriptor of the source file
  int src_file;

  // Path of the source file
  char *file_path;

  // Markers of the arguments given
  int d_mode = 0;
  int M_mode = 0;
  int F_mode = 0;

  // Default file if no file paths given from terminal
  char default_file[] = "logfile.txt";

  // Checks if a file was given from terminal
  if (argc > 1)
  {
    // Checks given arguments for parameters
    process_parameters(argc, argv, &d_mode, &M_mode, &F_mode);

    // If no source file was given, use logfile.txt
    if (d_mode == 1 || M_mode == 1 || F_mode == 1)
    {
      file_path = default_file;
    }
    else
    {
      file_path = argv[1];
    }
  }
  else
  {
    file_path = default_file;
  }

  // Get file name
  int start = get_last_index_of(file_path, '/');
  char filename[strlen(file_path)-start];
  if (start > -1)
  {
    strncpy(filename, &file_path[start+1], strlen(file_path)-start);
  }
  else
  {
    strcpy(filename, file_path);
  }

  // Open the file
  src_file = open(file_path,O_RDONLY);

  // If cannot open provided file
  if (src_file < 0)
  {
    close(src_file);
    perror("Cannot open given file.");
    perror(file_path);
    exit(1);
  }

  // If -M is given
  if (M_mode > 0)
  {
    // Makes sure -d is also given
    if (d_mode == 0)
    {
      perror("Invalid argument, no destination given to move the file!");
      exit(2);
    }

    // Checks to see if given a valid directory
    check_directory(argv, d_mode);

    // Gets the full path for output file
    char *d_path = argv[d_mode+1];
    add_filename_to_path(d_path, filename);

    // Writes content to output file and delete source file
    copy_to_file(d_path, src_file, F_mode);
    unlink(file_path);

    // Writes success message to terminal
    char success[] = "Move successful\n";
    write(1,success, strlen(success));
  }

  if (d_mode > 0 && M_mode == 0)
  {
    // Checks to see if given a valid directory
    check_directory(argv, d_mode);

    // Gets the full path for output file
    char *d_path = argv[d_mode+1];
    add_filename_to_path(d_path, filename);

    // Writes content to output file
    copy_to_file(d_path, src_file, F_mode);

    // Writes success message to terminal
    char success[] = "Copy successful\n";
    write(1,success, strlen(success));
  }

  // No further arguments given, simply display file content to terminal
  if (d_mode + M_mode + F_mode == 0)
  {
    write_file(src_file,1);
  }

  // Close file
  close(src_file);
  exit(0);
}

/*
 * Function: write_file
 * --------------------
 * Given 2 file descriptors, copy content from source file to destination file.
 *
 * src_file: file descriptor for source file
 * dest: file descriptor for target file
 *
 * returns: void
 */
void write_file(int src_file, int dest)
{
  // Set buffer to length of file (in bytes)
  int buffer_size = 1024;
  char buffer[buffer_size];

  int still_reading = 1;
  while (still_reading)
  {
    int bytes_read = read(src_file, buffer, buffer_size);

    if (bytes_read <= 0)
    {
      still_reading = 0;
    }

    int bytes_written = write(dest, buffer, bytes_read);
    if (bytes_written < 0)
    {
      perror("Writing error.");
      break;
    }
  }
}

/*
 * Function: get_last_index_of
 * ---------------------------
 * Given a string and a target character, returns the index of the last occurence
 * of the target in the string.
 *
 * str: string for searching
 * target: target character
 *
 * returns: index: index of the last occurence of the target in the string.
 */
int get_last_index_of(char *str, char target)
{
    int index = -1;
    int i = 0;

    while(str[i] != '\0')
    {
        // Update index if match is found
        if(str[i] == target)
        {
            index = i;
        }
        i++;
    }
    return index;
}

/*
 * Function: process_parameters
 * ----------------------------
 * Checks for given arguments and sets the paremeters/mode markers accordingly.
 *
 * argc: target path for written file
 * argv: file descriptor of the source file
 * d_mode: if > 0, argv[d_mode+1] must contain the destination directory for
 *         output file.
 * M_mode: if > 0, d_mode must also be > 0, deletes source file after writing.
 * F_mode: flag for 'Force' mode, if == 0: Only executes when target
 *         file has not existed, if > ): overwrites existing file.
 *
 * returns: void
 */
void process_parameters(int argc, char *argv[], int *d_mode, int *M_mode, int *F_mode)
{
  // Checks for parameters
  for (int j = 1; j < argc; j++)
  {
    // If -d is given
    if (strcmp(argv[j],"-d") == 0)
    {
      // Makes sure it's not the last parameter given
      if (j == argc-1)
      {
        perror("Invalid argument: immediately after -d, a directory path was expected.");
        exit(2);
      }
      // Set the marker
      *d_mode = j;
    }
    // Set -M marker
    if (strcmp(argv[j],"-M") == 0)
    {
      *M_mode = j;
    }
    // Set -F marker
    if (strcmp(argv[j],"-F") == 0)
    {
      *F_mode = j;
    }
  }
  if (*F_mode > 0 && *d_mode + *M_mode == 0)
  {
    perror("Invalid argument, -F is redundant as nothing to force here!");
    exit(2);
  }
}

/*
 * Function: copy_to_file
 * ----------------------
 * Given the target file path, a file descriptor for the original file,
 * and flag for 'Force' mode, copy source file content to path.
 *
 * d_path: target path for written file
 * src_file: file descriptor of the source file
 * F_mode: flag for 'Force' mode, if == 0: Only executes when target
 *         file has not existed, if > ): overwrites existing file.
 *
 * returns: void
 */
void copy_to_file(char *d_path, int src_file, int F_mode)
{
    if (F_mode == 0)
    {
      int new_file = open(d_path, O_WRONLY | O_CREAT | O_EXCL, 0664);
      if (new_file > 0)
      {
        write_file(src_file, new_file);
        close(new_file);
      }
      else
      {
        close(new_file);
        perror("File already exists in given directory.");
        exit(2);
      }
    }

    if (F_mode > 0)
    {
      int new_file = open(d_path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
      if (new_file > 0)
      {
        write_file(src_file, new_file);
        close(new_file);
      }
      else
      {
        close(new_file);
        perror("Error creating file in directory.");
        exit(2);
      }
    }
}

/*
 * Function: check_directory
 * -------------------------
 * Given the array of arguments and -d marker, checks to see if
 * the directory prodived after -d is a valid directory.
 *
 * argv: array of arguments given from terminal
 * d_mode: Marker index of the argument '-d'
 *
 * returns: void
 */
void check_directory(char *argv[], int d_mode)
{
  // Retrieve the path given for -d
  // NOTE: It is assumed that given path is absolute
  char *d_path = argv[d_mode+1];

  // Tries to see if the d_path given is valid
  int test_path = open(d_path, O_RDONLY);

  if (test_path == -1)
  {
    close(test_path);
    perror("Invalid destination to move file.");
    exit(2);
  }
  close(test_path);
}

/*
 * Function: add_filename_to_path
 * ------------------------------
 * Given the directory path and the source file name, append file name to path.
 * If the path given did not end with a '/', adds it before the file name is
 * appended.
 *
 * d_path: Given path of directory for target file
 * filename: Name of source file to use for target file
 *
 * returns: void
 */
void add_filename_to_path(char *d_path, char *filename)
{
  char slash = '/';
  // Checks if path ends with a '/'
  // Makes sure a path ends with '/'
  if (d_path[strlen(d_path) - 1] != '/')
  {
    strcat(d_path, &slash);
  }
  // Add filename to path to create the file using open()
  strcat(d_path, filename);
}

/*
 * Function: print_success
 * ------------------------------
 * Prints success message to terminal using system call.
 *
 * d_path: Given path of directory for target file
 * mode: 1: Copy. 2: Move.
 *
 * returns: void
 */
void print_success(char *d_path, int mode)
{
  char *success;
  switch (mode)
  {
    case 1:
      success = strdup("Successfully copied file to ");
      break;
    case 2:
      success = strdup("Successfully moved file to ");
      break;
  }
  strcat(success, d_path);
  strcat(success, ".\n");
  write(1,success, strlen(success));
}
