#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LENGTH 1024
#define TRUE 1
#define FALSE 0


int status;



void get_time()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  printf("Current time: %d-%d-%d %d:%d:%d", tm.tm_year + 1900,
  tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  printf("#\n");

}


void get_address()
{
  char hostnum[MAX_LENGTH] = "";
  gethostname(hostnum, sizeof(hostnum));
  char* currentDirectory = (char*) calloc(MAX_LENGTH,sizeof(char));
  printf("%s@%s %s > ", getenv("LOGNAME"), hostnum,getcwd(currentDirectory, MAX_LENGTH) );

}


//count how many arguments are in the command
int args_count(char* buffer)
{
    int count=0;
    int i = 0;
    while (buffer[i]!='\n')
    {
        if (buffer[i]==' ')
            count++;
        i++;
    }
    count++;
    return count;
}

void fork_exec(char* buffer)
{
  int pid = fork();

  if(pid!=0) // the parent
  {
          waitpid(pid, &status, WUNTRACED);
  }
  else // the child
  {
      int arguments = args_count(buffer);

      //Add one to the arguments so that the last one array is NULL
      char** array = malloc((sizeof(char*)*(arguments+1)));

      //break the string up and create an array of pointers that point to each of the arguments.
      int i=0;
      char* command;

      if (arguments>1)
      {
          command = strtok (buffer," ");
          while (command != NULL)
          {
              array[i]=(char*)malloc((sizeof(char)*strlen(command)));
              strcpy(array[i], command);
              command = strtok (NULL, " ");
              i++;
          }
      }
      else
      {
          i=1;
          array[0] = buffer;
          array[1] = NULL;
      }
      //deal with the '\n' in the string
      i--;
      int j=0;
      while(1)
      {
          if (array[i][j] == '\n')
          {
              array[i][j] = 0;
              break;
          }
          j++;
      }

      //format for command is eg. ls -a -l
      //the first element in the array will be the programram name
      char* program = malloc(sizeof(char)*(strlen(array[0])));
      program = array[0];

      //pass the prepared arguments to execv and we're done!
      int rv = execvp(program, array);

  }
}



int main(int argc, char **argv)
{
        //get command buffer
        char buffer[MAX_LENGTH];
        //get current time
        get_time();
        while(1)
        {
                // get current address
                get_address();

                //get input
                if(!fgets(buffer, MAX_LENGTH, stdin))
                {
                    break;
                }
                // comment exit
                if(strcmp(buffer, "exit\n")==0)
                {
                    break;
                }

                //fork
                fork_exec(buffer);
        }
        return 0;
}
