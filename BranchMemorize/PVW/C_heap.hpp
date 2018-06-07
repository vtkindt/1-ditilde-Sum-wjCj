#pragma once

/*************************************************************************************

	Cette classe impl�mente un tas (heap in english) d'int.

ATTENTION classe PAS g�n�rique du tout
		elle est d�di�e � L'ORDO

	Elle ne peut �tre utilis�e dans ce �tat que si on dispose
		des fonctions globales dont les prorotypes sont:
				int ditilde(int i);
				int pi(int i);
				int wi(int i);
		et de la variable globale:
				int nbre_travaux;

			NB: l'int "i" repr�sente donc un travail de processing time "pi(i)" etc...

	Elle permet ainsi de trier le tas par ordre croissant de pi ou ditilde ou wi selon
	la m�thode appel�e pour ins�rer... extraire...
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
	C_heap(int new_taille_max); // Alloue pour 'new_taille_max' �l�ments
	~C_heap();

	//Op�rateurs
	C_heap & operator = (C_heap & father);
 
	// Selecteurs
	int rechercher_element(int value);
 
	// Primitives d'insertion/extraction dans le cas non tri�

	void insert_alafin(int value); //Ins�re en position quelconque ( = en O(1) )
	int extract_position_pasdetri(int index);
																// Extrait l'�l�ment en position 'index' ( = en O(1) )

	// Primitives d'insertion/extraction dans le cas tri� croissant ( = en O(log(n)) )
	
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

	// Primitives d'insertion/extraction dans le cas tri� d�croissant ( = en O(log(n)) )

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
	void afficher();	//Affiche les �l�ments du tas

private:

	// Autres
	void echanger(int index1, int index2); // Echange 2 �l�ments

};
