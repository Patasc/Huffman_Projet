#include "Huffman_Ecriture.h"
#include "Huffman_Lecture.h"

int main() {
    // Nom des fichiers d'entrées / Sorties
    // On pourrait utiliser les paramètres passés en ligne de commande mais c'est plus rapide ainsi pour relancer
    // Le programme
    char * original_filename = "../res/A.txt";
    char * compressed_filename = "../res/A_COMPRESSED.txt";
    char * uncompressed_filename = "../res/A_UNCOMPRESSED.txt";

    // Dans ce programme de test nous effectuons et la compression et la décompression dans le même fichier puisqu'aucune
    // Raison d'en avoir plusieurs, mais il n'y a aucune tricherie en utilisant le fichier original
    Compresser_Fichier(&original_filename, &compressed_filename);
    Lire_Fichier_Compresse(&compressed_filename, &uncompressed_filename);

    return 0;
}
