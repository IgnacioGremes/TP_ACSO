#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 20

int main() {
    char command[256];
    char *commands[MAX_COMMANDS];
    char *args[MAX_ARGS];
    int command_count;

    while (1) {
        printf("Shell> ");
        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            break;
        }
        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */

        command_count = 0;
        char *token = strtok(command, "|");
        while (token != NULL && command_count < MAX_COMMANDS) {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        if (command_count == 0) {
            continue;
        }

        // Si tenemos mas de 1 comando creamos un pipe para mandar el output de un comando como input del siguiente
        int pipes[command_count - 1][2];
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        pid_t pids[MAX_COMMANDS];
        // Por cada comando hacemos un fork 
        for (int i = 0; i < command_count; i++) {
            // Conseguimos los argumentos de cada comando a partir de las separaciones por espacios y agregamos un NULL al final
            int arg_count = 0;
            token = strtok(commands[i], " \t");
            while (token != NULL && arg_count < MAX_ARGS - 1) {
                args[arg_count++] = token;
                token = strtok(NULL, " \t");
            }
            args[arg_count] = NULL; // Aca se agrega el NULL

            pids[i] = fork();
            if (pids[i] == -1) {
                perror("fork");
                exit(1);
            }
            if (pids[i] == 0) { // Hijo
                // Redirigimos el standart input del comando actual a la lectura del pipe que conecta este comando con el anterior, para todos los comandos salvo el primero.
                if (i > 0) {
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                        perror("dup2 input");
                        exit(1);
                    }
                }
                // Redeirigimos el standard output del comando actual a la escritura del pipe que conecta con el comando siguiente, para todos los comandos salvo el ultimo.
                if (i < command_count - 1) {
                    if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                        perror("dup2 output");
                        exit(1);
                    }
                }

                // Cerramos todas las pipes luego de haber redirigido la lectura y escritura del comando actual
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Ejecutamos el comando 
                if (execvp(args[0], args) == -1) {
                    perror("execvp");
                    exit(1);
                }
            }

            // Padre cierra las pipes usadas por el hijo
            if (i > 0) {
                close(pipes[i - 1][0]); // Cierra la pipe de lectura del hijo actual con el anterior
            }
            if (i < command_count - 1) {
                close(pipes[i][1]); // Cierra la pipe de escritura del hijo actual con el siguiente.
            }
        }

        // Padre cierra todas las pipes para asegurar que no haya quedado ninguna abierta.
        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // El padre espera a que todos los hijos terminen.
        for (int i = 0; i < command_count; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}