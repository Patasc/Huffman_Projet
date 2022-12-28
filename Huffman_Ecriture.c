//
// Created by - on 30/09/2022.
//
/*
 * Ce fichier comprend toutes les fonctions utilisées dans la compression d'un fichier, et non commune à la décompression
 */

#include <stdio.h>
#include <stdlib.h>

#include "Huffman_Common.h"
#include "Huffman_Ecriture.h"


void Compresser_Fichier(char ** original_filename, char ** compressed_filename){
    /*
     * Fonction s'assurant de l'initialisation pour la bonne compression du fichier, et sert de point d'entrée
     */
    char * nom_fichier_originel = * original_filename;
    char * nom_fichier_compresse = * compressed_filename;

    struct Struct_Distribution_Probabiliste * distribution_probabiliste;

    // On s'assure de la bonne ouverture des deux fichiers, sinon on signale une erreur
    FILE * fichier_originel = fopen(nom_fichier_originel, "r");
    FILE * fichier_compresse = fopen(nom_fichier_compresse,"wb");

    if (fichier_originel == NULL || fichier_compresse == NULL){
        printf("Compresser_Fichier : Impossible d'ouvrir le fichier\n");
        exit(1);
    }

    // On crée la distribution probabiliste à partir du fichier, puis l'arbre,
    // Et à partir de ces données on écrit le fichier compressé en appelant Ecrire_Fichier_Compresse
    distribution_probabiliste = Distribution_Probabiliste(fichier_originel);
    Tri_Tableau_Distribution_Probabiliste(&distribution_probabiliste);

    char * arbre_code = Creation_Arbre(distribution_probabiliste);

    Ecrire_Fichier_Compresse(fichier_originel, fichier_compresse, distribution_probabiliste, arbre_code);

    // Toujours fermer ses fichiers, nous codons proprement ici
    fclose(fichier_originel);
    fclose(fichier_compresse);
}

struct Struct_Distribution_Probabiliste * Distribution_Probabiliste(FILE * file_handle){
    /*
     * À partir du contenu d'un fichier, créer une structure Struct_Distribution_Probabiliste
     * Pas en commun avec la décompression puisque l'arbre suffira, il s'agit juste d'une étape intermédiaire
     * Pour la création de l'arbre
     */
    // On stocke le nombre d'occurence dans un tableau long de 256 octets, pour stocker tout les caractères possibles
    // Et impossibles pour ceux > 127 en ASCII standard, mais permet d'également compresser tout type de fichier,
    // Donc fichier exécutable, le taux de compression est mauvais puisque non adapté mais possible
    long long occurrence_chaque_char[256] = {};
    int nombre_caracteres = 0;
    int store_current_read;

    int nombre_caracteres_differents = 0;
    struct Struct_Distribution_Probabiliste * distribution_probabiliste;

    int compteur_calcul_proba = 0;

    // On compte le nombre de caractères total, ainsi que le nombre d'occurence de chaque caractères
    while ( (store_current_read = fgetc(file_handle)) != -1){
        occurrence_chaque_char[store_current_read] += 1;
        nombre_caracteres++;
    }

    // On compte ensuite le nombre de caractères différents, pour créer un tableau de la bonne taille
    for (int i = 0; i < 256; i++){
        if (occurrence_chaque_char[i] != 0){
            nombre_caracteres_differents++;
        }
    }

    // En fonction du nombre de caractères différents, on créer un tableau de Struct_Distribution_Probabiliste de la bonne taille
    distribution_probabiliste = (struct Struct_Distribution_Probabiliste *) malloc(sizeof(struct Struct_Distribution_Probabiliste) * (nombre_caracteres_differents + 1));
    // A la fin du tableau, on créer une structure distincte pour indiquer la fin
    distribution_probabiliste[nombre_caracteres_differents] = (struct Struct_Distribution_Probabiliste) {'\0', 0};

    // Pour chaque caractère on créer une structure, et pour la probabilite on met juste le nombre d'occurence
    // (Auparavant fait avec flottants / double mais rapidemment les erreurs d'arondissements peuvent devenir problématique)
    for (int i = 0; i < 256; i++){
        if (occurrence_chaque_char[i] != 0){
            distribution_probabiliste[compteur_calcul_proba] = (struct Struct_Distribution_Probabiliste) {(char) i, occurrence_chaque_char[i]};
            // Calcule l'index dans le tableau
            compteur_calcul_proba++;
        }
    }

