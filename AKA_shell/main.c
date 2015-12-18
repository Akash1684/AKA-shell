/***************************************************************************//**

   main.c

   Akash Srivastava
   National Instiute of Technology Patna,India
   31 Oct 2015

                                  
								AKAsh(AKASH_shell)

*******************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>



/*
  Function Declarations for builtin shell commands:
 */
int akash_cd(char **args);
int akash_help(char **args);
int akash_hist(char **args);
int akash_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "hist",
  "exit"
};



int (*builtin_func[]) (char **) = {
  &akash_cd,
  &akash_help,
  &akash_hist,
  &akash_exit
};

int akash_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int akash_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "aka: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("aka");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int akash_help(char **args)
{
  int i;
  printf("Akash Srivastava's AKA_sh\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < akash_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: hist.
   @param args List of args.  Not examined.
   @return Always returns 1.
 */


int akash_hist(char **args)
{
  printf("\n");
  system("cat aka_hist.txt") ;
  printf("\n");
  return 1;
}


/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int akash_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int akash_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("aka");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("aka");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int akash_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < akash_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return akash_launch(args);
}

#define AKASH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *akash_read_line(void)
{
  int bufsize = AKASH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "aka: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += AKASH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "aka: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define AKASH_TOK_BUFSIZE 64
#define AKASH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **akash_split_line(char *line)
{
  int bufsize = AKASH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "aka: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, AKASH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += AKASH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "aka: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, AKASH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void akash_write_to_file(char *line)
{
   FILE *p;
   char com[50];
   p = fopen("aka_hist.txt", "a");
   time_t t;
   time(&t);
   strcpy(com,line);
   if(strcmp(com,"\n")==0)
     fprintf(p,"%s : %s",line,ctime(&t));
   //fprintf(p,"\n");
   fclose(p);
}


/**
   @brief Loop getting input and executing it.
 */
void akash_loop(void)
{
  char *line;
  char **args;
  int status;
  static int k=0;

  do {
    if(k!=0)
     printf("AKA> ");
    k++;
	    
    line = akash_read_line();
    akash_write_to_file(line);
    args = akash_split_line(line);
    status = akash_execute(args);

    free(line);
    free(args);
  } while (status);
}


/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */

int validate()
{
	char c,user[20],pass[20];
	static int i=3;
	//system("clear");
	printf("User : ");
	scanf("%s",user);
	printf("Password : ");
	scanf("%s",pass);
	if(strcmp(user,"user")!=0)
	{ //check for invalid username
		if(i==1)
		 return 0;
		i--; 
		printf("\nInvalide Username\nAttempts Remaining: %d",i);
		scanf("%c",&c);
		printf("\n\n");
		validate();
	}
	else if(strcmp(pass,"password")!=0)
	{  //check for invalid password
		return 0;
	}
	else
	  return 1;
}


int main(int argc, char **argv)
{
  system("clear");
  int check=validate();
  
  if(check==1){  	
  system("clear");
  // Load config files, if any.
  printf("Welcome to AKA_shell \n\n");
  // Run command loop.
  akash_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
  }
  
  else return 0;
}

