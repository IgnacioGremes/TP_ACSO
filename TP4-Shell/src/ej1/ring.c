#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int n, c, s;
    int buffer;

    if (argc != 4) {
        printf("Uso: ring <n> <c> <s>\n");
        exit(1);
    }
    n = atoi(argv[1]);
    c = atoi(argv[2]);
    s = atoi(argv[3]) - 1;

    if ( s < 0 || s >= n) {
        printf("Error: s entre 1 y %d\n", n);
        exit(1);
    }

    // Creamos las pipes para conectar a todos los hijos del anillo entre ellos
    int pipes[n][2];
    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error creando pipe");
            exit(1);
        }
    }

    // Create pipes for parent-to-s and last-to-parent
	// Creamos pipe para conectar al padre con el hijo inicial indicado por s y otra pipe para conectar al hijo final del anillo con el padre.
    int padre_a_s[2], ultimo_a_padre[2];
    if (pipe(padre_a_s) == -1 || pipe(ultimo_a_padre) == -1) {
        perror("Error creating pipes al padre");
        exit(1);
    }

    // Creamos hijos
    pid_t pids[n];
    for (int i = 0; i < n; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("Error de fork");
            exit(1);
        }
        if (pids[i] == 0) { // Hijo
            // Cerramos las pipes no usadas por el hijo actual
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    close(pipes[j][0]); // Cerramos las pipes de lectura salvo la utilizada por este hijo
                }
                if (j != (i + 1) % n) {
                    close(pipes[j][1]); // Cerramos las pipes de escritura salvo la utilizada por este hijo
                }
            }
            if (i != s) { // si el hijo no es el inicial cerramos la pipe de coneccion con el padre
                close(padre_a_s[0]);
                close(padre_a_s[1]);
            }
            if (i != (s - 1 + n) % n) { // si el hijo no es el ultimo del anillo cerramos la pipe de coneccion con el padre.
                close(ultimo_a_padre[0]);
                close(ultimo_a_padre[1]);
            }
			// Lectura del numero
            if (i == s) { // caso hijo inicial lee de padre
                close(padre_a_s[1]);
                if (read(padre_a_s[0], &buffer, sizeof(int)) != sizeof(int)) {
                    perror("Error lectura del hijo s");
                    exit(1);
                }
                close(padre_a_s[0]);
            } else { // caso de hijo lee de otro hijo
                if (read(pipes[i][0], &buffer, sizeof(int)) != sizeof(int)) {
                    perror("Error lectura de hijo");
                    exit(1);
                }
                close(pipes[i][0]);
            }

            buffer += 1;

            // Escritura del numero
            if (i == (s - 1 + n) % n) { // caso hijo final escribe al padre
				close(pipes[(i + 1) % n][1]); 
                if (write(ultimo_a_padre[1], &buffer, sizeof(int)) != sizeof(int)) {
                    perror("Error de escritura del ulitmo hijo");
                    exit(1);
                }
                close(ultimo_a_padre[1]);
            } else { // caso hijo escribe al hijo siguiente
                if (write(pipes[(i + 1) % n][1], &buffer, sizeof(int)) != sizeof(int)) {
                    perror("Error escritura de hijo");
                    exit(1);
                }
                close(pipes[(i + 1) % n][1]);
            }

            exit(0);
        }
    }

    // Padre
    // Cerramos todas las pipes no usadas por el padre
    for (int i = 0; i < n; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    close(padre_a_s[0]);
    close(ultimo_a_padre[1]);

    // Enviar numero inicial al hijo s
    buffer = c;
    if (write(padre_a_s[1], &buffer, sizeof(int)) != sizeof(int)) {
        perror("Error escritura de padre");
        exit(1);
    }
    close(padre_a_s[1]);

    // Leemos valor recibido del hijo final
    if (read(ultimo_a_padre[0], &buffer, sizeof(int)) != sizeof(int)) {
        perror("Error lectura de padre");
        exit(1);
    }
    close(ultimo_a_padre[0]);

    printf("Resultado final: %d\n", buffer);

    // El padre espera a todos lo hijos
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    return 0;
}