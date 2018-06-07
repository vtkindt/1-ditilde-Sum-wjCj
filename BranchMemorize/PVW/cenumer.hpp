#pragma once
/******************************************************************************
***** Titre : A Branch and Bound algorithm for enumeration                *****
*******************************************************************************
***** Auteurs : Esswein Carl	**************************** Version 2.0		*****
***************	d'après une trame de Vincent T'Kindt **************************
***** Date : 13/12/01 *********************************************************
*******************************************************************************
***** Lecteur :  **************************************** Date ****************
*******************************************************************************
******************************************************************************/

#ifndef MCenumer
#define MCenumer

#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include "Clistenoeud.hpp"
#include "C_heap.hpp"
#include "Cliste.hpp"
#include "C_VectCrit.hpp"
#include "database.h"



class Cenumer
{
 public:

	BIGINT Nbmax;
	BIGINT fois;
	BIGINT BestUB;		// utile uniquement pour le pb 1|ditilde|wCsum résolu seul
	BIGINT LBroot;		// utile uniquement pour le pb 1|ditilde|wCsum résolu seul

	BIGINT wCsumSUP;	// Une borne sup pour le wCsum de toute solution non dominée
	BIGINT nb_opt;
	Cliste<C_VectCrit> Eapp; // Approximation de l'ensemble E
    Cnodelist *Nodelist;					// List of nodes to branch from (=activ nodes)
    Cnodelist *Levellist;  // Array of List of active, sorted by levels
	BIGINT nblevel; // Number of nodes in Levellist
    Cnodelist *NodelistSuiv;			// List of nodes delayed for (mayby) futur branching
	Cnodelist *Donelist[maxtrav];	// Array of List of nodes already developped
																	//    => one per level in the search tree
	// NB: Donelist[3] is a list of nodes whose scheduled list's size is 4 !!!

	C_VectCrit *BestVectCrit;	// Best criteria vector for the search of the current opt

	C_heap prec[maxtrav];	// Tableau des listes des prédécesseurs
												// NB: prec[0] contient les prédécésseurs de J1
	C_heap succ[maxtrav];	// Tableau des listes des successeurs
	BIGINT nb_nodes;     // Number of nodes developped for each sub-problem
	BIGINT nb_nodes_total_enum; // Total number of nodes developped for the enum problem

	C_heap Bestsol; // The best solution ever found
									// utilisé uniquement pour le 1|ditilde|wCsum...
 // User functions
 public:
  Cenumer();
  ~Cenumer();

	void determiner_Eapp();
	/* détermine l'approximation de l'ensemble cherché */

	void precedence(int a, int b);
        /* precedence
        I: le predecesseur a et le successeur b
        needs: 1<=a<=nbre_travaux AND 1<=b<=nbre_travaux
							 AND a n'est pas déjà successeur de b, ni b successeur de a
        O: nothing
        implies: a est ajouté à prec[b-1]
						 AND b est ajouté à succ[a-1] */

	void MAJ_Nodelist(BIGINT last_Lmax);
	/* cf .CPP */

	void initialize_1ditilde();
        /* This function reduce the deadlines and generates precedence-constraints
						that are necessarly satisfied by the optimal solution searched
        I: Nothing
        needs: The data have been read
        O: Nothing
        implies: The problem is solved */

	void initialize_seqref();

	void initialize_enum();
        /* idem MAIS : pour l'algo d'énumération seulement! */

  void solve_enum(double L, double R, int iter);
        /* This function solves the problem of enumeration
        I: 
        needs: The data have been read AND the function 'initialized' has been called
        O: Nothing
        implies: The problem is solved */

	void solve_1ditilde(double L, double R, int iter);
        /* This function solves the problem 1|ditilde|wCsum
        I: 
        needs: The data have been read AND the function 'initialized' has been called
        O: Nothing
        implies: The problem is solved */
  
  void separation(Cnoeud * father, Cnodelist * childs);
				/* This function ...
				I: the node to branch from AND the node list where to create the childs
				needs: the node "*father" has already been extracted from Nodelist
								AND "childs" is an empty list
        O: Nothing
        implies: ...     */

	void separation_enum(Cnoeud * father, Cnodelist * childs);
				/* This function ...
				I: the node to branch from AND the node list where to create the childs
				needs: the node "*father" has already been extracted from Nodelist
								AND "childs" is an empty list
        O: Nothing
        implies: ...     */

	bool calculerUBLB(Cnoeud *);
        /* This function computes and modify the UB and LB values of the given node
        I: the node to compute the UB and the LB of
        needs: nothing
        O: Nothing
        implies: the UB and the LB are modified if necessarys   */

    bool Cenumer::UBLBpvw(Cnoeud * node, double *LB, BIGINT *UB, BIGINT *Lmax);
        /* This function computes the original lower and upper bounds of P&VW83
        I: the node to compute the UB and the LB of
        needs: nothing
        O: Nothing
        implies: the UB and the LB are modified if necessarys   */

    bool Cenumer::UBLBnew(Cnoeud * node, double *LB, BIGINT *UB, BIGINT *Lmax);
        /* This function computes the modified lower and upper bounds of P&VW83
        I: the node to compute the UB and the LB of
        needs: nothing
        O: Nothing
        implies: the UB and the LB are modified if necessarys   */

	bool calculerUBLB_enum(Cnoeud *);
        /* This function computes and modify the UB and LB values of the given node
        I: the node to compute the UB and the LB of
        needs: nothing
        O: Nothing
        implies: the UB and the LB are modified if necessarys   */

	bool actif_domine(Cnoeud * node);
        /* This function checks if the given node is dominated by another active node
        I: the node to check
        needs: nothing
        O: TRUE if the node is dominated FALSE otherwise
        implies: nothing  */

	bool cut_domine(Cnoeud * node);
        /* This function checks if the given node is dominated by an already-cut
					(or already-expanded) node
        I: the node to check
        needs: nothing
        O: TRUE if the node is dominated FALSE otherwise
        implies: nothing  */
	bool cut_domine_enum_wCsum(Cnoeud * node, Cnoeud * dominant);
	/* pour l'algo d'énumération seulement! 

		Cette fonction teste si un noeud est dominé par un noeud de Donelist,
		à la fous sur le wCsum et sur les 2 critères (via la valeur retournée et
		l'argument "dominant" (je sais, c'est pas clair, mais les spécifs du .CPP
		sont mieux!!) */

        bool LBposner(Cnoeud * node, double *LB);

	bool traitement_fils(Cnoeud * courant);
	/* cf .CPP */

	void evaluer_feuille(Cnoeud *courant);
	/* cf .CPP */

	void MAJ_Eapp(BIGINT new_wCsum, BIGINT new_Lmax);
	/* cf .CPP */
 
};

#endif
