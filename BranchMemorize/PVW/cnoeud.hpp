#pragma once
/******************************************************************************
***** Titre : A node of the Branch and Bound                              *****
*******************************************************************************
***** AuteurS : T'kindt vincent ************************** Version 1.0    *****
*****					Carl ESSWEIN	****************************** Version 2.0    *****
***** Date : 15/12/01 *********************************************************
*******************************************************************************
***** This class contains the definition and the operations of a node     *****
******************************************************************************/

#ifndef Mcnoeud
#define Mcnoeud

#include <fstream>
#include "donnees.h"
#include "C_heap.hpp"

#define MAXVALUE 999999999

class Cnoeud
{
  public:
		static BIGINT cpteur; // compteur d'instances

		BIGINT LB; 										// Lower bound at a node
		BIGINT wCsum_heur;	// wCsum value of the solution giving the upper bound
		BIGINT Lmax_heur;	// Lmax value of the solution giving the upper bound
		bool * bool_scheduled;	// Liste de bools indiquant si les jobs sont ds scheduled ou non.
		C_heap scheduled;				// List of already scheduled jobs
		C_heap candidate;				// List of jobs candidate for the next position to be checked
		C_heap toschedule;			// Jobs remaining to schedule and not in candidate
		BIGINT partial_wCsum; // contribution of scheduled to the CsumW criteria
		BIGINT partial_Cmax;	// completion time of any sequence with the jobs of toschedule
		BIGINT partial_Lmax;	// maximum lateness of any sequence with the jobs of toschedule

  public:
		//Constructeurs et destructeur
		Cnoeud();
		Cnoeud(Cnoeud&);
		~Cnoeud();

		Cnoeud & operator =(Cnoeud & father);
					/* Overload of the assignement operator.
					Used to copy a father node into a child node.
					I: The father node
					needs: Nothing
					O: The child node
					implies: The child node has been updated */
	
		void init_root();
					/* This function must be ONLY called to initialize the root node
					I: Nothing
					needs: Nothing
					O: Nothing
					implies: "toschedule" and "candidate" are initialized */
};

#endif
