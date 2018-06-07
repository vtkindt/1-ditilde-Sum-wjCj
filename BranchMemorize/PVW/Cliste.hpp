/*---------------------------------------------------------------------------
-----   TITRE   : Classe pour gérer les listes                                                  -----
-----------------------------------------------------------------------------
-----   AUTEUR  : LAUGT D. / TERICNET F. / T'KINDT V.   VERSION 2.0             -----
-----   DATE    : 12/04/2000    ---------------------------------------------
-----------------------------------------------------------------------------
-----   LECTEUR : LAUGT D. / TERCINET F. / T'KINDT V.   DATE : 30/05/2000       -----
-----------------------------------------------------------------------------
-----   INTERFACE DE LA CLASSE Cliste                                                                   -----
---------------------------------------------------------------------------*/

#pragma once

#ifndef Mcliste
#define Mcliste

#include <fstream>



// Déclaration de la cellule constituant nos listes
template <class T> 
struct TCell
{ 
    T *val;
    TCell<T> *suiv;
		static BIGINT nbobj;
		TCell<T>() {nbobj++;}
		~TCell<T>() {nbobj--;}
};



// Déclaration de la liste
template <class T>
class Cliste
{
        // Cette classe, de type abstrait, représente une liste d'élements quelconques.

        // ATRIBUTS
        public:
                TCell<T> *liste;        // Le premier élément de la liste
                int taille;            // La taille de la liste
                TCell<T> *encours;
                int indpos;
								TCell<T> *last;
				public:
				static BIGINT nbobj;

        // PRIMITIVES
        public:
                inline Cliste() {taille=0; liste=NULL; indpos=999999; nbobj++;}
                /* Constructeur par défaut de la classe
                        E:néant
                        nécessite:néant
                        S:néant
                        entraine:l'objet est initialisé */

                inline ~Cliste();
                /* Destructeur de la classe
                        E:néant
                        nécessite:néant
                        S:néant
                        entraine:l'objet est détruit */

                Cliste (const Cliste &L);
                /* Constructeur de recopie
                        E: la liste à recopier
                        nécessite: néant
                        S: l'objet initialisé
                        entraine: (l'objet est initialisé et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de mémoire) */

                Cliste &operator = (const Cliste &L);
                /* Permet d'affecter une liste
                        E: la liste à recopier
                        nécessite: néant
                        S: l'objet initialisé
                        entraine: (l'objet est initialisé et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de mémoire) */

                T  * operator [](unsigned  int i);
                /* Permet d'accéder au ième élément dans la liste
                        E: l'indice de l'élément à acceder
                        nécessite: néant
                        S: l'élément en ième position
                        entraine: (l'élément en ième position est retourné ET non EXCEPTION)
                          OU (EXCEPTION "Acces impossible": l'indice est plus grand que le
                                                                                                nombre d'élement) */

               void Inserer_position(T *val,unsigned  int indice);
                /* Permet d'insérer un élément en ième élément dans la liste
                        E: l'élément à insérer et son indice
                        nécessite: néant
                        S: néant
                        entraine: (élément inséré en ième position ET non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de mémoire)
                          OU (EXCEPTION "Ajout impossible": l'indice est plus grand que le
                                                                                                nombre d'élement) */

               void ajouter_element(T * valeur);
                /* Cette fonction permet d'ajouter un élément à la liste si possible
                 E: un élément à ajouter
                 nécessite : néant
                 S: néant
                 entraine : (l'élément est ajouté ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de mémoire) */


                void Supprimer_position(unsigned  int indice);
                /* Permet de supprimer l'élément en ième élément dans la liste
                        E: l'indice de l'élément à supprimer
                        nécessite: néant
                        S: néant
                        entraine: (élément supprimé en ième position ET non EXCEPTION)
                          OU (EXCEPTION "Suppression impossible": l'indice est plus grand
                                                                                        que le nombre d'élement) */

                void Modifier_position(unsigned  int valeur, unsigned  int indice);
                /* Permet de modifier l'élément en ième position dans la liste
                        E: l'indice de l'élément à modifier et sa valeur
                        nécessite: néant
                        S: néant
                        entraine: (élément modifié en ième position ET non EXCEPTION)
                          OU (EXCEPTION "Suppression impossible": l'indice est plus grand
                                                                                        que le nombre d'élement) */


};

// Cette inclusion est nécessaire lors de la compilation d'objet template
#include "Cliste.cpp"

#endif
