// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 12     // Mav shell only supports four arguments

#define MAX_HISTORY_NUM 15


//Helper function for history array shifting if it reaches max size
void array_shifting(char hist_arr[MAX_HISTORY_NUM][MAX_COMMAND_SIZE], char* working_string)
{
  for (int i = 0; i < MAX_HISTORY_NUM - 1; i++)
  {
    strcpy(hist_arr[i], hist_arr[i + 1]);
  }
  strcpy(hist_arr[MAX_HISTORY_NUM - 1], working_string);
}

int main()
{
  //S
  pid_t pid = -1;
  char* command_string = (char*)malloc(MAX_COMMAND_SIZE);
  char history_arr[MAX_HISTORY_NUM][MAX_COMMAND_SIZE];
  int history_pid[MAX_HISTORY_NUM] = { -1 };
  int history_pid_count = 0;
  int history_count = 0;

  while (1)
  {
    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));
    if ((strcmp(command_string, "\n") == 0))
    {
      continue;
    }
    /* Parse input */
    char* token[MAX_NUM_ARGUMENTS];

    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      token[i] = NULL;
    }

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char* argument_ptr = NULL;

    char* working_string = strdup(command_string);

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char* head_ptr = working_string;

    
    // if !, then replacing working_string with the string from history
    if (command_string[0] == '!')
    {
      int get_index = atoi(&command_string[1]);
      if (get_index < history_count)
      {
        working_string = strdup(history_arr[get_index]);
      }
      else
      {
        printf("Command not in history. \n");
        continue;
      }
    }

    // Copying the string to history array if count is less than MAX_HISTORY_NUM
    // and incrementing the count
    if ((history_count < MAX_HISTORY_NUM))
    {
      strcpy(history_arr[history_count], working_string);
      history_count++;
    }
    // If the count is greater, then shifting each index by one
    // to store the last input
    else
    {
      array_shifting(history_arr, working_string);
    }
    // Assigning -1 to the history pid, will replace it later if in built command
    history_pid[history_pid_count] = -1;

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) &&
      (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    // Checking command line arguments from here
    if (token[0] == NULL)
    {
      continue;
    }

    // if exit or quit, then exit the program
    if (strcmp(token[0], "exit") == 0 || strcmp(token[0], "quit") == 0)
    {
      exit(0);
    }
    // If the input is history, then printing the previous history
    else if (strcmp(token[0], "history") == 0 && token[1] == NULL)
    {
      for (int i = 0; i < history_count;i++)
      {
        printf("[%d]: %s", i, history_arr[i]);
      }

    }
    // Comparing the input with history -p to get the history along with the pids
    else if (strcmp(token[0], "history") == 0 && (strcmp(token[1], "-p") == 0))
    {

      for (int i = 0; i < history_count;i++)
      {
        printf("[%d]: [%d] %s", i, history_pid[i], history_arr[i]);
      }
    }

    // If input is cd, then changing the directory
    else if (strcmp(token[0], "cd") == 0)
    {
      if (token[1] == NULL)
      {
        printf("Please specify a directory.\n");
      }
      else
      {
        // Getting the return value to check for error
        int ret_val = chdir(token[1]);
        if (ret_val == -1)
        {
          printf("no such file or directory: %s\n", token[1]);
        }
      }
    }
    // All the above commands fail, which means it is in built command
    else
    {
      // Calling fork
      pid = fork();
      // replacing the history pid array at the given index with pid number
      history_pid[history_pid_count] = pid;

      // Entering the child state and executing the command
      if (pid == 0)
      {

        int ret = execvp(token[0], &token[0]);
        if (ret == -1)
        {
          printf("%s: Command not found.\n", token[0]);
          return 0;
        }

      }
      // If the return value is -1, then there was an error while forking
      else if (pid == -1)
      {
        printf("Error occured while forking.\n");
        exit(0);
      }
      // Entering the parent state and waiting until the child completes
      else
      {
        int status;
        wait(&status);
      }

    }

    // Incrementing the history pid count if pid array is less than the MAX
    if (history_pid_count < MAX_HISTORY_NUM - 1)
    {
      history_pid_count++;
    }
    // If it is reaches the max count, then shifting each index up by 1
    else
    {
      for (int i = 0; i < MAX_HISTORY_NUM - 1; i++)
      {
        history_pid[i] = history_pid[i + 1];
      }
      history_pid[MAX_HISTORY_NUM - 1] = pid;
    }

    // Cleanup allocated memory
    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      if (token[i] != NULL)
      {
        free(token[i]);
      }
    }

    free(head_ptr);
  }

  free(command_string);

  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}

