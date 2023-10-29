#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define IMG_DIST_PATH "./img-dist/img-dist"

// Fonction qui compare deux images et retourne la distance entre les deux
int compare_image(char path_image1[999], char path_image2[999]);

int main(int argc, char* argv[]) {
    // Vérifier que le nombre d'arguments est correct
    if (argc < 1) {
        printf("Usage: %s <image_to_compare_path>\n", argv[0]);
        exit(1);
    }

    // Copier le chemin de l'image à comparer dans une variable
    char image_to_compare_path[999];
    strcpy(image_to_compare_path, argv[1]);

    // Variables pour stocker les chemins d'images
    char path_image1[999] = "";
    char path_image2[999] = "";

    // Créer les deux processus fils
    pid_t pid1 = fork();
    if (pid1 < 0) {  // Erreur de fork
        perror("fork()");
        return 1;
    } else if (pid1 == 0) {  // Processus fils 1
        while (fgets(path_image1, sizeof(path_image1) / sizeof(char), stdin) != NULL) {
            // Supprimer le retour à la ligne à la fin du buffer
            size_t len = strlen(path_image1);
            if (path_image1[len - 1] == '\n') {
                path_image1[len - 1] = '\0';
            }

            // Attendre que le processus fils 2 ait fini de comparer les images
            int status;
            wait(&status);

            // Comparer les images
            printf("pid1: %i\n", getpid());
            int return_value = compare_image(image_to_compare_path, path_image1);
            printf("La distance entre les deux images est de : %i\n", return_value);
        }

        // Terminer le processus fils 1
        exit(0);
    } else {  // Processus père
        // Créer le deuxième processus fils
        pid_t pid2 = fork();
        if (pid2 < 0) {  // Erreur de fork
            perror("fork()");
            return 1;
        } else if (pid2 == 0) {  // Processus fils 2
            while (fgets(path_image2, sizeof(path_image2) / sizeof(char), stdin) != NULL) {
                // Supprimer le retour à la ligne à la fin du buffer
                size_t len = strlen(path_image2);
                if (path_image2[len - 1] == '\n') {
                    path_image2[len - 1] = '\0';
                }

                // Attendre que le processus fils 1 ait fini de comparer les images
                int status;
                wait(&status);

                // Comparer les images
                printf("pid2: %i\n", getpid());
                int return_value = compare_image(image_to_compare_path, path_image2);
                printf("La distance entre les deux images est de : %i\n", return_value);
            }

            // Terminer le processus fils 2
            exit(0);
        } else {  // Processus père
            // Attendre la fin des deux processus fils
            int status1, status2;
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);
        }
    }

    return 0;
}

int compare_image(char path_image1[999], char path_image2[999]) {
    int return_value = -1;
    // Utilisation du programme img-dist qui prend en paramètre deux images et qui retourne la distance entre les deux

    // Fork pour exécuter le programme img-dist
    pid_t pid = fork();
    if (pid < 0) {  // Erreur de fork
        perror("fork()");
        return 1;
    } else if (pid == 0) {  // Processus fils
        // Exécution de la commande img-dist avec les deux chemins d'images en paramètres
        char* argument_list[] = {IMG_DIST_PATH, path_image1, path_image2, NULL};
        execvp(IMG_DIST_PATH, argument_list);

        // Afficher une erreur si l'exécution de la commande a échoué
        if (return_value < 0) {
            perror("Fils: execlp()");
            return 1;
        }
    } else {  // Processus père
        // Attendre la fin du processus fils et récupérer sa valeur de retour
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return_value = WEXITSTATUS(status);
        } else {
            printf("Le fils s'est terminé de manière inattendue\n");
        }
    }

    return return_value;
}