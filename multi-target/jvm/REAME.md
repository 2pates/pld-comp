Compilation vers jeu d'instructions JVM
=======================================

# Dépendances

Compilateur java: `javac` (inclus dans votre JDK favori)

Assembleur java: `jasmin`

    wget https://sourceforge.net/projects/jasmin/files/latest/download -O jasmin.zip
    unzip jasmin.zip
    cd jasmin-2.4 ; ./build.sh
    rm jasmin.zip

Désassebleur java: `tinapoc`

    mkdir tinapoc ; cd tinapoc
    wget https://sourceforge.net/projects/tinapoc/files/latest/download -O tinapoc.zip
    unzip tinapoc.zip ; rm tinapoc.zip

# Principe

Les programmes qu'exécute la JVM sont stockés dans des fichiers `.class` sous forme de "bytecode", une représentation binaire du code exécutable. Traditionnellement, c'est le rôle du compilateur `javac` de traduire le code source java vers le bytecode. Mais dans ce PLD, on va générer du bytecode en partant d'un programme en C.

Comme pour les autres cibles, on va découper la traduction `.c` -> `.class` en distanguant l'étape de compilation proprement dite et l'étape d'assemblage.

Vous ferez l'étape d'assemblage avec l'outil "Jasmin" dont voici la description:

> Jasmin is an assembler for the Java Virtual Machine. It takes ASCII descriptions of Java classes, written in a simple assembler-like syntax using the Java Virtual Machine instruction set. It converts them into binary Java class files, suitable for loading by a Java runtime system.

L'objectif de votre compilateur sera ainsi de produire des fichiers `.j` en syntaxe jasmin:

    `.c` --> votre compilateur --> `.j`

Jasmin est disponible via sourceforge ou via github: http://jasmin.sourceforge.net/ ou https://github.com/davidar/jasmin


# Quelle implem de référence pour la traduction `.c` -> `.j` ?

Comme pour les autres cibles, vous aurez probablement envie de vous inspirer du code généré par un compilateur existant. Nous n'avons pas trouvé pour cela de solution "directe" ; mais vous pouvez adopter l'approche suivante:

     `.c` --> traduction manuelle --> `.java` --> javac --> `.class` --> dejasmin --> `.j`

En gros, il s'agit de "ré-écrire" le programme d'entrée en java, puis de le compiler avec la toolchain classique, et enfin de le désassembler, pour obtenir du code lisible à l'oeil nu. Vous pouvez faire la ré-écriture du code source soit manuellement, soit en passant (manuellement) par un site web tel que `www.mtsystems.com`. Le fichier `C-to-java/Example.java` est directement obtenu via ce site web, à partir 

Remarque: le fichier `.class` obtenu est exécutable, et aura la même sémantique que le fichier `.c` de départ.

L'outil `dejasmin` mentionné ci-dessus est un "désassembleur", c'est à dire un programme qui lit du bytecode et qui produit un fichier en syntaxe jasmin. il est disponible là: http://tinapoc.sourceforge.net



