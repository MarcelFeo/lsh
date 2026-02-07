#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* ========== READ LINE ========== */

#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void) {
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(bufsize);
    int c;

    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        buffer[position++] = c;

        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
        }
    }
}

/* ========== PARSER ========== */

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char **lsh_split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

/* ========== BUILTINS ========== */

int lsh_cd(char **args) {
    if (args[1] == NULL)
        fprintf(stderr, "lsh: expected argument\n");
    else
        chdir(args[1]);
    return 1;
}

int lsh_help(char **args) {
    printf("Simple C Shell\n");
    printf("Builtins: cd, help, exit\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

char *builtin_str[] = {"cd", "help", "exit"};
int (*builtin_func[]) (char **) = {&lsh_cd, &lsh_help, &lsh_exit};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/* ========== PROCESS EXEC ========== */

int lsh_launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("lsh");
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("lsh");
    }
    else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

/* ========== EXECUTOR ========== */

int lsh_execute(char **args) {
    if (args[0] == NULL)
        return 1;

    for (int i = 0; i < lsh_num_builtins(); i++)
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);

    return lsh_launch(args);
}

/* ========== LOOP ========== */

void lsh_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

/* ========== MAIN ========== */

int main() {
    lsh_loop();
    return EXIT_SUCCESS;
}
