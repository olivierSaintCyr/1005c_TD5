/*************************************************************************//**
*  Le TD5 final d'INF1005C, hiver 2020.
*  Programme pour lire, découper et afficher des images TGA.
*  \file    td5.cpp
*  \author  Lévesque, William (2028363)
*  \author  Saint-Cyr, Olivier (2054136)
*  \date    02 Avril 2020
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
wchar_t é() {
	return wchar_t(130);
}
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
void deleteLigne(LigneImage& ligne) {
	delete[] ligne.intensites; //A verifier
	ligne.longueur = 0;
	ligne.intensites = nullptr;
}

void copierLigne(LigneImage& ligne, int debut, int longueur) {
	uint8_t* nouvelleIntensites = new uint8_t[longueur]; //Copier ligne
	
	for (int j : range(0, longueur)) {
		nouvelleIntensites[j] = ligne.intensites[debut + j];
	}
	deleteLigne(ligne);

	ligne.debut = debut;
	ligne.longueur = longueur;
	ligne.intensites = nouvelleIntensites;
}
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
	for (int i : range(image.hauteur - 1, -1, -1)) {
		if (image.lignes[i].intensites != nullptr) {
			delete[] image.lignes[i].intensites;
			image.lignes[i].intensites = nullptr;
		}
	}
	if (image.lignes != nullptr) {
		delete[] image.lignes;
		image.lignes = nullptr;
	}
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
	ifstream fichier(nomImage, ios::binary);
	
	fichier.read((char*)&entete, sizeof(entete));
	
	// On vérifie que le fichier correspond au format simple qu'on supporte.d
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
	image.lignes = new LigneImage[image.hauteur];
	
	for (int i : range(entete.hauteur - 1, -1, -1)) {
		//TODO: Mettre le début de ligne à 0 et sa longueur à entete.largeur .
		image.lignes[i].debut = 0;
		image.lignes[i].longueur = entete.largeur;
		
		//TODO: Allouer le tableau pour les intensités des pixels de la ligne.
		image.lignes[i].intensites = new uint8_t[image.largeur];
		for (int j : range(0, image.largeur)) {
			//TODO: Lire les intensités pour une ligne, à partir du fichier, vers le tableau alloué ci-dessus.
			fichier.read((char*)&image.lignes[i].intensites[j], sizeof(uint8_t));
		}
		
	}
	return true;
}

//TODO: Écrire l'entête de fonction.
void decouperVide(Image& image)//<---------------------------------------------------------------------------------------
{
	//TODO: Pour chaque ligne de l'image où un tableau d'intensité est alloué...
	//TODO:   Si la ligne a uniquement des pixels vides, désallouer, mettre le pointeur à nullptr et la longueur à zéro.
	bool existeNonZero = false, debutTrouver = false, finTrouver = false;
	int debutLigne = -1, finLigne = -1;
	for (int i : range(0,image.hauteur)) {
		debutLigne = -1;
		finLigne = -1;
		existeNonZero = false, debutTrouver = false, finTrouver = false;
		if (image.lignes[i].intensites != nullptr) {//<---------------------------------------------------------------------------------------
			for (int j : range(0, image.largeur)) {
				if (image.lignes[i].intensites[j] != 0 && !debutTrouver) {
					existeNonZero = true;
					debutTrouver = true;
					debutLigne = j;
				}

				if (image.lignes[i].intensites[image.largeur - j - 1] != 0 && !finTrouver) {
					existeNonZero = true;
					finTrouver = true;
					finLigne = image.largeur - j - 1;
				}
			}
		}
		int longueur = finLigne - debutLigne + 1;
		if (!existeNonZero) {//<---------------------------------------------------------------------------------------
			//delete[] image.lignes[i].intensites; //A verifier
			//image.lignes[i].longueur = 0;
			//image.lignes[i].intensites = nullptr;
			deleteLigne(image.lignes[i]);
		}
		else if (image.lignes[i].debut != debutLigne && image.lignes[i].longueur != longueur) {
			//<---------------------------------------------------------------------------------------
			//uint8_t* nouvelleIntensites = new uint8_t[longueur]; //Copier ligne
			//for (int j : range(0, longueur)) {
			//	nouvelleIntensites[j] = image.lignes[i].intensites[debutLigne+j];
			//}
			//image.lignes[i].debut = debutLigne;
			//image.lignes[i].longueur = longueur;
			//delete image.lignes[i].intensites;
			//image.lignes[i].intensites = nouvelleIntensites;
			copierLigne(image.lignes[i], debutLigne, longueur);
		}
		
	}
	//TODO:   Sinon, si la ligne a des pixels vides au début et/ou à la fin, allouer un nouveau tableau plus petit, y copier les intensités qu'il faut conserver, et ajuster le debut et la longueur de la ligne.  Ensuite désallouer l'ancien tableau et le remplacer par le nouveau.
}

//TODO: Écrire la fonction tailleImage.
unsigned tailleImage(const Image& image)
{
	unsigned taille = 0;
	for (int i : range(0,image.hauteur)){
		taille += image.lignes[i].longueur;
	}
	return taille;
}

//TODO: Écrire la fonction afficherImage.
void afficherImage(Image& image) {
	for (int i : range(0,image.hauteur)) {
		if (image.lignes[i].intensites != nullptr) {
			unsigned indexPixelVivant = 0;
			for (int j : range(0, image.largeur)) {
				if (j >= image.lignes[i].debut && j < image.lignes[i].debut + image.lignes[i].longueur) {
					afficherGris(image.lignes[i].intensites[indexPixelVivant]);
					indexPixelVivant++;
				}
				else {
					cout << " ";
				}
			}
		}
		cout << endl;
	}
}
//TODO: S'assurer que la boucle interne de afficherImage n'a pas trois déréférences; de préférence elle devrait utiliser une boucle sur intervalles qui fait une seule déréférence implicitement.

/*************************************************************************//**
*  Fonction principale.
*/
int main()
{
	initDebogageMemoire();

	Image image = {};
	//TODO: Charger une image.
	string nomFichier = "rond.tga";
	chargerImage(image, nomFichier);
	//TODO: Afficher la taille de l'image en nombre d'octets conservés au total dans les lignes.
	cout << "L'image " << tailleImage(image) << " octets" << endl;
	//TODO: Découper le vide de l'image.
	decouperVide(image);
	//TODO: Afficher nouvelle taille de l'image en nombre d'octets conservés au total dans les lignes.
	//wcout << L"L'image a " << tailleImage(image) << L" octets suite au " << "d" << wchar_t(130) <<"coupage" << endl;
	wcout << L"L'image a " << tailleImage(image) << L" octets suite au " << "d" << é() << "coupage" << endl;
	//TODO: Afficher l'image en texte.
	afficherImage(image);
	//TODO: Désallouer l'image.
	desallouerImage(image);
	if (image.lignes != nullptr)
		cout << "Le pointeur de lignes devrait etre nul rendu a la fin du programme." << endl;
	
}

//TODO: S'assurer que la compilation ne donne pas d'avertissement. \file
//TODO: S'assurer que l'exécution avec débogage n'indique pas de fuite de mémoire dans le fenêtre "Sortie" de Visual Studio. \file

// vi: syn=cpp : ts=4 : wrap : lbr : bri : brishift=4 : tagmarker=//TODO,m3

