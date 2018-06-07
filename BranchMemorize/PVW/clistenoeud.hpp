#pragma once
/*---------------------------------------------------------------------------
-----   TITLE   : Classe pour gérer les listes de noeuds                -----
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

// Déclaration d'un élément de la liste
struct TCellNode
{ 
    Cnoeud *val;
    TCellNode *suiv;
		TCellNode() {val=NULL; suiv=NULL;}
		TCellNode(TCellNode & ) {val=NULL; suiv=NULL;}
		~TCellNode() {}
		TCellNode & operator = (TCellNode &) {return *this;}
};

// Déclaration de la liste de noeuds
class Cnodelist
{
        // Cette classe, de type abstrait, représente une liste de noeuds.

        // ATRIBUTS
        public:
				static BIGINT cpteur;
				TCellNode *liste;      // Le premier élément de la liste
				int taille;            // La taille de la liste
				TCellNode *encours;
				int indpos;
				TCellNode *avencours;
				int avindpos;

        // PRIMITIVES
        public:
                Cnodelist();
                /* Constructeur par défaut de la classe
                        E:néant
                        nécessite:néant
                        S:néant
                        entraine:l'objet est initialisé */

                ~Cnodelist();
                /* Destructeur de la classe
                        E:néant
                        nécessite:néant
                        S:néant
                        entraine:l'objet est détruit */

                Cnodelist (const Cnodelist &L);
                /* Constructeur de recopie
                        E: la liste à recopier
                        nécessite: néant
                        S: l'objet initialisé
                        entraine: (l'objet est initialisé et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de mémoire) */

                Cnodelist &operator = (const Cnodelist &L);
                /* Permet d'affecter une liste
                        E: la liste à recopier
                        nécessite: néant
                        S: l'objet initialisé
                        entraine: (l'objet est initialisé et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de mémoire) */

                Cnoeud  * operator [](int i);
                /* Permet d'accéder au ième élément dans la liste
                        E: l'indice de l'élément à acceder
                        nécessite: néant
                        S: l'élément en ième position
                        entraine: (l'élément en ième position est retourné ET non EXCEPTION)
                          OU (EXCEPTION "Acces impossible": l'indice est plus grand que le
                                                            nombre d'élement) */

               void ajouter_element(Cnoeud * valeur);
                /* Cette fonction permet d'ajouter un élément à la liste si possible
                 E: un élément à ajouter
                 nécessite : néant
                 S: néant
                 entraine : (l'élément est ajouté ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de mémoire) */

               void Inserer_position(Cnoeud *val,int indice);
                /* Permet d'insérer un élément en ième élément dans la liste
                        E: l'élément à insérer et son indice
                        nécessite: néant
                        S: néant
                        entraine: (élément inséré en ième position ET non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de mémoire)
                          OU (EXCEPTION "Ajout impossible": l'indice est plus grand que le
                                                           nombre d'élement) */

			   int ajouter_element_LB_croissant(Cnoeud *valeur);
                /* Cette fonction permet d'ajouter un élément à la liste si possible
                        dans l'ordre croissant de valeur 
                 E: un élément à ajouter
                 nécessite : néant
                 S: La position d'insertion
                 entraine : (l'élément est ajouté ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de mémoire) */
			 int ajouter_element_LB_decroissant(Cnoeud *valeur);
                /* Cette fonction permet d'ajouter un élément à la liste si possible
                        dans l'ordre decroissant de valeur de la LB
                 E: un élément à ajouter
                 nécessite : néant
                 S: La position d'insertion
                 entraine : (l'élément est ajouté ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de mémoire) */

			 int ajouter_element_NIVcroissant(Cnoeud *valeur);
                /* Cette fonction permet d'ajouter un élément à la liste si possible
                        dans l'ordre croissant de son niveau
                 E: un élément à ajouter
                 nécessite : néant
                 S: La position d'insertion
                 entraine : (l'élément est ajouté ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de mémoire) */

        void Supprimer_position(int indice);
                /* Permet de supprimer l'élément en ième élément dans la liste
                        E: l'indice de l'élément à supprimer
                        nécessite: néant
                        S: néant
                        entraine: (élément supprimé en ième position ET non EXCEPTION)
                          OU (EXCEPTION "Suppression impossible": l'indice est plus grand
                                                                 que le nombre d'élement) */
				void concatene(Cnodelist *First);
				   /* This function add the content of the whole list First at the end of 
					    the current list of nodes */

};

#endif
