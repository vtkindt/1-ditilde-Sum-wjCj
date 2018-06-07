/*---------------------------------------------------------------------------
-----   TITRE   : Classe pour g�rer les listes                                                  -----
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



// D�claration de la cellule constituant nos listes
template <class T> 
struct TCell
{ 
    T *val;
    TCell<T> *suiv;
		static BIGINT nbobj;
		TCell<T>() {nbobj++;}
		~TCell<T>() {nbobj--;}
};



// D�claration de la liste
template <class T>
class Cliste
{
        // Cette classe, de type abstrait, repr�sente une liste d'�lements quelconques.

        // ATRIBUTS
        public:
                TCell<T> *liste;        // Le premier �l�ment de la liste
                int taille;            // La taille de la liste
                TCell<T> *encours;
                int indpos;
								TCell<T> *last;
				public:
				static BIGINT nbobj;

        // PRIMITIVES
        public:
                inline Cliste() {taille=0; liste=NULL; indpos=999999; nbobj++;}
                /* Constructeur par d�faut de la classe
                        E:n�ant
                        n�cessite:n�ant
                        S:n�ant
                        entraine:l'objet est initialis� */

                inline ~Cliste();
                /* Destructeur de la classe
                        E:n�ant
                        n�cessite:n�ant
                        S:n�ant
                        entraine:l'objet est d�truit */

                Cliste (const Cliste &L);
                /* Constructeur de recopie
                        E: la liste � recopier
                        n�cessite: n�ant
                        S: l'objet initialis�
                        entraine: (l'objet est initialis� et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de m�moire) */

                Cliste &operator = (const Cliste &L);
                /* Permet d'affecter une liste
                        E: la liste � recopier
                        n�cessite: n�ant
                        S: l'objet initialis�
                        entraine: (l'objet est initialis� et non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de m�moire) */

                T  * operator [](unsigned  int i);
                /* Permet d'acc�der au i�me �l�ment dans la liste
                        E: l'indice de l'�l�ment � acceder
                        n�cessite: n�ant
                        S: l'�l�ment en i�me position
                        entraine: (l'�l�ment en i�me position est retourn� ET non EXCEPTION)
                          OU (EXCEPTION "Acces impossible": l'indice est plus grand que le
                                                                                                nombre d'�lement) */

               void Inserer_position(T *val,unsigned  int indice);
                /* Permet d'ins�rer un �l�ment en i�me �l�ment dans la liste
                        E: l'�l�ment � ins�rer et son indice
                        n�cessite: n�ant
                        S: n�ant
                        entraine: (�l�ment ins�r� en i�me position ET non EXCEPTION)
                          OU (EXCEPTION bad_alloc: pas assez de m�moire)
                          OU (EXCEPTION "Ajout impossible": l'indice est plus grand que le
                                                                                                nombre d'�lement) */

               void ajouter_element(T * valeur);
                /* Cette fonction permet d'ajouter un �l�ment � la liste si possible
                 E: un �l�ment � ajouter
                 n�cessite : n�ant
                 S: n�ant
                 entraine : (l'�l�ment est ajout� ET non EXCEPTION) OU
                   (EXCEPTION bad_alloc : pas assez de m�moire) */


                void Supprimer_position(unsigned  int indice);
                /* Permet de supprimer l'�l�ment en i�me �l�ment dans la liste
                        E: l'indice de l'�l�ment � supprimer
                        n�cessite: n�ant
                        S: n�ant
                        entraine: (�l�ment supprim� en i�me position ET non EXCEPTION)
                          OU (EXCEPTION "Suppression impossible": l'indice est plus grand
                                                                                        que le nombre d'�lement) */

                void Modifier_position(unsigned  int valeur, unsigned  int indice);
                /* Permet de modifier l'�l�ment en i�me position dans la liste
                        E: l'indice de l'�l�ment � modifier et sa valeur
                        n�cessite: n�ant
                        S: n�ant
                        entraine: (�l�ment modifi� en i�me position ET non EXCEPTION)
                          OU (EXCEPTION "Suppression impossible": l'indice est plus grand
                                                                                        que le nombre d'�lement) */


};

// Cette inclusion est n�cessaire lors de la compilation d'objet template
#include "Cliste.cpp"

#endif
