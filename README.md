# Readme

## Description
Projet 1 de Systeme d'exploitation

## Installation

Pour pouvoir avoir les droits d'execution sur le script
```shell
chmod +x ./list-file.sh
```
## Compilation

### Compiler img-dist
```shell
cd ./img-dist
make all
``` 

### Compiler img-search
```shell
make all
```
(depuis le dossier root du projet)

## Utilisation

### img-dist
Le programme compare deux images.
(attention le programme renvoie une valeur mais n'imprime rien)

```shell
./img-dist <path_to_image1> <path_to_image2> 
```

### img-search
Le programme compare une image prise en paramètre, avec les images entrée en commande par la suite. Il affiche le score de chaque image comparée.
```shell	
./img-search <path_to_image> 
```
