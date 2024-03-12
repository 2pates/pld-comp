Reciblage du backend
====================

demo: dans chaque répertoire, lire le README pour installer les pré-requis. Puis `make run`

# Intro

Objectif: valider le volet "reciblage" du PLD comp

Approche: compiler un même programme vers différentes cibles, et comparer les résultats à l'exécution

Note: par "résultats" on entend ici "affichage sur stdout" et "code de retour" du processus. On utilise le programme `example.c` sur toutes les cibles, en vérifiant que le comportement à l'exécution soit le même.

Les cibles supportées pour l'instant (juin 2019) sont : x86, ARM, MSP430, JVM

D'autres cibles envisagées sont: WebAssembly, Z80 (suggestions bienvenues)

