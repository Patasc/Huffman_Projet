CODE :

	Un code de caractère est stocké dans un tableau d'une longueur d'au plus de 8 (+ 1 pour le caractère de fin)
	Puisque la taille est variable, il y a un caractère pour indiquer la fin, qui est '2'
	Le tableau stocke des '0' et des '1' pour indiquer leurs bits respectifs
	
	Un tableau est utilisé plutôt que directement les octets pour simplifier leur manipulation, nottament lors de l'écriture
		Quand il faut isoler les bits individuels, cette méthode simplifie les calculs nécessaire mais prends plus d'espace

ARBRE :
	
	L'arbre d'encodage est un arbre à priori parfait (sauf une exception non abordé), un fils droit indique un 1, un fils gauche un 0
	Tout noeud ne correspondant pas à un caractère est égal à '\0', normalement aucune feuille est égale à '\0'


STRUCTURE DISTRIBUTION PROBABILISTE :
	
	Cette structure associe chaque caractère dans le fichier avec son pourcentage d'apparition dans le fichier, son nombre d'apparition est
	Stocké plutôt que probabilité (flottants / double) en raison des erreurs d'arrondissement avec de petits nombres, causant des soucis de
	Lecture


CODE DE SORTIES :
	
	1	Impossibilité de lire / écrire dans un fichier
	2	Supprimé donc vide entre 1 et 3
	3	Caractère non trouvé dans l'arbre