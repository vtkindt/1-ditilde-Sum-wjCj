/******************************************************************************
***** Titre : A node of the Branch and Bound                              *****
*******************************************************************************
***** Auteur : T'kindt vincent ************************** Version 1.0     *****
*****					 Carl	Esswein		*************************** Version 2.0     *****
***** Date : 15/12/01 *********************************************************
*******************************************************************************
***** This class contains the definition and the operations of a node     *****
******************************************************************************/

#include "Cnoeud.hpp"
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

BIGINT Cnoeud::cpteur=0;

Cnoeud::Cnoeud()
{
	cpteur++;
	Lmax_heur=-MAXVALUE; wCsum_heur=MAXVALUE;
	LB=0; partial_wCsum=0; partial_Cmax=0; partial_Lmax=-MAXVALUE;

	bool_scheduled=(bool*)malloc(nbre_travaux*sizeof(bool));

	for(int i=0;i<nbre_travaux;i++)
		bool_scheduled[i]=false;

}

Cnoeud::Cnoeud(Cnoeud & father)
{
		cpteur++;
		// We copy the basic data
		LB=father.LB;
		partial_wCsum=father.partial_wCsum;
		partial_Cmax=father.partial_Cmax;
		partial_Lmax=father.partial_Lmax;
		Lmax_heur=father.Lmax_heur;
		wCsum_heur=father.wCsum_heur;

	// We copy the list of scheduled jobs
	scheduled = father.scheduled;
	// We copy the list of jobs remaining to schedule
	toschedule = father.toschedule;
	// ...list of candidates
	candidate = father.candidate;

	bool_scheduled=(bool*)malloc(nbre_travaux*sizeof(bool));

	//...list of booleans...
	for(int i=0;i<nbre_travaux;i++)
		 bool_scheduled[i]=father.bool_scheduled[i];
}

Cnoeud::~Cnoeud()
{
	cpteur--;
	free(bool_scheduled);
}

/*---------------------------------------------------------------------------
-----   NAME     : operator =                                           -----
-----------------------------------------------------------------------------
-----   Overload of the assignement operator. Is used to copy a father  -----
-----   node into a child node							                            -----
-----------------------------------------------------------------------------
-----   INPUT: The father node                                          -----
-----   OUTPUT: The child node                                          -----
---------------------------------------------------------------------------*/
Cnoeud & Cnoeud::operator =(Cnoeud & father)
{
 if (this != & father)
  {// The father node is not the child node

		// We copy the basic data
		wCsum_heur=father.wCsum_heur;
		LB=father.LB;
		partial_wCsum=father.partial_wCsum;
		partial_Cmax=father.partial_Cmax;
		partial_Lmax=father.partial_Lmax;
		Lmax_heur=father.Lmax_heur;


   // We copy the list of scheduled jobs
   scheduled = father.scheduled;
   // We copy the list of jobs remaining to schedule
   toschedule = father.toschedule;
	 // ...list of candidates
	 candidate = father.candidate;
	 //...list of booleans...
	 for(int i=0;i<nbre_travaux;i++)
		 bool_scheduled[i]=father.bool_scheduled[i];
  }
 return *this;
}

/*---------------------------------------------------------------------------
-----   NAME     : init_root                                            -----
-----------------------------------------------------------------------------
-----		This function must be ONLY called to initialize the root node		-----
-----------------------------------------------------------------------------
-----		Input	: Nothing																									-----
-----		needs: Nothing																									-----
-----		Output: Nothing																									-----
-----		implies: "toschedule" and "candidate" are initialized						-----
---------------------------------------------------------------------------*/
void Cnoeud::init_root()
{
	int i;

	// Construction de "toschedule"
	for(i=1; i<=nbre_travaux; i++)
	{
		partial_Cmax+=pi(i);
		toschedule.insert_alafin(i);
	}

	// Construction de "candidate"
	for(i=nbre_travaux; i>=1; i--)
		if(ditilde(toschedule.element[i-1])>=partial_Cmax)
			//Le travail est candidat
			candidate.insert_alafin(toschedule.extract_position_pasdetri(i));

	// Ainsi, "toschedule" n'est PAS TRIEE !!
	// et "candidate" non plus !

}    


