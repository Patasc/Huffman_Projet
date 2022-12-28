//
// Created by - on 30/09/2022.
//

#include <stdio.h>

#ifndef HUFFMAN_PROJECT_HUFFMAN_ECRITURE_H
#define HUFFMAN_PROJECT_HUFFMAN_ECRITURE_H

void Compresser_Fichier(char ** original_filename, char ** compressed_filename);

struct Struct_Distribution_Probabiliste * Distribution_Probabiliste(FILE * file_handle);

void Ecrire_Fichier_Compresse(FILE * file_handle, FILE * new_file_handle, struct Struct_Distribution_Probabiliste * distribution_probabiliste, char * arbre_code);
#endif //HUFFMAN_PROJECT_HUFFMAN_ECRITURE_H
