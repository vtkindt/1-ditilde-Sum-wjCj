#pragma once
/*---------------------------------------------------------------------------
-----   TITLE   : Classe pour g�rer les listes de noeuds                -----
-----------------------------------------------------------------------------
-----   AUTEUR  : T'KINDT V. / LAUGT D. / TERICNET F. VERSION 1.0       -----
-----							Carl ESSWEIN																					-----
-----   DATE    : 15/10/2001    ---------------------------------------------
-----------------------------------------------------------------------------
-----   LECTEUR : T'KINDT V. / LAUGT D. / TERICNET F. DATE : 15/10/2001 -----
-----							Carl ESSWEIN																					-----
-----------------------------------------------------------------------------
-----   INTERFACE DE LA CLASSE Cnodelist                                -----
---------------------------------------------------------------------------*/

#ifndef Mclistenoeud
#define Mclistenoeud

#include <fstream>
#include "Cnoeud.hpp"
#include <stdio.h>

// D�claration d'un �l�ment de la liste
struct TCellNode
{ 
    Cnoeud *val;
    TCellNode *suiv;
		TCellNode() {val=NULL; suiv=NULL;}
		TCellNode(TCellNode & ) {val=NULL; suiv=NULL;}
		~TCellNode() {}
		TCellNode & operator = (TCellNode &) {return *this;}
};

// D�claration de la liste de noeuds
class Cnodelist
{
        // Cette classe, de type abstrait, repr�sente une liste de noeuds.

        // ATRIBUTS
        public:
				static BIGINT cpteur;
				TCellNode *liste;      // Le premier �l�ment de la liste
				int taille;            // La taille de la liste
				TCellNode *encours;
				int indpos;
				TCellNode *avencours;
				int avindpos;

        // PRIMITIVES
        public:
                Cnodelist();
                /* Constructeur par d�faut de la classe
                        E:n�ant
                        n�cessite:n�ant
                        S:n�ant
                        entraine:l'objet est initialis� */

                ~Cnodelist();
                /* Destructeur de la classe
                        E:n�ant
                        n�cessite:n�ant
                        S:n�ant
                        entraine:l'objet est d�truit */

                Cnodelist (const Cnodelist &L);
                /* Constructeur de recopie
                        E: la liste � recopier
                        n�cessite: n�ant
                        S: l'objet initialis�
                        entraine: (l'objet est initialis� et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de m�moire) */

                Cnodelist &operator = (const Cnodelist &L);
                /* Permet d'affecter une liste
                        E: la liste � recopier
                        n�cessite: n�ant
                        S: l'objet initialis�
                        entraine: (l'objet est initialis� et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de m�moire) */

                Cnoeud  * operator [](int i);
                /* Permet d'acc�der au i�me �l�ment dans la liste
                        E: l'indice de l'�l�ment � acceder
                        n�cessite: n�ant
                        S: l'�l�ment en i�me position
                        entraine: (l'�l�ment en i�me position est retourn� ET non EXCEPTION)
                          OU (EXCEPTION "Acces impossible": l'indice est plus grand que le
                                                            nombre d'�lement) */

               void ajouter_element(Cnoeud * valeur);
                /* Cette fonction permet d'ajouter un �l�ment � la liste si possible
                 E: un �l�ment � ajouter
                 n�cessite : n�ant
                 S: n�ant
                 entraine : (l'�l�ment est ajout� ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de m�moire) */

               void Inserer_position(Cnoeud *val,int indice);
                /* Permet d'ins�rer un �l�ment en i�me �l�ment dans la liste
                        E: l'�l�ment � ins�rer et son indice
                        n�cessite: n�ant
                        S: n�ant
                        entraine: (�l�ment ins�r� en i�me position ET non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de m�moire)
                          OU (EXCEPTION "Ajout impossible": l'indice est plus grand que le
                                                           nombre d'�lement) */

			   int ajouter_element_LB_croissant(Cnoeud *valeur);
                /* Cette fonction permet d'ajouter un �l�ment � la liste si possible
                        dans l'ordre croissant de valeur 
                 E: un �l�ment � ajouter
                 n�cessite : n�ant
                 S: La position d'insertion
                 entraine : (l'�l�ment est ajout� ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de m�moire) */
			 int ajouter_element_LB_decroissant(Cnoeud *valeur);
                /* Cette fonction permet d'ajouter un �l�ment � la liste si possible
                        dans l'ordre decroissant de valeur de la LB
                 E: un �l�ment � ajouter
                 n�cessite : n�ant
                 S: La position d'insertion
                 entraine : (l'�l�ment est ajout� ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de m�moire) */

			 int ajouter_element_NIVcroissant(Cnoeud *valeur);
                /* Cette fonction permet d'ajouter un �l�ment � la liste si possible
                        dans l'ordre croissant de son niveau
                 E: un �l�ment � ajouter
                 n�cessite : n�ant
                 S: La position d'insertion
                 entraine : (l'�l�ment est ajout� ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de m�moire) */

        void Supprimer_position(int indice);
                /* Permet de supprimer l'�l�ment en i�me �l�ment dans la liste
                        E: l'indice de l'�l�ment � supprimer
                        n�cessite: n�ant
                        S: n�ant
                        entraine: (�l�ment supprim� en i�me position ET non EXCEPTION)
                          OU (EXCEPTION "Suppression impossible": l'indice est plus grand
                                                                 que le nombre d'�lement) */
				void concatene(Cnodelist *First);
				   /* This function add the content of the whole list First at the end of 
					    the current list of nodes */

};

#endif
