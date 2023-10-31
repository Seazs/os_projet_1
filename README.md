# Readme

## Description
Projet 1 de Systeme d'exploitation

## Installation

Cloner le projet
```shell
git clone ssh://git@gitlab.ulb.be:30422/alexbien/os-projet-1.git
ou
git clone https://gitlab.ulb.be/alexbien/os-projet-1.git
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

### Droit d'execution
Pour pouvoir avoir les droits d'execution sur les scripts.
```shell
chmod +x ./list-file.sh
chmod +x ./launcher
```

### img-dist
Le programme compare deux images.
(attention le programme renvoie une valeur mais n'imprime rien).

```shell
./img-dist <path_to_image1> <path_to_image2> 
```



### Launch script

Le script permet de lancer img-search avec 2 modes différents.
- **Mode interactif** : Le programme demandera de rentrer 1 à 1 les images à comparer avec l'image entrée en paramètre. Une fois toutes les images rentrées, il suffit d'appuyer sur CTRL+D ou CRTRL+C pour terminer les comparaisons et obtenir l'image la plus proche.
```shell	
./launcher -i <path_to_image> 
```

- **Mode automatique** : Le programme va comparer l'image entrée en paramètre avec toutes les images du dossier d'une base de donnée. Il affichera l'image la plus proche à la fin.
par défaut, le dossier de la base de donnée est ./img
```shell
./launcher -a <path_to_image> 
./launcher -a <path_to_image> <path_to_database>
```
