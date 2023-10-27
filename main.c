#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define IMG_DIST_PATH "./img-dist/img-dist"

// Fonction qui compare deux images et retourne la distance entre les deux
int compare_image(char path_image1[999]);

int main(int argc, char* argv[]) {
    // Vérifier que le nombre d'arguments est correct
    if (argc < 1) {
        printf("Usage: %s <image_to_compare_path>\n", argv[0]);
        exit(1);
    }

    // Copier le chemin de l'image à comparer dans une variable
    char image_to_compare_path[999];
    strcpy(image_to_compare_path, argv[1]);

    // Appeler la fonction de comparaison d'images
    int return_value = compare_image(image_to_compare_path);
    return return_value;
}

int compare_image(char path_image1[999]) {
    int return_value = -1;
    // Utilisation du programme img-dist qui prend en paramètre deux images et qui retourne la distance entre les deux
    char path_image2[999] = "";

    // Lire l'entrée standard pour récupérer le chemin de la deuxième image
    while (fgets(path_image2, sizeof(path_image2) / sizeof(char), stdin) != NULL) {
        // Supprimer le retour à la ligne à la fin du buffer
        size_t len = strlen(path_image2);
        if (path_image2[len - 1] == '\n') {
            path_image2[len - 1] = '\0';
        }

        // Fork pour exécuter le programme img-dist
        pid_t pid = fork();
        if (pid < 0) {  // Erreur de fork
            perror("fork()");
            return 1;
        } else if (pid > 0) {  // Processus père
            // Attendre la fin du processus fils et récupérer sa valeur de retour
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                return_value = WEXITSTATUS(status);
                printf("La distance entre les deux images est de : %i\n", return_value);
            } else {
                printf("Le fils s'est terminé de manière inattendue\n");
            }
        } else {  // Processus fils
            // Exécution de la commande img-dist avec les deux chemins d'images en paramètres
            char* argument_list[] = {IMG_DIST_PATH, path_image1, path_image2, NULL};
            execvp(IMG_DIST_PATH, argument_list);

            // Afficher une erreur si l'exécution de la commande a échoué
            if (return_value < 0) {
                perror("Fils: execlp()");
                return 1;
            }
        }
    }

    // Afficher une erreur si la lecture de l'entrée standard a échoué
    if (ferror(stdin)) {
        perror("Erreur lors de la lecture sur stdin");
        return 1;
    }

    return 0;
}