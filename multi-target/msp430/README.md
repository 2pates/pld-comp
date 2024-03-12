Compilation vers jeu d'instructions MSP430
==========================================

# Dépendances

Compilateur pour MSP430: `mspgcc` 
   version 4.6 (i.e. la "vieille" version de 2012) telle que installée sur les machines du département.

   d'autres versions plus récentes de gcc pour MSP430 sont disponibles, mais sont nettement plus compliquées à utiliser.

Émulateur: `mspdebug`
    n'importe quelle version


Installation sous ubuntu:

    sudo apt install binutils-msp430 gcc-msp430 msp430-libc msp430mcu mspdebug

Installation sous macosx (macports): 

    sudo port install msp430-gcc msp430-libc msp430mcu mspdebug

Note: tous ces outils sont déjà installés sur les machines du département. La version de mspgcc est buggée (on a un un warning désagréable du style `memory region REGION_FAR_TEXT not declared`) mais ce n'est pas gênant pour le PLD.


# Exécution

Le binaire est interprété instruction par instruction dans `mspdebug` (cf makefile). Par souci de simplicité, j'exécute froidement 10000 instructions, en supposant que ce sera suffisant pour tomber dans la boucle infinie qui suit le `main` (cf objdump-msp430.txt). On peut augmenter ce chiffre si c'est insuffisant.

L'affichage du programme (sur stdout) est capté à l'aide d'un port série virtuel (cf commande "simio" dans le makefile, et cf fichier `bsp.c`)

Le code de retour est lu directement dans R15 après la fin du programme.

# Liens utiles

jeu d'instruction msp430: 
https://www.ti.com/sc/docs/products/micro/msp430/userguid/as_5.pdf

documentation mspgcc (version 3):
http://mspgcc.sourceforge.net/manual/

documentation mspdebug:
https://www.dlbeer.co.nz/mspdebug/index.html
