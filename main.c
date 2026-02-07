#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

/* ================= CONFIG ================= */

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define HISTORY_SIZE 100

/* ================= HISTORY ================= */

char *history[HISTORY_SIZE];
int history_count = 0;

void add_history(char *line) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(line);
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d  %s\n", i + 1, history[i]);
    }
}

/* ================= PROMPT ================= */

void print_prompt() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("\033[1;32mlsh\033[0m:\033[1;34m%s\033[0m$ ", cwd);
}

/* ================= READ LINE ================= */

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

/* ================= PARSER ================= */

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

/* ================= BUILTINS ================= */

int lsh_cd(char **args) {
    if (args[1] == NULL)
        fprintf(stderr, "lsh: expected argument to cd\n");
    else
        chdir(args[1]);
    return 1;
}

int lsh_pwd(char **args) {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    return 1;
}

int lsh_echo(char **args) {
    for (int i = 1; args[i] != NULL; i++)
        printf("%s ", args[i]);
    printf("\n");
    return 1;
}

int lsh_clear(char **args) {
    system("clear");
    return 1;
}

int lsh_history(char **args) {
    show_history();
    return 1;
}

int lsh_help(char **args) {
    printf("LSH - Learning Shell\n");
    printf("Builtins:\n");
    printf("  cd, pwd, echo, clear, history, help, exit\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

/* builtin registry */
char *builtin_str[] = {
    "cd", "pwd", "echo", "clear", "history", "help", "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_pwd,
    &lsh_echo,
    &lsh_clear,
    &lsh_history,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/* ================= EXECUTION ================= */

int lsh_launch(char **args, int background) {
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
        if (!background) {
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
            printf("[bg] process started pid=%d\n", pid);
        }
    }

    return 1;
}

/* ================= EXECUTOR ================= */

int lsh_execute(char **args) {
    if (args[0] == NULL)
        return 1;

    /* background execution */
    int background = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "&") == 0) {
            background = 1;
            args[i] = NULL;
        }
    }

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);
    }

    return lsh_launch(args, background);
}

/* ================= LOOP ================= */

void lsh_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        print_prompt();

        line = lsh_read_line();
        add_history(line);

        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);

    } while (status);
}

/* ================= MAIN ================= */

int main() {
    lsh_loop();
    return EXIT_SUCCESS;
}
