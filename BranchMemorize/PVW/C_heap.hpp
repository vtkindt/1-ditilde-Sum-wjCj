#pragma once

/*************************************************************************************

	Cette classe implémente un tas (heap in english) d'int.

ATTENTION classe PAS générique du tout
		elle est dédiée à L'ORDO

	Elle ne peut être utilisée dans ce état que si on dispose
		des fonctions globales dont les prorotypes sont:
				int ditilde(int i);
				int pi(int i);
				int wi(int i);
		et de la variable globale:
				int nbre_travaux;

			NB: l'int "i" représente donc un travail de processing time "pi(i)" etc...

	Elle permet ainsi de trier le tas par ordre croissant de pi ou ditilde ou wi selon
	la méthode appelée pour insérer... extraire...
	...et aussi etre utilises comme liste non triee.

*************************************************************************************/

#include "donnees.h"

class C_heap
{
public:
	static int cpteur; //compteur d'instances
	int taille;				// taille du tas
	int taille_max;		// taille maximale du tas
	int * element;		// contenu du tas

public:
	// Constructeurs et Destructeurs
	C_heap();
	C_heap(int new_taille_max); // Alloue pour 'new_taille_max' éléments
	~C_heap();

	//Opérateurs
	C_heap & operator = (C_heap & father);
 
	// Selecteurs
	int rechercher_element(int value);
 
	// Primitives d'insertion/extraction dans le cas non trié

	void insert_alafin(int value); //Insère en position quelconque ( = en O(1) )
	int extract_position_pasdetri(int index);
																// Extrait l'élément en position 'index' ( = en O(1) )

	// Primitives d'insertion/extraction dans le cas trié croissant ( = en O(log(n)) )
	
	void insert_croissant(int value);		//
	int extract_croissant();						// prend VALUE en compte pour le tri
	void insert_decroissant(int value);	//
	int extract_decroissant();					//

	void insert_pi_croissant(int value);
	int extract_pi_croissant();
	void insert_pidisurwi_croissant(int value);
	int extract_pidisurwi_croissant();
	void insert_pisurwi_croissant(int value);
	int extract_pisurwi_croissant();
	void insert_pisurwitmp_croissant(int value);
	int extract_pisurwitmp_croissant();
	void insert_ditilde_croissant(int value);
	int extract_ditilde_croissant();
	void insert_wi_croissant(int value);
	int extract_wi_croissant();

	// Primitives d'insertion/extraction dans le cas trié décroissant ( = en O(log(n)) )

	void insert_pi_decroissant(int value);
	int extract_pi_decroissant();
	void insert_pisurwi_decroissant(int value);
	int extract_pisurwi_decroissant();
	void insert_pisurwitmp_decroissant(int value);
	int extract_pisurwitmp_decroissant();
	void insert_ditilde_decroissant(int value);
	void insert_ditilde_puis_pisurwi_decroissant(int value);
	int extract_ditilde_decroissant();
	void insert_wi_decroissant(int value);
	int extract_wi_decroissant();


	// Primitives diverses
	void afficher();	//Affiche les éléments du tas

private:

	// Autres
	void echanger(int index1, int index2); // Echange 2 éléments

};
