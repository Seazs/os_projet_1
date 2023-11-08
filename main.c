//Pour que ANONYMOUS MAP fonctionne
#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

volatile sig_atomic_t sigint_received = 0;

// Structure pour stocker les données partagées entre les processus
struct shared_data {
    int min_distance;
    char min_distance_image_path[999];
};

// Fonction qui compare deux images et retourne la distance entre les deux
void compare_image(char path_image1[999], char path_image2[999], struct shared_data* data);
// Fonction qui gère les signaux
void signal_handler(int signal_number);
// Fonction qui execute le code des fils
void child_process(int child_pipe[2], int other_child_pipe[2], char image_to_compare[999], char image_path[999], size_t len_path_image, struct shared_data* data);
// Fonction qui execute le code du père
void parent_process(pid_t pid1, pid_t pid2, int pipe1[2], int pipe2[2], char image_path[999], size_t len_path_image, struct shared_data* data);

int main(int argc, char* argv[]) {
    // Vérifier que le nombre d'arguments est correct
    if (argc < 1) {
        printf("Usage: %s <image_to_compare_path>\n", argv[0]);
        exit(1);
    }

    // Copier le chemin de l'image à comparer dans une variable
    char image_to_compare_path[999];
    strcpy(image_to_compare_path, argv[1]);
    printf("Image to compare: %s\n", image_to_compare_path);

    // Variables pour stocker les chemins d'images
    char path_image[999] = "";
    
    // Créer la région de mémoire partagée
    struct shared_data* data = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    if (data == MAP_FAILED) {
        perror("mmap()");
        exit(1);
    }

    // Initialiser les données partagées
    data->min_distance = 999999;
    strcpy(data->min_distance_image_path, "");

    //créer les pipes pour la communication entre le père et les fils
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1) {
        perror("pipe1()");
        exit(1);
    }
    if (pipe(pipe2) == -1) {
        perror("pipe2()");
        exit(1);
    }

    // Créer les deux processus fils
    pid_t pid1 = fork();
    if (pid1 < 0) {  // Erreur de fork
        perror("fork()");
        return 1;
    } else if (pid1 == 0) {  // Processus fils 1

        child_process(pipe1, pipe2, image_to_compare_path, path_image, sizeof(path_image), data);
        
    } else {  // Processus père       
        // Créer le deuxième processus fils
        pid_t pid2 = fork();
        if (pid2 < 0) {  // Erreur de fork
            perror("fork()");
            return 1;
        } else if (pid2 == 0) {  // Processus fils 2 

            child_process(pipe2, pipe1, image_to_compare_path, path_image, sizeof(path_image), data);

        } else {  // Processus père
            
            parent_process(pid1, pid2, pipe1, pipe2, path_image, sizeof(path_image), data);
        }
    }

    // Détacher la région de mémoire partagée
    if (munmap(data, sizeof(struct shared_data)) < 0) {
        perror("munmap()");
        exit(1);
    }


    return 0;
}

void child_process(int child_pipe[2], int other_child_pipe[2], char image_to_compare[999], char image_path[999], size_t len_path_image, struct shared_data* data){
    printf("pid1: %i\n", getpid());

        // Fermer les pipes inutiles
        close(child_pipe[WRITE]);
        close(other_child_pipe[READ]);
        close(other_child_pipe[WRITE]);

        // Lire le chemin de l'image à comparer dans le pipe

        while(read(child_pipe[READ], image_path, len_path_image) > 0){   
            printf("pid1: %i\n", getpid());              
            printf("image read on pipe1: %s\n", image_path);           

            // Comparer les images
            
            compare_image(image_to_compare, image_path, data);
            
        }
        // Fermer les pipes inutiles
        close(child_pipe[READ]);

        // Terminer le processus fils 1
        exit(0);
}

