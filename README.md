# Projet compilateur

[doc x86](doc_x86.pdf)

## Choices

Here we put the non-programming choices we made

- variables can be only constitued of letters, `_` and numbers
- for the moment we can do multiple affectations
- we choose to change the memory_offset and the tmp_index before the actions
- 
- 
- 
- 
- 

## TODO
### Obligatoire
- [ ] Type de données char
- [x] Type de données int
- [x] Variables
- [x] Constantes entières
- [ ] Caractère (avec simple quote)
- [x] Opérations arithmétiques de base : +,-, *
- [x] Opérations logiques bit-à-bit : |, &,ˆ
- [x] Opérations de comparaison : ==, !=, <, >
- [x] Opérations unaires : ! et - et ~ (bonus)
- [x] Déclaration de variables n’importe où
- [ ] Affectation (qui, en C, retourne aussi une valeur)
- [ ] Possibilité d’initialiser une variable lors de sa déclaration : Gaspard & Benjamin
- [ ] Utilisation des fonctions standard putchar et getchar pour les entrées-sorties : James
- [ ] Définition de fonctions avec paramètres, et type de retour int, char ou void
- [ ] Vérification de la cohérence des appels de fonctions et leurs paramètres (noms réservés)
- [ ] Structure de blocs grâce à { et } : Roua
- [ ] Support des portées de variables et du shadowing
- [ ] Les structures de contrôle if, else, while : Mathieu
- [ ] Support du return expression n’importe où
- [ ] Vérification qu’une variable utilisée dans une expression a été déclarée
- [ ] Vérification qu’une variable n’est pas déclarée plusieurs fois
- [ ] Vérification qu’une variable déclarée est utilisée au moins une fois

### Facultatif
- [ ] Division et modulo (dans l'IR) : Bachir
- [ ] Opérateurs d’affectation +=, -= etc., d’incrémentation ++ et décrémentation - -
- [ ] Tableaux (à une dimension)
- [ ] Pointeurs
- [ ] break et continue
- [ ] Les chaînes de caractères représentées par des tableaux de char
- [ ] switch...case
- [ ] Les opérateurs logiques paresseux ||, && : Myriam
- [ ] Reciblage vers plusieurs architectures : x86, MSP430, ARM
- [ ] Propagation de constantes simple
- [ ] Propagation de variables constantes (avec analyse du data-flow)


## Ressources

- see example of [c.g4](https://github.com/antlr/grammars-v4/blob/master/c/C.g4)
- 
- 

![operator priority](operator_priority.png "source - wikipedia")





