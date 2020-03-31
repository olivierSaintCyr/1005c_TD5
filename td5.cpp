/*************************************************************************//**
*  Le TD5 final d'INF1005C, hiver 2020.
*  Programme pour lire, découper et afficher des images TGA.
*  \file    td5.cpp
*  \author  Nom, Prénom (matricule)
*  \author  Nom, Prénom (matricule)
*  \date    Avril 2020
*  \note    Basé sur un code incomplet fourni par Francois-R.Boyer@PolyMtl.ca
*****************************************************************************/
//TODO: Mettre les bons noms dans l'entête du fichier. \file

#pragma region "Includes"//{
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <ciso646>
#include <cstddef>
#include <cstdint>  // Pour les types de taille certaine uint8_t, uint16_t...
#include <clocale>
#include "debogageMemoire.h"
#include "cppitertools/range.hpp"
#include "gsl/span"

using namespace std;
using namespace iter;
using namespace gsl;
#pragma endregion//}

#pragma region "Définitions de types"//{

// Sur notre système les types déclarés par cstdint sont équivalents à:
// typedef unsigned char   uint8_t;  // Un octet non signé (0 à 255).
// typedef unsigned short  uint16_t; // Un mot de 16 bits non signé (0 à 65535).

/// Structure pour l'entête d'un fichier TGA.
/// \note La structure est simplifiée; le programme ne supporte que les images en teintes de gris.
struct EnteteTGA {
	uint8_t  tailleId;          ///< = 0  (pas d’identifiant).
	uint8_t  typePalette;       ///< = 0  (pas de palette).
	uint8_t  typeImage;         ///< = 3  (teintes de gris).
	uint8_t  inutilise[9];      ///< Tous à 0. Inutilisés (voir la note).
	uint16_t largeur;           ///< Largeur de l'image en pixels.
	uint16_t hauteur;           ///< Hauteur de l'image en pixels.
	uint8_t  nbBits;            ///< = 8  (chaque pixel est sur un octet).
	uint8_t  descripteurImage;  ///< = 0  (coin inférieur gauche en premier, pas d’alpha).
};

/// Structure pour une ligne d'une image.
struct LigneImage {
	int      debut;         ///< Position horizontale où commence la ligne.
	int      longueur;      ///< Longueur du tableau \a intensites .
	uint8_t* intensites;    ///< Tableau des intensités de pixels de la ligne.
};

/// Structure pour une image.
struct Image {
	int         largeur;    ///< Largeur de l'image en pixels.
	int         hauteur;    ///< Hauteur de l'image en pixels; nombre d'éléments dans le tableau \a lignes .
	LigneImage* lignes;     ///< Tableau des lignes de l'image.
};
#pragma endregion//}

//TODO: Ajouter les fonctions nécessaires pour le principe DRY et la lisibilité.  Vous n'avez pas à écrire de commentaires d'entête pour ces fonctions ajoutées.

/*************************************************************************//**
*  Affiche un caractère gris d'une certaine intensité à la console.
*  Version très simple avec peu de niveaux de gris vraiment différents.
*  \param  intensite  L'intensité, entre 0 et 255.
*/
void afficherGris(int intensite)
{
	static const int nNiveauxEntree = 256;
	if (intensite < 0)
		intensite = 0;
	else if (intensite > nNiveauxEntree-1)
		intensite = nNiveauxEntree-1;
	static const string niveauxDeGris = " .:-=+*#%@";
	cout << niveauxDeGris[intensite * niveauxDeGris.size() / nNiveauxEntree];
}

/*************************************************************************//**
*  Désalloue les structures associées à l'image.
*  \param [in,out]  image  L'image à désallouer.
*/
void desallouerImage(Image& image)
{
	//TODO: Si le pointeur n'est pas nullptr, désallouer tout ce qui est pointé directement/indirectement par la structure Image, puis mettre le pointeur à nullptr.
}

/*************************************************************************//**
*  Charge une image d'un fichier TGA.
*  \param [in,out]  image     L'image qui sera remplacée par celle chargée.
*  \param           nomImage  Nom du fichier TGA.
*  \return  \c true si l'image s'est bien chargée.
*/
bool chargerImage(Image& image, const string& nomImage)
{
	EnteteTGA entete;
	//TODO: Ouvrir le fichier et charger le début dans la structure 'entete'.  Si le fichier ne s'ouvre pas ou la lecture échoue, retourner faux.

	// On vérifie que le fichier correspond au format simple qu'on supporte.
	if (entete.tailleId != 0
		|| entete.typePalette != 0
		|| entete.typeImage != 3
		|| entete.nbBits != 8
		|| entete.descripteurImage != 0
		|| entete.inutilise[0] != 0) {
		cout << "Mauvais format de fichier." << endl;
		return false;
	}
	// On désalloue avant de charger une image, s'il y en avait déjà une avant.
	desallouerImage(image);
	image.largeur = entete.largeur;
	image.hauteur = entete.hauteur;

	//TODO: Allouer le tableau pour les lignes.
	for (int i : range(entete.hauteur - 1, -1, -1)) {
		//TODO: Mettre le début de ligne à 0 et sa longueur à entete.largeur .
		//TODO: Allouer le tableau pour les intensités des pixels de la ligne.
		//TODO: Lire les intensités pour une ligne, à partir du fichier, vers le tableau alloué ci-dessus.
	}
	return true;
}

//TODO: Écrire l'entête de fonction.
void decouperVide(Image& image)
{
	//TODO: Pour chaque ligne de l'image où un tableau d'intensité est alloué...
	//TODO:   Si la ligne a uniquement des pixels vides, désallouer, mettre le pointeur à nullptr et la longueur à zéro.
	//TODO:   Sinon, si la ligne a des pixels vides au début et/ou à la fin, allouer un nouveau tableau plus petit, y copier les intensités qu'il faut conserver, et ajuster le debut et la longueur de la ligne.  Ensuite désallouer l'ancien tableau et le remplacer par le nouveau.
}

//TODO: Écrire la fonction tailleImage.

//TODO: Écrire la fonction afficherImage.
//TODO: S'assurer que la boucle interne de afficherImage n'a pas trois déréférences; de préférence elle devrait utiliser une boucle sur intervalles qui fait une seule déréférence implicitement.

/*************************************************************************//**
*  Fonction principale.
*/
int main()
{
	initDebogageMemoire();

	Image image = {};
	//TODO: Charger une image.
	//TODO: Afficher la taille de l'image en nombre d'octets conservés au total dans les lignes.
	//TODO: Découper le vide de l'image.
	//TODO: Afficher nouvelle taille de l'image en nombre d'octets conservés au total dans les lignes.
	//TODO: Afficher l'image en texte.
	//TODO: Désallouer l'image.
	
	if (image.lignes != nullptr)
		cout << "Le pointeur de lignes devrait etre nul rendu a la fin du programme." << endl;
}

//TODO: S'assurer que la compilation ne donne pas d'avertissement. \file
//TODO: S'assurer que l'exécution avec débogage n'indique pas de fuite de mémoire dans le fenêtre "Sortie" de Visual Studio. \file

// vi: syn=cpp : ts=4 : wrap : lbr : bri : brishift=4 : tagmarker=//TODO,m3









test test test