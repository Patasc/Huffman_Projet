//
// Created by - on 30/09/2022.
//

#include <stdio.h>
#include <stdlib.h>

#include "Huffman_Common.h"
#include "Huffman_Lecture.h"

void Lire_Fichier_Compresse(char ** compressed_filename, char ** decompressed_filename){
    /*
     * Fonction lisant le contenu d'un fichier compressé
     */
    char * nom_fichier_compresse = * compressed_filename;
    char * nom_fichier_decompresse = * decompressed_filename;

    long long nombre_caracteres;

    int longueur_distribution_probabiliste;
    struct Struct_Distribution_Probabiliste * distribution_probabiliste;
    char * arbre_code;


    long long nombre_caracteres_lus = 0;

    int store;
    short int code[9]; // Max code length + 1, don't bother optimising for a few bytes
    short int end = 0;

    int current_byte_length = 0;
    int offset = 0;

    char current_char;

    FILE * fichier_compresse = fopen(nom_fichier_compresse, "rb");
    FILE * fichier_decompresse = fopen(nom_fichier_decompresse,"w");

    // Vérification habituelle de la bonne ouverture du fichier
    if (fichier_compresse == NULL || fichier_decompresse == NULL){
        printf("Compresser_Fichier : Impossible d'ouvrir le fichier\n");
        exit(1);
    }

    // On lit les métadonnées stockés dans le fichier
    fread(&nombre_caracteres, sizeof(long long), 1, fichier_compresse);

    fread(&longueur_distribution_probabiliste, sizeof(int), 1, fichier_compresse);

    distribution_probabiliste = (struct Struct_Distribution_Probabiliste * ) malloc(sizeof(struct Struct_Distribution_Probabiliste) * longueur_distribution_probabiliste + 2); // TODO : CHECK IF THIS HELPS
    fread(distribution_probabiliste, sizeof(struct Struct_Distribution_Probabiliste), longueur_distribution_probabiliste, fichier_compresse);

    arbre_code = Creation_Arbre(distribution_probabiliste);

    // ---------

    // Boucle continuant jusqu'à ce qu'on est lu le nombre de caractère attendus (Simplifie certains calculs, plutôt que
    // De lire jusqu'à la fin du fichier, ou un code particulier)
    while (!end) {
        // On lit un octet
        fread(&store, sizeof(char), 1, fichier_compresse);

        // On le décompose ensuite en un tableau contenant 8 bit, ses composantes
        for (int i = 0; i < 8; i++) {
            current_byte_length++;

            // On ajoute à la fin du tableau le bit, suivi d'un 2 pour indiquer la fin du code
            // Cette monstruosité isole d'abord le bit qui nous intéresse avec store & (1 << (7 - i)), avant de le
            // Décaler en bit de poids faible (tout à droite) en utilisant >>> 7 - i
            code[i + offset] = ((store & (1 << (7 - i))) >> (7 - i));
            code[i + 1 + offset] = 2;

            // Si en utilisant l'arbre, le code est différent de '\0', alors on le code complet pour récupérer un
            // Caractère,
            if ((current_char = caractere_depuis_code(code, arbre_code)) != '\0') {
                // Si on a lu tout les caractères, on quitte les boucles, puisque finir de parcourir l'octet risque de
                // Ajouter un faux caractère à la fin
                if (nombre_caracteres == nombre_caracteres_lus) {
                    i = 8;
                    end = 1;
                }

                // Sinon on incrémente le nombre de caractères lus,
                // On modifie offset pour ajuster l'écriture dans code,
                // Puisque l'on utilise i, si un code se fini en milieu de octet, il faut écrire le code suivant à
                // Partir de 0, et non de 4 par exemple
                else {
                    nombre_caracteres_lus++;
                    offset -= current_byte_length;

                    // On écrit le caractère déchiffré
                    fputc(current_char, fichier_decompresse);

                    current_byte_length = 0;

                }
            }
        }

        // Si un code n'a pas été entièrement écrit, et que l'on lit l'octet suivant, il nous continuer à écrire le
        // Code à aprtir du bon endroit, et non de 0 (puisque i = 0 lorsque la boucle reprendra)
        offset = current_byte_length;

    }

    // Normalement une erreur aurait eu lieu avant si on à pas n / n
    printf("Nombre de caracteres lus : %lld sur %lld\n", nombre_caracteres_lus, nombre_caracteres);

    fclose(fichier_compresse);
    fclose(fichier_decompresse);
}