    // Puisque nous avons parcouru le fichier, on pense au suivant et remet la tête de lecture en début de fichier
    fseek(file_handle, 0, SEEK_SET);

    return distribution_probabiliste;
}

long long File_Size(FILE * file_handle){
    // Renvoie la longueur d'un fichier, possible d'utiliser une autre technique :
    // fseek(file_handle, 0, SEEK_END); int n = ftell(file_handle);fseek(file_handle, 0, SEEK_SET);
    // Mais impropre et ne marche pas pour des fichiers non textes
    long long file_size = 0;

    while (fgetc(file_handle) != -1) file_size++;

    fseek(file_handle, 0, SEEK_SET);
    return file_size;
}

void Ecrire_Fichier_Compresse(FILE * old_file_handle, FILE * new_file_handle, struct Struct_Distribution_Probabiliste * distribution_probabiliste, char * arbre_code){
    /*
     * Fonction écrivant dans un fichier, à partir d'un autre fichier, sa version compressé, en utilisant diverses
     * Données fournis et calculés auparavant
     */
    long long nombre_caracteres;
    int longueur_distribution_probabiliste = Longueur_Distribution_Probabiliste(distribution_probabiliste) + 1; // Fonction ne compte pas la struct nulle de fin

    int stockage_caractere;
    int longueur_code = 0;
    int longueur_arbre = Longueur_Tableau_Code_Depuis_Longueur_Distribution_Probabiliste(longueur_distribution_probabiliste - 2);
    int longueur_arbre_code = longueur_arbre;

    char octet_actuel = 0;
    short int longueur_octet_actuel = 0;
    int longueur_code_actuel;
    short int * code_caractere;

    // Par prudence, on se remet en début de fichier
    fseek(old_file_handle, 0, SEEK_SET);
    nombre_caracteres = File_Size(old_file_handle);

    // On écrit les "métadonnées" du fichier ici, avant d'écrire les données
    fwrite(&nombre_caracteres, sizeof(long long), 1, new_file_handle);

    fwrite(&longueur_distribution_probabiliste, sizeof(int), 1, new_file_handle);
    fwrite(distribution_probabiliste, sizeof(struct Struct_Distribution_Probabiliste), longueur_distribution_probabiliste, new_file_handle);

    // On récupère la profondeur de l'arbre, pour la longueur d'un code maximum
    while (longueur_arbre_code > 0){
        longueur_arbre_code = longueur_arbre_code >> 1;
        longueur_code++;
    }

    // Tableau contenant le code d'un caractère, travail avec les listes pour une manipulation plus simple
    code_caractere = (short int *) malloc(sizeof(short int) * longueur_code);

    // Tant que la fin du fichier n'a pas été atteint
    // La boucle while parcourt chaque caractère du fichier
    while ((stockage_caractere = fgetc(old_file_handle)) != -1) {
        longueur_code_actuel = code_depuis_caractere(&code_caractere, (char) stockage_caractere, &arbre_code, longueur_arbre);

        // La boucle intérieur, for, parcourt chaque bit du code du caractère
        for (int i = 0; i < longueur_code_actuel; i++) {
            // Si pour l'instant on a pas 'rempli' l'octet actuel, on le met juste à droite du dernier bit placé,
            // Et on continue
            if (longueur_octet_actuel + 1 <= 8) {
                octet_actuel = octet_actuel + (char) (code_caractere[i] << (7 - longueur_octet_actuel));

                longueur_octet_actuel++;
                // Si l'octet sur lequel on travaille, est plein, alors on l'écrit dans le fichier, et on reprend
                // L'écriture du caractère
            } else {
                longueur_octet_actuel = 0;

                fwrite(&octet_actuel, sizeof(char), 1, new_file_handle);
                octet_actuel = 0;
                // i - 1 puisque l'octet est déjà plein lorsque l'on atteint cette boucle, donc le bit n'a pas été écris
                // Dans un octet
                i--;
            }
        }
    }

    // A la fin on écrit l'octet courant dans un fichier, puisque vu qu'il n'est peut être pas plein, il y a des bits
    // Non écrit dans le fichier
    fwrite(&octet_actuel, sizeof(char), 1, new_file_handle);
}