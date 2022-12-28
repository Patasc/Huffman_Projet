//
// Created by - on 30/09/2022.
//

/*
 * Ce fichier contient toutes les fonctions communes à la compression et décompression, et est donc inclus par les deux
 */

#include <stdio.h>
#include <malloc.h>

#include "Huffman_Common.h"

int Longueur_Distribution_Probabiliste(struct Struct_Distribution_Probabiliste * distribution_probabiliste){
    // Pour un objet de structure Struct_Distribut... renvoie son nombre d'élément, -1 puisque le caractère de fin de
    // Structure '\0' n'est pas compté
    int i = -1;

    // On regarde store_char mais on aurait pu également regarder probabilite_char
    while (distribution_probabiliste[++i].store_char != '\0');

    return i;
}

char caractere_depuis_code(const short int * code, char * arbre_code){
    // Pour un code donné et l'arbre d'encodage, renvoie le caractère correspondant au code
    int i = 0;
    int Huffman_Array_Index = 0;

    // Le code est un tableau comprenant soit des 0, soit 1 pour désigner les bits individuels respectifs, et 2 permet
    // D'identifier la fin du tableau (Puisque la longueur n'est pas fixe)
    while (code[i] != 2) {
        // En fonction du bit la position dans l'arbre change, et dépend de la position précédente
        Huffman_Array_Index = Huffman_Array_Index * 2 + 1 + (code[i]);
        i++;
    }

    return arbre_code[Huffman_Array_Index];
}

int Longueur_Tableau_Code_Depuis_Longueur_Distribution_Probabiliste(int longueur_distribution_probabiliste){
    /*
     * Cette petite fonction permet de, à partir de la longueur de la distribution probabiliste,
     * De déterminer la taille du tableau, supposant un arbre complet
     * Utile pour le malloc lorsqu'il faut stocker l'arbre
     */
    int longueur_tableau_arbre = 1;

    // Puisqu'il s'agit d'un arbre on travaille avec des puissances de deux, d'où le << 1 (bitshift)
    // On cherche juste la plus petite puissance de 2 supérieure à la longueur de distribution_probab...
    while(longueur_tableau_arbre < longueur_distribution_probabiliste){
        longueur_tableau_arbre = longueur_tableau_arbre << 1;
    }

    // Un de plus parce que l'on s'arrête un trop tôt sinon, do_while aurait pu résoudre le problème
    return (longueur_tableau_arbre << 1);
}

int code_depuis_caractere(short int ** code, char caractere_demande, char ** arbre_code, int longueur_arbre_code){
    /*
     * Pour un caractère donné renvoie le code et sa longueur
     * Le code est renvoyé au moyen d'un paramètre de sortie, 'code'
     */
    int code_index = -1;

    // En partant de la fin de l'arbre, en le parcourant à l'envers on cherche à trouver l'index du caractère demandé
    // Parcouru à l'envers pour utiliser une variable de moins
    while (longueur_arbre_code >= 0 && (* arbre_code)[longueur_arbre_code] != caractere_demande) {
        longueur_arbre_code--;
    }

    // Puisqu'il est parcouru à l'envers, si on obtient un indexe inférieur à zéro alors forcément le caractère
    // Demandé n'est pas l'arbre, normalement impossible mais il vaut mieux être prudent
    if (longueur_arbre_code < 0 && (* arbre_code)[longueur_arbre_code] != caractere_demande) {
        fprintf(stderr, "get_code_from_char : Requested character not found in tree !");
        exit(3);
    }

    // La longueur du tableau contenant le code est égal à la plus petite puissance de 2 supérieur à l'indexe du code
    // Le code correspondant à l'indexe du caractère dans l'arbre
    // +1 pour ajouter le caractère de fin de tableau, 2, puisqu'il n'ont pas une taille fixe
    int code_array_len = longueur_arbre_code + 1;
    while (code_array_len > 0) {
        code_array_len = code_array_len >> 1;
        code_index++;
    }

    // Affectation du caractère de fin, et stockage de la longueur du code pour pouvoir le renvoyer ensuite,
    // Puisqu'on utilise (et modifie) la variable juste après
    (*code)[code_index--] = 2;

    int code_len = code_index + 1;

    // Si la longueur de l'arbre est pair, alors nous sommes un fils gauche, et si impair alors un fils droit
    // Droit -> 1, Gauche -> 0      , & 1 permet de récupérer le dernier bit et donc la parité
    // Cas d'arrêt quand longueur_arbre_code == 0, autrement dit que nous sommes à la racine
    while (longueur_arbre_code) {
        if (longueur_arbre_code & 1) {
            (*code)[code_index--] = 0;
        } else {
            (*code)[code_index--] = 1;
        }

        // -1 et ensuite divisé par 2 pour récupérer le père, et donc de monter dans l'arbre
        longueur_arbre_code = (longueur_arbre_code - 1) / 2;
    }

    return code_len;
}

