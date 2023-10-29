#!/bin/bash

# Vérifie si le nombre d'arguments est correct
if [ "$#" -ne 1 ]; then
    echo "Missing directory name." >&2
    exit 1
fi

# Vérifie si le chemin spécifié est un dossier
if [ ! -d "$1" ]; then
    echo "The specified path is not a directory." >&2
    exit 2
fi

# Liste les fichiers dans le dossier spécifié (ignorant les sous-dossiers)
for file in $(ls -v1 "$1" 2>/dev/null); do
        echo "$1$file"
done

# Fin du script