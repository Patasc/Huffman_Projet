//
// Created by - on 30/09/2022.
//
// Voir le .c pour explications

#ifndef HUFFMAN_PROJECT_HUFFMAN_COMMON_H
#define HUFFMAN_PROJECT_HUFFMAN_COMMON_H

struct Struct_Distribution_Probabiliste {
    char store_char;
    long long probabilite_char;
};

char * Creation_Arbre(struct Struct_Distribution_Probabiliste * distribution_probabiliste);
void Creation_Arbre_Code(struct Struct_Distribution_Probabiliste * distribution_probabiliste, char ** arbre_codage ,int debut_distribution_probabiliste, int fin_distribution_probabiliste, int index_arbre);
int Longueur_Distribution_Probabiliste(struct Struct_Distribution_Probabiliste * distribution_probabiliste);
int Longueur_Tableau_Code_Depuis_Longueur_Distribution_Probabiliste(int longueur_distribution_probabiliste);
void Tri_Tableau_Distribution_Probabiliste(struct Struct_Distribution_Probabiliste ** distribution_probabiliste);

int code_depuis_caractere(short int ** code, char caractere_demande, char ** arbre_code, int longueur_arbre_code);
char caractere_depuis_code(const short int * code, char * arbre_code);

#endif //HUFFMAN_PROJECT_HUFFMAN_COMMON_H