char * Creation_Arbre(struct Struct_Distribution_Probabiliste * distribution_probabiliste){
    /*
     * Fonction créant l'arbre d'encodage à partir d'une structure de distribution probabiliste
     * Cette fonction s'occupe de l'initialisation, et fait appel à Creation_Arbre_Code qui fait le vrai travail
     */
    int longueur_distribution_probabiliste;
    int longueur_tableau_arbre;

    // ????
    // Possible source d'un bug à cause du -1 pour ne pas compter la structure nulle de fin de structure dans l'arbre
    // Et dans l'idée ne pas créer un niveau de trop, ce qui doublerait la taille de l'arbre
    longueur_distribution_probabiliste = Longueur_Distribution_Probabiliste(distribution_probabiliste) - 1; // TODO: FIX WHATEVER THIS'LL BREAK

    longueur_tableau_arbre = Longueur_Tableau_Code_Depuis_Longueur_Distribution_Probabiliste(longueur_distribution_probabiliste);

    // arbre_codage contient l'arbre d'encodage, chaque noeud est un char pour stocker le caractère qu'ils représentent
    char * arbre_codage = (char * ) malloc(sizeof(char) * (longueur_tableau_arbre));

    // Pour l'instant on ne sait pas où va où, donc on met tout à '\0' qui correspond au caractère de fin d'arbre,
    // Simplifiant ainsi la vie pour correctement indiquer si un noeud est un de fin ou non
    for (int i = 0; i < longueur_tableau_arbre; i++) {
        arbre_codage[i] = '\0';
    }

    Creation_Arbre_Code(distribution_probabiliste, &arbre_codage, 0, longueur_distribution_probabiliste, 0);

    // On renvoie en réalité un pointeur, aucun déférencement nécessaire puisqu'il s'agit d'un tableau
    return arbre_codage;
}

void Tri_Tableau_Distribution_Probabiliste(struct Struct_Distribution_Probabiliste ** distribution_probabiliste){
    /*
     * Cette fonction permet de trier un objet Struct_Distribution_Probabiliste de telle manière à trier en ordre
     * Décroissant de probabilité, permet d'affecter les encodages sans avoir à chercher le caractère le plus probable
     * A chaque fois
     * Le tri bulle est utilisé, éventuellement remplacer par un tri en log(n)n
     */

    // Pas commenté parce que même si nom à rallonge, il s'agit d'un tri bulle parfaitement classique
    int i = 0;
    struct Struct_Distribution_Probabiliste * dereferencement_distribution_probabiliste = * distribution_probabiliste;

    while (dereferencement_distribution_probabiliste[i].store_char != '\0') {
        int a = i + 1;

        while (dereferencement_distribution_probabiliste[a].store_char != '\0') {
            if (dereferencement_distribution_probabiliste[a].probabilite_char > dereferencement_distribution_probabiliste[i].probabilite_char) {
                struct Struct_Distribution_Probabiliste stockage_structure = dereferencement_distribution_probabiliste[a];
                dereferencement_distribution_probabiliste[a] = dereferencement_distribution_probabiliste[i];
                dereferencement_distribution_probabiliste[i] = stockage_structure;
            }
            a++;
        }

        i++;
    }

}

void Creation_Arbre_Code(struct Struct_Distribution_Probabiliste * distribution_probabiliste, char ** arbre_codage ,int debut_distribution_probabiliste, int fin_distribution_probabiliste, int index_arbre){
    /*
     * Fonction récursive créant l'arbre d'encodage à partir de la distribution probabiliste, le tableau où stocker
     * L'arbre, et puisqu'il s'agit d'une fonction récursive l'index de début et fin du tableau (ou zone de travail)
     * Ressemble au tri dichotomique
     */

    // Ces trois variables permettent de déterminer si nous avons atteint le cas d'arrêt (longueur_intervalle_travail),
    // Ou les nouvelles zones de travail
    int longueur_intervalle_travail = fin_distribution_probabiliste - debut_distribution_probabiliste + 1;

    int moitie_intervalle = (longueur_intervalle_travail - 1) / 2;
    // Parite nécessaire pour les chiffres impair, puisque la division d'un entier impair renvoie le résultat arrondi
    // A l'entier le plus proche inférieur
    int parite_moitie_intervalle = moitie_intervalle & 1;


    // Cas d'arrêt, nous n'avons qu'un seul élément à traiter, on va donc y stocker le caractère puisqu'il a trouvé
    // Sa place
    if (longueur_intervalle_travail == 1){
        (* arbre_codage)[index_arbre] = (distribution_probabiliste[debut_distribution_probabiliste]).store_char;
    }

    // Second cas d'arrêt, distinct du premier pour traiter les cas où un père à exactement 1 ou 2 descendants
    // (Si plus alors on appelle la fonction à nouveau)
    // Donc même chose qu'avant, mais pour 2 éléments
    else if (longueur_intervalle_travail == 2){
        index_arbre = index_arbre * 2 + 1;

        (* arbre_codage)[index_arbre] = distribution_probabiliste[debut_distribution_probabiliste].store_char;
        (* arbre_codage)[index_arbre + 1] = distribution_probabiliste[fin_distribution_probabiliste].store_char;
    }

    // Sinon on appelle la fonction à nouveau, les calculs sont grossiers mais sont les mêmes que pour la dichotomie,
    // On prend juste en compte la parité pour s'assurer de ne pas donner un index aux deux appels
    // ...a.......c -> ...a...(b-1)(b)...c
    // b calculer en faisant a + (c - a) + la parité pour prendre en compte les longueurs impaires
    else {
        Creation_Arbre_Code(distribution_probabiliste, arbre_codage, debut_distribution_probabiliste + moitie_intervalle + parite_moitie_intervalle, fin_distribution_probabiliste, 2 * index_arbre + 2);
        Creation_Arbre_Code(distribution_probabiliste, arbre_codage, debut_distribution_probabiliste, debut_distribution_probabiliste + moitie_intervalle - 1 + parite_moitie_intervalle, 2 * index_arbre + 1);
    }
}