void parent_process(pid_t pid1, pid_t pid2, int pipe1[2], int pipe2[2], char image_path[999], size_t len_path_image, struct shared_data* data){
    // Fermer les pipes inutiles
            
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

    int active_child_num = 1;
    int nb_image_process1 = 0;
    int nb_image_process2 = 0;
    close(pipe1[READ]);
    close(pipe2[READ]);
    while (sigint_received == 0 && fgets(image_path, len_path_image, stdin) != NULL){
        // Supprimer le retour à la ligne à la fin du buffer
        size_t len = strlen(image_path);
        
        if (image_path[len - 1] == '\n' || image_path[len - 1] == '\r') {
            image_path[len - 1] = '\0';
        }

        printf("image got on input: %s\n", image_path);
        // vérifier si le fichier existe
        if (access(image_path, F_OK) == -1) {
            printf("Le fichier '%s' n'existe pas.\n", image_path);
            continue;
        }
        // Écrire le chemin de l'image dans le pipe
        printf("active_child_num: %i\n", active_child_num);
        if (active_child_num == 1){
            if(write(pipe1[1], image_path, len_path_image) == 0){
                printf("pipe1 est vide\n");
            }
            active_child_num = 2;
            nb_image_process1++;
        } else {
            
            if (write(pipe2[1], image_path, len_path_image) == 0){
                printf("pipe2 est vide\n");
            }
            active_child_num = 1;
            nb_image_process2++;
        }

    }



    // Fermer les pipes pour terminer les processus fils 
    //(si les pipes sont fermés, les processus fils se terminent)
    close(pipe1[WRITE]);
    close(pipe2[WRITE]);


    // Attendre la fin des processus fils
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    printf("nb_image_process1: %i\n", nb_image_process1);
    printf("nb_image_process2: %i\n", nb_image_process2);
    // Afficher le chemin de l'image avec la distance minimale
    if (data->min_distance_image_path[0] != '\0'){
        printf("Most similar image found: '%s' with a distance of %i.\n", data->min_distance_image_path, data->min_distance);
    } else {
        printf("No similar image found (no comparison could be performed successfully).\n");
    }
}

void compare_image(char path_image1[999], char path_image2[999], struct shared_data* data) {
    int return_value = -1;
    // Utilisation du programme img-dist qui prend en paramètre deux images et qui retourne la distance entre les deux


    // Fork pour exécuter le programme img-dist
    pid_t pid = fork();
    if (pid < 0) {  // Erreur de fork
        perror("fork()");
        exit(1);
    } else if (pid == 0) {  // Processus fils
        

        // Exécution de la commande img-dist avec les deux chemins d'images en paramètres
        char* argument_list[] = {"img-dist", path_image1, path_image2, NULL};
        execvp("img-dist", argument_list);

        // Afficher une erreur si l'exécution de la commande a échoué
        if (return_value < 0) {
            perror("Fils: execlp()");
            exit(1);
        }
    } else {  // Processus père
        // Attendre la fin du processus fils et récupérer sa valeur de retour
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return_value = WEXITSTATUS(status);
            printf("La distance avec l'image '%s' est de : %i\n", path_image2, return_value);
        } else {
            printf("Le fils s'est terminé de manière inattendue\n");
        }
    }

    // Mettre à jour la distance minimale et le chemin de l'image avec la distance minimale
    if (return_value < data->min_distance) {
        data->min_distance = return_value;
        strcpy(data->min_distance_image_path, path_image2);
    }
}

void signal_handler(int signal_number) {
    printf("Signal %i reçu\n", signal_number);
    if (signal_number == SIGINT) {
        printf("Signal SIGINT reçu\n");
        sigint_received = 1;
        //envoie EOF sur stdin
        close(STDIN_FILENO);
    }
    else if(signal_number == SIGPIPE){
        printf("Signal SIGPIPE reçu\n");
        printf("L'un des fils s'est terminé de manière inattendue et a fermé sa lecture des pipes\n C'est un erreur cr");
        exit(1);
    }else{
        printf("Signal reçu inconnu\n");
    }
}