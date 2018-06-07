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

/* CLASS Cenumer
		variables : The variables describes in the hpp file
    nécessite: This class uses the classes ...
		... */

#include "Cenumer.hpp"
#include <conio.h>
#include <stdlib.h>
#include <time.h>
/*CORPS*/
extern int DomStrategy;

#pragma region solmemo
#include <cassert>
#include <process.h>
#include <algorithm>
using namespace std;
#define TRUE true
#define FALSE false
#define BOOL bool
int tmp[NB + 1];
//! best[l] should always be updated as the currently best solution of the current node on level l. 
//  For a non-leaf node, this value is updated by children nodes on level l+1, for leaf nodes it is updated by the node itself.
//!! it should only be updated when ub is improved. Dom conditions must be disabled in order to not loss the opt node
BIGINT best[NB + 1];//!血与泪 = { INT_MAX };
//! used like best but for storing the best lb of a node, computed as the least of all lb of its childen. 
//Note that the lb of the node itself should not be counted if it has children, since it is the lowest.
//!! All ending nodes must be considered, any miss would lead to a too high lb so that the opt sol may be missed. Dom conditions must be disabled.
BIGINT bestlb[NB + 1];
char memlb[NB + 1] = { 0 };  //! flag indicating for the current node, we should memo the opt sol or lb. Lb is considered when: a polluting global dominance condition is applied.
bool Stra6LBMemoOn = true; //! whether enable lb memo for strategy 6
//! when a node is solved, update it's father's best value. Save the current node if needed
//! PREREQUEST: best[niv] must store the optimal value of the current node
//! First piege found: the UB rsc when first computed before branching, may already have a good value. On a node, its children may either be cut or not cut but solved directly with a value worst than rsc. 
//		In this case the optimal value of that node cannot be obtained since its optimal node may be in the cut subtree. Basicly the whole node is implicitely cut.
//! When this is called, must be sure that what we got is the OPTIMAL sol
void UpdateBestOnSolve(Cnoeud * node, bool saveToDb, bool isLb){
	int niv = node->scheduled.taille;
	if (memlb[niv] == 1)isLb = TRUE;
	if (niv < 1 || (isLb && !Stra6LBMemoOn)) return;
	BIGINT sol = best[niv];//node could be a sentinel
	if (isLb){
		assert((sol == INT_MAX || memlb[niv] == 1) && bestlb[niv] < INT_MAX);
		sol = bestlb[niv];
	}
	else assert(sol != INT_MAX);
	//int Csumi[NB];

	//for (int i = 0, Ci = *Csumi = 0; i <= niv - 1; ++i){
	//	Ci +=  pi(node->scheduled.element[i]);
	//	if (i > 0)Csumi[i] = Ci + Csumi[i - 1]; else Csumi[0] = Ci;
	//}
	//PAUSE_ON(Csumi[node->niv-1] != node->brn);
	//update father's best
	int i = niv - 1;
	BIGINT Ci = (node->partial_Cmax + pi(node->scheduled.element[niv - 1])) * wi(node->scheduled.element[niv - 1]);
	//if (i > 0) Ci -= Csumi[i - 1];				//! Cost value of the last prefixed job
	BIGINT t0 = 0;	//! t0 of the remaining part: we are in backward branching here
	if (saveToDb){
		i = niv;
		//check sol (not considering lb memo)
		if (FALSE && !isLb)//node->niv == 33)// && sol==28823)
		{
			FILE*f = fopen("donnees.dat", "w");
			for (int i403 = 0; i403 <= node->toschedule.taille; ++i403)
				fprintf(f, "%d %d %d %d %d\n", i403 + 1, pi(node->toschedule.element[i403]), wi(node->toschedule.element[i403]), di(node->toschedule.element[i403]), ditilde(node->toschedule.element[i403]));
			fclose(f);
			spawnl(P_WAIT, "pvw_600w.exe", "pvw_600w.exe", NULL);
			f = fopen("pvw.txt", "r");
			int op = -1; double tmp;
			fscanf(f, "%lf\n", &tmp);
			fscanf(f, "%d\n", &op);
			fclose(f);
			//int trueSol = op + (nb_task - i)*t0;
			////if (isLb && sol > trueSol || !isLb && sol != trueSol){
			if (!isLb && sol != op){
				printf("isLB=%d; %lld!=%d. n=%d, lb/ub=%lld,%lld\n", isLb, sol, op, node->toschedule.taille,node->LB, node->wCsum_heur);
				getch();
				sol = op;
			}
		}
		copy(node->toschedule.element, node->toschedule.element + node->toschedule.taille, tmp);
		copy(node->candidate.element, node->candidate.element + node->candidate.taille, tmp + node->toschedule.taille);
		DBAddPb(tmp, node->toschedule.taille+node->candidate.taille, t0, sol, isLb);	//! Note we do not store sequence. Only store jobset
	}
	if (sol + Ci < bestlb[niv - 1])
		bestlb[niv - 1] = sol + Ci; //! update father's bestlb: opt is also lb
	if (!isLb && sol + Ci < best[niv - 1])	best[niv - 1] = sol + Ci; //! update father's best
	if (memlb[niv] == 1)memlb[niv-1] = 1;
	best[niv] = BIGINT_MAX;					//! Empty levels should have their best value reset
	bestlb[niv] = BIGINT_MAX;
	memlb[niv] = 0;
}
#pragma endregion solmemo



int key(C_heap & scheduled)
{
 int i,Sum=0;

 for (i=0; i<scheduled.taille; i++) Sum += scheduled.element[i];
 return ((scheduled.taille-1)*HASHITEMS + (Sum % HASHITEMS)+1);
}


Cenumer::Cenumer()
{
	
	LBroot=-99999999;
	BestUB=999999999;
	nb_opt=0;
	nb_nodes=0;
	nblevel=0;
	nb_nodes_total_enum=0;
	if ((SearchStrategy==0)||(SearchStrategy==3) || SearchStrategy==6)
	{
	 Nodelist = new Cnodelist();
	} else
	{
	 if (SearchStrategy!=5)
		Levellist = new Cnodelist [nbre_travaux+1];
	  else 
		Levellist = new Cnodelist [HASHITEMS*nbre_travaux+1];
	}
	NodelistSuiv = new Cnodelist();
	BestVectCrit = new C_VectCrit(MAXVALUE,-999999); //!Leak reported
	//for(int i=0;i<nbre_travaux; i++)	Donelist[i]=new Cnodelist();
	if ((SearchStrategy==3)||(SearchStrategy==4) || SearchStrategy==6)
		AllocDB(DBMaximumDimension, nbre_travaux);
	Nbmax=0;
	fois=0;
}

void Cenumer::initialize_seqref()
{
	BIGINT i, j, k, pos, UB, taille, date, P,oldtaille;
	double LB,lambdabasetmp[maxtrav][maxtrav];
	C_heap * candidats;	C_heap * toschedule;
	
	// On applique l'algorithme Smith pour calculer la séquence de référence
	taille=nbre_travaux;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux);	//	pas triée

	// Copie courante (triée) de la liste des travaux candidats
	P=0;
	for (i=0;i<nbre_travaux;i++) P+=pi(i+1);
	for(i=0; i<nbre_travaux; i++)
		toschedule->insert_pisurwi_croissant(i+1);

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=nbre_travaux; date=P;
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		j=candidats->element[0];
	
		// affectation du travail choisi
		
		Smith[pos-1]=candidats->extract_pisurwi_croissant();

		date-=pi(j);
		
		pos--;
	}

	// On applique l'algorithme WSPT pour calculer la séquence de référence

	for(i=0; i<nbre_travaux; i++)
		toschedule->insert_pisurwi_croissant(i+1);

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=nbre_travaux; date=P;
	while(pos>0)
	{// position "pos" = 1..truc

		// affectation du travail choisi
		
		WSPT[pos-1]=toschedule->extract_pisurwi_croissant();

		date-=pi(WSPT[pos-1]);
		
		pos--;
	}

	delete candidats; delete toschedule;

		// On calcule maintenant l'ensemble des lambdabase
	for (i=0;i<nbre_travaux;i++)
		for (pos=0;pos<nbre_travaux;pos++)
			lambdabasetmp[i][pos]=0;
    /* Nouvelle méthode : on applique Smith et on ne calcule des
	   lambdabase qu'entre travaux candidats */
	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant //!Leak reported
	toschedule = new C_heap(nbre_travaux);	//	pas triée //!Leak reported

	for(i=0; i<nbre_travaux; i++)
		toschedule->insert_pisurwi_croissant(i+1);

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=nbre_travaux; date=P;
	oldtaille=0;
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if (pos==nbre_travaux)
		{
		 lcpteur+=candidats->taille*(candidats->taille-1)/2;
		 oldtaille=candidats->taille;
		} else 
		{
		 if (oldtaille<=candidats->taille)
			lcpteur+=(candidats->taille-oldtaille)*(2*candidats->taille-1-candidats->taille+oldtaille)/2;
		 oldtaille=candidats->taille;
		}

		// Calcule des lambdabase
		for (k=0;k<candidats->taille;k++)
			for (i=0;i<candidats->taille;i++)
				if ((k!=i)&&(wi(candidats->element[i])<wi(candidats->element[k]))&&(((double)pi(candidats->element[i])/(double)wi(candidats->element[i]))<((double)pi(candidats->element[k])/(double)wi(candidats->element[k]))))
				{
  					lambdabasetmp[candidats->element[i]-1][candidats->element[k]-1]=(double)wi(candidats->element[k])*(double)pi(candidats->element[i])/(double)pi(candidats->element[k])-(double)wi(candidats->element[i])-0.0001;
				}


		// affectation du travail choisi		
		j=candidats->element[0];	
		candidats->extract_pisurwi_croissant();
		date-=pi(j);
		pos--;
	}
	//printf("Pourcentage de lambdabase non nuls : %3.3lf\n",100.0*(double)lcpteur/(double)(nbre_travaux*nbre_travaux-nbre_travaux));
    //getch();
	// On met à jour les structures utilisées par la suite dans la LB
	nblambda=0;
	for (i=0;i<nbre_travaux;i++)
		for (k=0;k<nbre_travaux;k++)
			if (lambdabasetmp[i][k]!=0.0)
			{
			 jobdep[nblambda]=i+1;
			 jobarr[nblambda]=k+1;
			 lambdabase[nblambda]=lambdabasetmp[i][k];
			 nblambda++;
			}

	/* Ancienne méthode
	// On calcule maintenant l'ensemble des lambdabase
	for (i=0;i<nbre_travaux;i++)
		for (pos=0;pos<nbre_travaux;pos++)
			lambdabase[i][pos]=0;
	for (i=0;i<nbre_travaux;i++)
	{
	 for (pos=i+1;pos<nbre_travaux;pos++)
		 lambdabase[WSPT[i]-1][WSPT[pos]-1]=(double)wi(WSPT[pos])*(double)pi(WSPT[i])/(double)pi(WSPT[pos])-(double)wi(WSPT[i])-0.0001;
	}*/

	//!Leak Solved
	delete candidats; delete toschedule;
}

Cenumer::~Cenumer()
{
	int i;
	if ((SearchStrategy == 0) || (SearchStrategy == 3) || SearchStrategy==6)
		delete Nodelist;	
	delete NodelistSuiv;
	delete BestVectCrit;
	//for (i = 0; i < nbre_travaux; i++)	{
	//	try{ delete Donelist[i]; }
	//	catch (...){ printf("Delete exception occurs!\n"); }
	//}
	//! Leak solved. DB now freed.
	if ((SearchStrategy == 3) || (SearchStrategy == 4) || SearchStrategy == 6)
		FreeDB();
}
/*---------------------------------------------------------------------------
-----   NAME     : determiner_Eapp                                      -----
-----------------------------------------------------------------------------
-----				Cette fonction détermine une approximation de l'ensemble E	-----
-----				Cette approximation est mise dans la liste Eapp							-----
-----------------------------------------------------------------------------
-----   Input   : Nothing																								-----
-----   needs		: BestVectCrit contient le 1er opt [=Lex(wCsum, Lmax)]	-----
-----   Output	: Nothing																								-----
-----		implies	: Eapp contient l'approximation de E										-----
-----														( E privé de son 1er element	)					-----
---------------------------------------------------------------------------*/
void Cenumer::determiner_Eapp()
{
	bool faisable=true; bool premier;
	BIGINT i, j, pos, wCsum, last_wCsum, last_Lmax, date;
	signed long k;

	C_heap * candidats;
	C_heap * toschedule;
	C_heap * scheduled;
	
	
	// 1er élément de Eapp
	C_VectCrit *premier_opt;
	premier_opt = new C_VectCrit(BestVectCrit->wCsum,BestVectCrit->Lmax);

	Eapp.ajouter_element(premier_opt); // cette fonction n'alloue pas

	// Init
	last_Lmax=BestVectCrit->Lmax;
	last_wCsum=0;

	premier=true;
	// TQ on trouve une nelle solution
	while(faisable)
	{
printf(".");

		// Mise à jour des deadlines
		for(i=1;i<nbre_travaux+1;i++) set_ditilde(i,last_Lmax+di(i)-1);

		if(!premier) // 'premier' est mis à FAUX plus bas !
			last_wCsum=wCsum;

		pos=nbre_travaux; date=somme_pi; wCsum=0; last_Lmax=-999999;

		candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
		toschedule = new C_heap(nbre_travaux);	//	pas triée
		scheduled = new C_heap(nbre_travaux);

		// Création des tas:
			for(i=0;i<nbre_travaux;i++)
				if(ditilde(i+1)<date)
					toschedule->insert_alafin(i+1);
				else
					candidats->insert_pisurwi_croissant(i+1);

		// Application de l'heuristique de Smith
		
		while(pos>0)
		{// position "pos" = 1..truc

			if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
			{
				delete scheduled; delete toschedule; delete candidats;
				faisable = false; pos=0;
			}else{

				j=candidats->element[0];
				
				// affectation du travail choisi
				scheduled->insert_alafin(candidats->extract_pisurwi_croissant());

				// MAJ de LB, UB et date
				wCsum+=date*wi(j);
				last_Lmax=__max(last_Lmax,date-di(j));
				date-=pi(j);
				
				// MAJ des candidats
				for(k=toschedule->taille-1; k>=0; k--)
					if(ditilde(toschedule->element[k]) >= date )
						candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

				pos--;
			}
		}

		// On ajoute la solution à Eapp
		if(faisable)
		{
			delete scheduled;	delete candidats;	delete toschedule;

			if(wCsum>last_wCsum && !premier)
			{
				// On alloue un nouvel élément et on l'ajoute
				C_VectCrit *vect;
				vect = new C_VectCrit(wCsum,last_Lmax);
				Eapp.ajouter_element(vect);

			}else{
				// On modifie sa valeur
				C_VectCrit * vect;
				vect=Eapp[Eapp.taille-1];
				vect->modifier(wCsum,last_Lmax);
			}
			premier = false; // A laisser ici (nécessaire pour le if ci dessus)
		}
	}
	
	return;
}

/*---------------------------------------------------------------------------
-----   NAME     : initialize_enum                                      -----
-----------------------------------------------------------------------------
-----   This function generates (1) precedence-constraint						    -----
-----	constraints. Both are at least satisfied by one opt solution			-----
-----------------------------------------------------------------------------
-----   Input   : Nothing                                               -----
-----   needs: The data have been read                                  -----
-----   Output  : bool=false if WSPT is known to be optimal							-----
-----	  (only a Sufficient Condition is checked)												-----
---------------------------------------------------------------------------*/
void Cenumer::initialize_enum()
{
	bool again=true;
	C_heap *S;				// ensemble S  de PVW83 (TH1)
	C_heap *Sprime;		// ensemble S' de PVW83 (TH1)
	BIGINT i, j, k, s, t, val, mini, job;

	for(i=0;i<nbre_travaux;i++) if(ditilde(i+1)>somme_pi)
			set_ditilde(i+1,somme_pi);

	// Théorème 1 de PVW83

	S = new C_heap(nbre_travaux);
	for(job=1; job<=nbre_travaux; job++)
		S->insert_pisurwi_croissant(job);

	Sprime = new C_heap(nbre_travaux);
	Sprime->insert_ditilde_decroissant(S->extract_pisurwi_croissant());
	
	while( S->taille>0 && ditilde(S->element[0]) <= ditilde(Sprime->element[0]) )
		{Sprime->insert_ditilde_decroissant(S->extract_pisurwi_croissant());}

	for(j=0; j<Sprime->taille;j++)
		for(i=0; i<S->taille;i++)
			if(ditilde(S->element[i])<=ditilde(Sprime->element[j])
				//&&di(S->element[i])<=di(Sprime->element[j])
				)

				precedence(S->element[i],Sprime->element[j]);


	// Théorème 4 (de VTK & CE '01 ('02?) )

	for(t=0; t<nbre_travaux; t++)	for(s=0; s<nbre_travaux; s++)
	if(	pi(s+1)<=pi(t+1) && wi(s+1)>=wi(t+1) &&	di(s+1)<=di(t+1) && s!=t)
	{
		val=somme_pi;
		for(k=0;k<succ[s].taille;k++)	val-=pi(succ[s].element[k]);
		if((mini=__min(ditilde(s+1),val))<=ditilde(t+1))
		{
			if(!prec[t].rechercher_element(s+1)&&!prec[s].rechercher_element(t+1))
			{
				precedence(s+1,t+1);
				again=true;
			}
			set_ditilde(s+1,mini); //On met à jour des deadlines en utilisant le 
						// théorème 2 du rapport interne, i.e. ditilde=min(ditilde,XXX)
		}
	}

	delete S; delete Sprime;

}

/*---------------------------------------------------------------------------
-----   NAME     : solve_1ditilde						                            -----
-----------------------------------------------------------------------------
-----   This function solves the problem 1|ditilde|wCsum                -----
-----------------------------------------------------------------------------
-----   Input   :	three parameters used to display results (not important) --
-----   needs: The data have been read                                  -----
-----   Output  : Nothing                                               -----
-----		Implies : the problem is solved, the output file is written			-----
---------------------------------------------------------------------------*/
void Cenumer::solve_1ditilde(double L, double R, int iter)
{
	Cnoeud *courant;
	Cnodelist *fils = NULL;				// The list of the childs of father_node
	Cnodelist *fils_tries = NULL;	// The sorted list of the childs of father_node
	//unsigned long i;
	signed long j;
	clock_t start;
	short cut;
	BIGINT minlb, i, tempLB;

	if (SearchStrategy == 6) {
		DomStrategy = 1;
		printf("DomStrategy set to 1 for Strategy 6 (no effect)\n");
		for (i = 0; i < NB + 1; ++i) {
			best[i] = INT_MAX;
			bestlb[i] = INT_MAX;
			memlb[i] = 0;
		}
		//printf("70/6.txt should = 88787\n");
	}

	// Génération de contraintes de prec
	initialize_1ditilde();
	initialize_seqref();

	fils = new Cnodelist();
	courant = new Cnoeud();

	// The root node is initialized
	courant->init_root();	// Initialise la liste des travaux à placer,
															//	la liste ds candidats, la valeur de "partialCmax"...
	if (!calculerUBLB(courant))
	{
		delete courant;	delete fils; fils = NULL;
		return;
	}
	LBroot = courant->LB;
	// La ligne suivante est valable pour avoir le programme "bounds.exe"
	//return;

	if (LBroot != BestUB) { // We add the node if we dont have the optimal solution
		nb_nodes++;

		if ((SearchStrategy == 0) || (SearchStrategy == 3) || SearchStrategy == 6)
		{
			Nodelist->ajouter_element(courant);
		}
		else
		{
			Levellist[courant->scheduled.taille].ajouter_element_LB_croissant(courant);
			nblevel++;
		}
	}

	start = clock();
	i = 0;
	while (((SearchStrategy != 0) && (SearchStrategy != 3) && SearchStrategy != 6 && (nblevel != 0)) ||
		(((SearchStrategy == 0) || (SearchStrategy == 3) || SearchStrategy == 6) && (Nodelist->taille != 0)))
		///////// Traitement de la liste des noeuds ////
	{

		// Extraction of the father node
		if ((SearchStrategy == 0) || (SearchStrategy == 3) || SearchStrategy == 6)
		{  // Depth first
			courant = (*Nodelist)[Nodelist->taille - 1];
			Nodelist->Supprimer_position(Nodelist->taille - 1);
			cut = 0;
			int niv = courant->scheduled.taille;
			if (SearchStrategy == 3)
			{ // We look if the node is dominated by a done node
				if (cut_domine(courant)) // The father node is dominated
				{
					cut = 1;
					delete courant;
				}
			}
			else if (SearchStrategy == 6) {
				//printf("%d,%d,%d,%d\n", BestUB, nb_nodes, CutActive, CutDone);

				// We search in solved nodes
				if (courant->LB < 0) {//! This is a sentinel node. it is just solved and should be saved to database (solution memo)
					if (best[niv] != INT_MAX)//INTMAX when the node has no solution: all children are cut. we wanted to also memo this in order to indicate the node as not promising, however it is wrong since what is unpromising is not the subpb but the partial seq+subpb.
						UpdateBestOnSolve(courant, TRUE, FALSE);
					else {
						if (bestlb[niv] == INT_MAX) //! if lb is not updated by children, remember the computed one.
							bestlb[niv] = -courant->LB - 1 - courant->partial_wCsum;
						UpdateBestOnSolve(courant, TRUE, TRUE);	//! if some updates are missed, nodes may be cut innocently since the lb is higher than what it should be
					}
					cut = 1; //since this is a sentinel node
					delete courant;
				}
				else if (niv > 1) {
					//!Find sol from db
					BIGINT sol = 999999;
					int t0 = 0;
					int isLB = 0;
					copy(courant->toschedule.element, courant->toschedule.element + courant->toschedule.taille, tmp);
					copy(courant->candidate.element, courant->candidate.element + courant->candidate.taille, tmp + courant->toschedule.taille);

					if ((sol = DBSearchPb(tmp, courant->toschedule.taille + courant->candidate.taille, t0, &isLB)) != -1) {
						//! opt sol is also lb
						courant->LB = sol + courant->partial_wCsum;
						//assert(bestlb[v->niv] > sol); 
						bestlb[niv] = sol;
						//UpdateBestOnSolve(v, FALSE, TRUE);

						if (!isLB) {
							if (sol + courant->partial_wCsum <= BestUB) {
								best[niv] = sol;
								BestUB = sol + courant->partial_wCsum;
								//!also need to update its father. Only do this when sol<rsc since otherwise this is a cut node, which is not optimal. !!! Caution on this
								UpdateBestOnSolve(courant, FALSE, FALSE);
							}
							else {//not improving best, but it's still valid as a lb.
								//bestlb[v->niv] = sol;
								UpdateBestOnSolve(courant, FALSE, TRUE);
							}
							cut = 1;
							delete courant;
							CutDone++;
							//LOG("\nSolMemo cut size %d\n", nb_task - v->niv);
						}
						else if (Stra6LBMemoOn) {
							UpdateBestOnSolve(courant, FALSE, TRUE);
							if (sol + courant->partial_wCsum >= BestUB) {//Cut by UB
								cut = 1;
								delete courant;
								CutActive++;
								//LOG("\nLbMemo cut size %d\n", nb_task - v->niv);
							}
						}
					}
					else if (courant->LB >= BestUB) {//cut by ub, which may have been updated after the creation of node v
						bestlb[niv] = courant->LB - courant->partial_wCsum;
						UpdateBestOnSolve(courant, TRUE, TRUE);
						cut = 1;
						delete courant;
					}
				}

			}
		}

		if ((SearchStrategy == 1) || (SearchStrategy == 4))
		{  // Best first
			minlb = INT_MAX;
			for (i = 0; i <= nbre_travaux; i++)
				if ((Levellist[i].taille != 0) && (Levellist[i][0]->LB <= minlb))
				{
					courant = Levellist[i][0];
					minlb = Levellist[i][0]->LB;
				}
			Levellist[courant->scheduled.taille].Supprimer_position(0);
			nblevel--;
			if (courant->toschedule.taille == 0)
			{
				if (BestUB > courant->LB)
					BestUB = courant->LB;
				return;
			}
			cut = 0;
			if (SearchStrategy == 4)
			{ // We look if the node is dominated by an active node
			 /*if(actif_domine_best(courant)) // The father node is dominated
			 {
				 cut=1;
				 delete courant;
			 }	*/
				if ((cut == 0) && (cut_domine(courant))) // The father node is dominated
				{
					cut = 1;
					delete courant;
				}
			}
		}

		if (SearchStrategy == 2)
		{  // Breadth first without database management
			i = 0;
			while ((i <= nbre_travaux) && (Levellist[i].taille == 0)) i++;
			courant = Levellist[i][0];
			Levellist[courant->scheduled.taille].Supprimer_position(0);
			nblevel--;
			if (courant->toschedule.taille == 0)
			{
				if (BestUB > courant->LB)
					BestUB = courant->LB;
				return;
			}
			cut = 0;
		}

		if (SearchStrategy == 5)
		{  // Breadth first with database management
		 //i=0;
			while ((i <= nbre_travaux * HASHITEMS) && (Levellist[i].taille == 0)) i++;
			courant = Levellist[i][0];
			Levellist[i].Supprimer_position(0);
			nblevel--;
			if (courant->toschedule.taille == 0)
			{
				if (BestUB > courant->LB)
					BestUB = courant->LB;
				//return;
			}
			cut = 0;
			// We look if the node is dominated by an active node
			if (actif_domine(courant)) // The father node is dominated
			{
				cut = 1;
				delete courant;
			}
		}


		//! K perm. Not enabled for stra 6
		if ((SearchStrategy == 3 || SearchStrategy == 4) && cut == 0 && (DomStrategy & 4) && courant->scheduled.taille >= 3) {
			// The current node is not dominated so we try to generate alternative sequences to try 
			// to find one dominating the current node : in this case it is added to the database
			cut = DBGenerate(courant->scheduled.element, courant->partial_wCsum, courant->partial_Cmax, courant->scheduled.taille);
			if (cut != 0)delete courant; //! Mem leaks without this line
		}

		//Le noeud est ajouté à la liste des noeuds déjà branchés
		if (cut == 0)
		{
			int niv = courant->scheduled.taille;

			if (courant->scheduled.taille >= nbre_travaux)
			{	//--- Le noeud est une feuille ---
			   // on regarde s'il est le meilleur pour l'instant
				if (courant->LB <= BestUB)			// La LB donne le wCsum !! Lei:add = in order to update best
				{
					Bestsol = courant->scheduled;	// On conserve la solution
					BestUB = courant->LB;
					if (SearchStrategy == 6) {
						bestlb[niv] = best[niv] = 0;
						UpdateBestOnSolve(courant, FALSE, FALSE);//!no need to save empty node
					}
				}
				else	if (SearchStrategy == 6) {
					// Not improving ub, still a good lb
					bestlb[niv] = 0;
					UpdateBestOnSolve(courant, FALSE, TRUE);
				}
			}
			else
			{
				// [A] SEPARATION
				separation(courant, fils);
				tempLB = courant->LB;
				if ((courant->scheduled.taille > 0) && ((SearchStrategy == 3) || (SearchStrategy == 4)))
				{ // We add the node to the done list
				 //Donelist[courant->scheduled.taille-1]->ajouter_element(courant);
					DBAdd(courant->scheduled.element, courant->partial_wCsum, courant->partial_Cmax, courant->scheduled.taille, -1, NULL);
					delete courant;
				}
				else if (fils->taille > 0 && courant->scheduled.taille > 1 && SearchStrategy == 6) {
					//! add courant as sentinel
					courant->LB = -courant->LB - 1;
					Nodelist->ajouter_element(courant);
					//so do not delete courant
				}
				else
					delete courant;

				if (fils->taille != 0)
				{
					if ((SearchStrategy == 0) || (SearchStrategy == 3) || SearchStrategy == 6) {
						fils_tries = new Cnodelist();//!Leak reported
					}

					while (fils->taille != 0)
					{	// Extraction du noeud fils
						courant = (*fils)[fils->taille - 1];
						fils->Supprimer_position(fils->taille - 1);
						if (courant->Lmax_heur == -1234) {//! a polluted node: cut by a dominance based on scheduled jobs.
							//printf("polluted!!!!!!!!!!\n"); getchar();
							memlb[courant->scheduled.taille - 1] = 1;
							++nbPolluted;
						}
						//! search lb from database.!!! Disabled to be sure: the calculerublb may also update BestUB
						//int isLb = 0, sol=999999;
						//if (SearchStrategy == 6 && courant->scheduled.taille > 1 && Stra6LBMemoOn){
						//	//!Find sol from db
						//	if ((sol = DBSearchPb(courant->toschedule.element, courant->toschedule.taille, 0, &isLb)) != -1){
						//		courant->LB = sol + courant->partial_wCsum;	//! we donot propagate this now: it will be done when the node is extracted.
						//	}
						//}


						// Test des conditions de coupe
						// et ajout éventuel à la liste des noeuds actifs
						//if ((sol < 999999 && courant->LB >= BestUB) || !calculerUBLB(courant) || courant->LB >= BestUB)
						BIGINT oldBest = BestUB;
						bool callu = calculerUBLB(courant);//! node is only modified on lb ub, not job lists
						if (!callu || courant->LB >= BestUB)
						{//!Need to distinguish: cut bad lb, solved bad ub, solved good ub. Whether we are having a opt sol or only lb for this node?
							if (SearchStrategy == 6) {
								if (courant->LB == courant->wCsum_heur && courant->wCsum_heur == BestUB)
								{//! the node is implicitely solved and the resulting ub is improving
									//CutActive++;
									best[courant->scheduled.taille] = bestlb[courant->scheduled.taille] = BestUB - courant->partial_wCsum;
									UpdateBestOnSolve(courant, TRUE, FALSE);
								}
								else
								{
									// We can add this binf since it has no child. //! bug fixed: for lb, no end nodes should be omitted for the update of lb. Otherwise lb may be greater than it should and the node be cut by fault
									bestlb[courant->scheduled.taille] = courant->LB - courant->partial_wCsum;
									UpdateBestOnSolve(courant, TRUE, TRUE);
								}
							}
							delete courant;
						}
						else if (courant->Lmax_heur == -1234) {
							//! We'll not add this one since it's already cut by a polluting dominance condition
							bestlb[courant->scheduled.taille] = courant->LB - courant->partial_wCsum;
							UpdateBestOnSolve(courant, TRUE, TRUE);
							delete courant;
						}
						else
							// --- Le noeud va être ajouté ---( juste après le While...)
							// On trie la liste des noeuds à ajouter
						{
							if ((SearchStrategy == 0) || (SearchStrategy == 3) || SearchStrategy == 6)
								fils_tries->ajouter_element_LB_decroissant(courant);
							else
							{
								if ((SearchStrategy == 1) || (SearchStrategy == 4))
									Levellist[courant->scheduled.taille].ajouter_element_LB_croissant(courant);
								else
									if (SearchStrategy == 2)
										Levellist[courant->scheduled.taille].Inserer_position(courant, 0);
									else
										Levellist[key(courant->scheduled) - 1].Inserer_position(courant, 0);
								nblevel++;
							}
							if (tempLB > courant->LB)
							{
								//printf("Probleème\n");
								/*getch();
								exit(1);*/
							}
						}
					}
				}
				else {
					fils_tries = new Cnodelist();//!Leak reported
				}
				nb_nodes++;

				if ((SearchStrategy == 0) || (SearchStrategy == 3) || SearchStrategy == 6) {
					Nodelist->concatene(fils_tries); // Stratégie DEPTH FIRST (+ptit LB en dernier)
					delete fils_tries;//!Leak solved. Also removed the destructor of Cnodelist
				}
				Cnodelist::cpteur--;
			}
		}
		// Séparation et évaluation effectuées. On passe au noeud suivant...
	}
	if (fils != NULL)delete fils;
}

/*---------------------------------------------------------------------------
-----   NAME    : solve_enum						                                -----
-----------------------------------------------------------------------------
-----   This function solves the problem of enumeration                 -----
-----------------------------------------------------------------------------
-----   Input   :	three parameters used to display results (not important) --
-----   needs: The data have been read                                  -----
-----   Output  : Nothing                                               -----
-----		Implies : the problem is solved, the output file is written			-----
---------------------------------------------------------------------------*/
void Cenumer::solve_enum(double L, double R, int iter)//! never called
{

	Cnodelist *fils;				// The list of the childs of the father node
	Cnodelist *fils_tries;	// The sorted list of the childs of the father node
	Cnoeud *courant;	FILE  *partiel;
	time_t debut, time;	double temps;

	BIGINT last_Lmax, LmaxSUP;
	bool again; bool feasible;
	int i, k, job, current_time;

			// Calcul de la valeur du Lmax du 2nd optimum lexicographique
			// et calcul de wCsum associé
			// Ce calcul n'est utile que pour tester le 2d opt Lex à la fin de l'énum
					C_heap *EDD;		EDD = new C_heap(nbre_travaux);
					for(i=1;i<=nbre_travaux;i++)
						EDD->insert_ditilde_decroissant(i); // + petit ditilde à la racine
					wCsumSUP=0; current_time=0; LmaxSUP=-999999999;
					for(i=1;i<=nbre_travaux;i++)
					{
						job=EDD->extract_ditilde_decroissant();
						current_time+=pi(job);		wCsumSUP+=current_time*wi(job);
						LmaxSUP=__max(LmaxSUP,(signed long)(current_time-di(job)) );
					}
					delete EDD;

	// Résolut° du 1er sous-pb
	// (=faite ds le tri [qui est fait lors de la lecture des données]
	// ==>reste juste à inverser la liste des jobs)
	///////////////////////////////////
	current_time=0;BestVectCrit->Lmax=-999999; BestVectCrit->wCsum=0; feasible=true;
	
	for(i=0;i<nbre_travaux;i++){
		current_time+=pi(nbre_travaux-i); BestVectCrit->wCsum+=current_time*wi(nbre_travaux-i);
		BestVectCrit->Lmax=__max(BestVectCrit->Lmax,(signed long)(current_time-di(nbre_travaux-i)) );}
	partiel=fopen("PartialResults.txt","a+");
	fprintf(partiel,"%lu\t%1.1lf\t%1.1lf\t%lu\t%lu\t%ld\n",
		nbre_travaux,L,R,iter,BestVectCrit->wCsum,BestVectCrit->Lmax);
	fclose(partiel);

	nb_opt=1; last_Lmax=BestVectCrit->Lmax;

//printf("Determination de l'approximation de l'ensemble E\n...en cours...\t");
	debut=clock();

	determiner_Eapp();

//time=clock(); temps=((double)(time-debut)/CLOCKS_PER_SEC);
//printf("OK en %.2lf sec\n",temps);
//printf("Eapp=");for(i=0;i<Eapp.taille;i++) printf("(%ld,%ld),",Eapp[i]->wCsum,Eapp[i]->Lmax);printf("\n");
//printf("Execution de la PSE proprement dite:\n");	

	// on met wCsumSUP a jour connaissant Eapp
	wCsumSUP=__min(wCsumSUP,Eapp[Eapp.taille-1]->wCsum);

	// Génération de contraintes de prec valables pour TOUS les opt. cherchés
	initialize_enum();

	fils= new Cnodelist();
	debut=clock();
	while(feasible)
	{
//printf("\n\t\t\t\t  ET DE %ld : (%ld,%ld)\n", nb_opt,BestVectCrit->wCsum, BestVectCrit->Lmax);
printf("*");
//time=clock();
//temps=((double)(time-debut)/CLOCKS_PER_SEC);
//debut=time;
//printf("%.3lf\n",temps);

		// Mise à jour des deadlines
		for(i=1;i<=nbre_travaux;i++) set_ditilde(i,BestVectCrit->Lmax+di(i)-1);

		nb_nodes=0;
		// Initialisation de la meilleure solution pour l'opt. courant
		if(Eapp.taille>0)
		{
			BestVectCrit->wCsum=Eapp[0]->wCsum; BestVectCrit->Lmax=Eapp[0]->Lmax;
		}
		else
		{
			// Laisser cette initialisation (même si elle est pourrie) (utilisé pour détecter infaisabilité)
			BestVectCrit->wCsum=MAXVALUE;	BestVectCrit->Lmax=-999999999;
		}

		// Transfert de la liste des noeuds actifs
		Cnodelist *tmp; tmp=Nodelist;
		Nodelist=NodelistSuiv;
		NodelistSuiv=tmp;

		// On vide également la liste des noeuds développés
		for(k=0; k<nbre_travaux; k++){
			int maTailleDone=Donelist[k]->taille;
			for(i=1;i<=maTailleDone;i++){ // Vide la liste 'suiv'
				courant = (*(Donelist[k]))[Donelist[k]->taille-1];
				Donelist[k]->Supprimer_position(Donelist[k]->taille-1);
				delete courant;
			}
			printf("%d\n", Donelist[k]->taille);
		}
		
		MAJ_Nodelist(last_Lmax); // Mise à jour des noeuds qui ont été delayés
		
		again=true;
		if(nb_opt==1)// The (empty) root node has to be initialized
		{
			courant = new Cnoeud();	courant->init_root();
			again=calculerUBLB_enum(courant);
			if(again)
				Nodelist->ajouter_element(courant);
		}
		if(again){
		/////////////////////////////////////////////////////////////////////////////////////
			while (Nodelist->taille!=0) ///////// Traitement de la liste des noeuds ////
			{
				// Extraction of the father node
				courant=(*Nodelist)[Nodelist->taille-1];	
				Nodelist->Supprimer_position(Nodelist->taille-1);


//printf("ACTIF=");courant->scheduled.afficher();
				//Le noeud est ajouté à la liste des noeuds déjà branchés
				if(courant->scheduled.taille>0){
					Cnoeud *copie;
					copie= new Cnoeud(*courant);
					Donelist[courant->scheduled.taille-1]->ajouter_element(copie);
				}

				if(courant->scheduled.taille==nbre_travaux)
					evaluer_feuille(courant);
				else
				{
					separation_enum(courant, fils);
					delete courant;
					if(fils->taille!=0)
					{
						fils_tries = new Cnodelist();

						while(fils->taille!=0)
						{	// Extraction du noeud fils
							courant = (*fils)[fils->taille-1];
							fils->Supprimer_position(fils->taille-1);
							// Le noeud à conserver est (peut-être) mis dans fils_tries 
							if(traitement_fils(courant))
							{
								fils_tries->ajouter_element_LB_decroissant(courant);
//printf("\tOn conserve le fils ");courant->scheduled.afficher();
							}
						}						

						Nodelist->concatene(fils_tries); // Stratégie DEPTH FIRST (+ptit LB en dernier)
						Cnodelist::cpteur--;
					}
					nb_nodes++;
				}
			}// noeud suivant
		}else{
			// Infaisabilité détéctée dès la racine
			delete courant;
		}

		// Etude des résultats de la résolution du sous-problème
		if(BestVectCrit->wCsum!=MAXVALUE) // Else = unfeasible
		{
			// Ecriture des résultats partiels
			partiel=fopen("PartialResults.txt","a+");
			fprintf(partiel,"%lu\t%1.1lf\t%1.1lf\t%lu\t%lu\t%ld\n",
				nbre_travaux,L,R,iter,BestVectCrit->wCsum,BestVectCrit->Lmax);
			fclose(partiel);

			last_Lmax=BestVectCrit->Lmax;
			nb_opt++;

			// Retirer les éléments dominés de Eapp
			for(i=0;i<Eapp.taille;i++)
			{
				if(Eapp[i]->Lmax>=last_Lmax)
				{
					C_VectCrit * vect;
					vect=Eapp[i];
					Eapp.Supprimer_position(i);
					delete vect;
					i--; // car 1 element est supprimé
				}else
					break;
			}

		}else feasible=false;

	nb_nodes_total_enum+=nb_nodes;


//printf("\n_%ld noeud(s) non desalloue(s) et ",Cnoeud::cpteur);
//printf("%ld développe(s)",nb_nodes);
//printf(" %ld-%ld ",NodelistSuiv->taille,nb_nodes);
//printf("%ld ",nb_nodes);
//if(nb_nodes!=0 && nb_opt!=2)
//	getch();

	
	}//While(feasible)
printf("=%lld\n",nb_nodes_total_enum);
	if(last_Lmax!=LmaxSUP)	{printf("\n\t\t\tTOUCHETTE\n");getch();}

//printf("\n");

}

/*---------------------------------------------------------------------------
-----   NAME     : traitement_fils                                      -----
-----------------------------------------------------------------------------
-----   Cette fonction effectue les évaluations nécessaires sur un fils -----
-----		nouvellement créé afin de déterminer s'il doit être conservé,		-----
-----		délayé ou bien coupé.																						-----
-----		Elle calcule notamment les bornes !															-----
-----------------------------------------------------------------------------
-----   Input   : Le noeud "courant" à évaluer.													-----
-----   needs:		Le noeud est cohérent.										            -----
-----   Output	:	TRUE si le noeud doit être conservé pour le problème	-----
-----							en cours, i.e. si il doit être ajouté à "Nodelist"		-----
-----							FALSE sinon																						-----
-----								i.e. si le noeud a été coupé ou delayé (dans la fct!) ---
-----   implies: 	Les bornes du noeud sont calculées ET									-----
-----							les MAJ nécessaires de Eapp sont faites ET						-----
-----							si possible il est coupé ou delayé sinon on renvoie TRUE --
---------------------------------------------------------------------------*/
bool Cenumer::traitement_fils(Cnoeud * courant)//!never called
{
	int i, j; Cnoeud * dominant; bool domine;

		// On met à jour la liste des travaux candidats (avant de calculer LB!)
		for(j=courant->toschedule.taille-1; j>=0;j--)
			if(ditilde(courant->toschedule.element[j])>=courant->partial_Cmax)
				courant->candidate.insert_alafin(
					courant->toschedule.extract_position_pasdetri(j+1));

		if(!calculerUBLB_enum(courant)){
			delete courant;
			return false;
		}

		if(courant->scheduled.taille==nbre_travaux) // c'est une feuille
		{
			evaluer_feuille(courant);
			return false;
		}

		else if(courant->LB<=wCsumSUP)
		{//potentiellement non dominé
			if(courant->LB>=BestVectCrit->wCsum)
			{// dominé (wCsum) par la meilleure solution actuelle
				if(courant->partial_Lmax>=BestVectCrit->Lmax)
				{// dominé (last_Lmax) par la meilleure solution actuelle			
//printf("\tOn coupe [Best domine 2 crit] ");courant->scheduled.afficher();
					delete courant;
					return false;
				}
				else// dominé uniquement sur le wCsum
				{
					dominant=NULL;
					cut_domine_enum_wCsum(courant,dominant);
					if(dominant==NULL)// aucun elem de Donelist ne domine sur les 2 crit
					{
						domine=false;
						for(i=0;i<Eapp.taille;i++)
						{
							if(courant->partial_wCsum>=Eapp[i]->wCsum && courant->partial_Lmax>=Eapp[i]->Lmax)
							{//'dominant'(de Eapp) domine 'courant' sur les 2 critères
//printf("\tOn coupe  [un elem de Eapp domine 2 crit] ");courant->scheduled.afficher();
								delete courant;
								return false;
							}
						}
						// Ici, 'courant' est forcément non dominé par Eapp
//printf("\tOn delaye  [Best domine wCsum seulement] ");courant->scheduled.afficher();
							NodelistSuiv->ajouter_element(courant);
//printf("\tOn MAJ Eapp avec ");courant->scheduled.afficher();
							MAJ_Eapp(courant->wCsum_heur,courant->Lmax_heur);
							return false;
					}
					else
					{
printf("\tOn coupe [Done domine 2 crit] ");courant->scheduled.afficher();
						delete courant;
						return false;
					}
				}
			}
			else// meilleur que BestVectCrit
			{
				dominant=NULL; 
				domine=cut_domine_enum_wCsum(courant,dominant);
				if(dominant!=NULL)
				{// un elem de Donelist domin 'courant' sur les 2 critères
//printf("\tOn coupe [Done domine 2 crit] ");courant->scheduled.afficher();
					delete courant;
					return false;
				}
				else
				{// Aucun  élément de Donelist ne domine "courant" sur les 2 critères
					if(domine) // un elem de Donelist domine sur le wCsum
					{
//printf("\tOn delaye [un elem de Done domine sur wCsum] ");courant->scheduled.afficher();
						NodelistSuiv->ajouter_element(courant);
//printf("\tOn MAJ Eapp avec ");courant->scheduled.afficher();
						MAJ_Eapp(courant->wCsum_heur,courant->Lmax_heur);
						return false;					
					}
					else
					{
						// On met à jour la meilleure solution faisable  si besoin
						if(courant->wCsum_heur<BestVectCrit->wCsum)
						{
							BestVectCrit->wCsum=courant->wCsum_heur;
							BestVectCrit->Lmax=courant->Lmax_heur;
						}
						// On va conserver le noeud
//printf("\tOn va conserver ");courant->scheduled.afficher();
						return true;
					}
				}
				
			}
		}else // i.e. courant->LB>wCsumSUP
		{
//printf("\tOn coupe [wCsumSUP] ");courant->scheduled.afficher();
			delete courant;
			return false;
		}
		

}
/*---------------------------------------------------------------------------
-----   NAME     : evaluer_feuille                                      -----
-----------------------------------------------------------------------------
-----   Cette fonction effectue l'ensemble des MAJ (Eapp et BestVectCrit) ---
-----		engendrées par la connaissance d'un noeud feuille.							-----
-----------------------------------------------------------------------------
-----   Input   : Le noeud "courant" à évaluer													-----
-----   needs:		courant est un noeud feuille						              -----
-----   Output	:	Nothing		                                            -----
-----   implies: 	"courant" est détruit et les MAJ de BestVectCrit et		-----
-----							de Eapp sont faites.																	-----
---------------------------------------------------------------------------*/
void Cenumer::evaluer_feuille(Cnoeud *courant)//!never called
{
	int i;
	bool MAJ_Eap;
//printf("\t\t\tEVAL FEUILLE ...\n");
	if(courant->partial_wCsum>=wCsumSUP){
//printf("\tOn coupe [wCsumSUP] ");courant->scheduled.afficher();
		delete courant;}else
	
	if(courant->partial_wCsum<BestVectCrit->wCsum || courant->partial_Lmax<BestVectCrit->Lmax)
	{// Le noeud feuille est potentiellement non dominé
		if(courant->partial_wCsum>BestVectCrit->wCsum && courant->partial_Lmax<BestVectCrit->Lmax)
		{	// "courant" pourra mener à un futur optimum
			//	On met a jour Eapp avec 'courant'
			//	sauf si courant est dominé par un element de Eapp
			MAJ_Eap = true;
			for(i=0;i<Eapp.taille;i++)
			{
				if(courant->partial_wCsum>=Eapp[i]->wCsum && courant->partial_Lmax>=Eapp[i]->Lmax)
				{
//printf("\tOn coupe [Eapp domine 2 crit] ");courant->scheduled.afficher();
					delete courant;
					MAJ_Eap = false;
					break;
				}
			}
			if(MAJ_Eap)
			{
//printf("\tOn MAJ Eapp avec ");courant->scheduled.afficher();
				MAJ_Eapp(courant->partial_wCsum, courant->partial_Lmax);
				// insère 'courant' dans Eapp et enlève les sol° dominées
				delete courant;
			}

		}else // 'courant' améliore la meilleure solution courante
		{	//i.e. courant->partial_wCsum<BestVectCrit->wCsum
			
			if(courant->partial_wCsum<BestVectCrit->wCsum && courant->partial_Lmax>BestVectCrit->Lmax)
			{ // On ajoute l'ancien meilleur vecteur à Eapp
				MAJ_Eapp(BestVectCrit->wCsum,BestVectCrit->Lmax);
				BestVectCrit = new C_VectCrit();
			}
			// On met à jour la meilleure solution courante
			BestVectCrit->modifier(courant->partial_wCsum,courant->partial_Lmax);
			delete courant;
		}
	}
	else
	{
//printf("\tOn coupe [Best domine 2 crit] ");courant->scheduled.afficher();
		delete courant;
	}

//printf("\t\t\t-FIN- EVAL FEUILLE\n");

}


////////////////////////////////////////////////////////////////////////
//	MAJ_Eapp
//
// Cette fonction insère un vecteur de critères dans Eapp 
// et enlève les sol° dominées par ce vecteur de critères
//
// PRECondition : aucune solution de Eapp ne domine 'courant'
//
////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------
-----   NAME     : MAJ_Eapp		                                          -----
-----------------------------------------------------------------------------
-----   Cette fonction met à jour la liste Eapp des vecteurs de critères ----
-----		potentiellement non dominés à venir, en insérant un vecteur de  -----
-----		critère, et en enlevant les solutions dominées par ce vecteur.	-----
-----------------------------------------------------------------------------
-----   Input   : les valeurs des 2 critères du vecteur									-----
-----   needs:		Aucune solution de Eapp ne domine (même faiblement)		-----
-----							le vecteur à insérer.																	-----
-----   Output	:	Nothing		                                            -----
-----   implies: 	Eapp est mise à jour.												          -----
---------------------------------------------------------------------------*/
void Cenumer::MAJ_Eapp(BIGINT new_wCsum, BIGINT new_Lmax)
{
	int i;
//printf("((MAJ_Eapp...))");
	// On atteint la position d'insertion 
	i=0;
	while(i<Eapp.taille && Eapp[i]->wCsum<new_wCsum)
		i++;
	
	Eapp.Inserer_position(new C_VectCrit(new_wCsum,new_Lmax),i);

	// On enlève les éléments dominés
	while(i<Eapp.taille && Eapp[i]->Lmax>=new_wCsum)
		Eapp.Supprimer_position(i);
	
//printf("((-FIN- MAJ_Eapp...))\n");
//printf("Eapp=");for(i=0;i<Eapp.taille;i++) printf("(%ld,%ld),",Eapp[i]->wCsum,Eapp[i]->Lmax);printf("\n");
}

/*---------------------------------------------------------------------------
-----   NAME     : MAJ_Nodelist                                         -----
-----------------------------------------------------------------------------
-----   Cette fonction met à jour la liste "Nodelist" des noeuds actifs	-----
-----		qui ont été delayés. Elle teste que les noeuds sont faisables,	-----
-----		recalcule les bornes et met totalement les noeuds à jour (dont	-----
-----		'candidate'... )																								----- 
-----------------------------------------------------------------------------
-----   Input   : la valeur du Lmax de l'optima précédent								-----
-----   needs:			...																		              -----
-----   Output	:	néant			                                            -----
-----   implies: 		...																				          -----
---------------------------------------------------------------------------*/
void Cenumer::MAJ_Nodelist(BIGINT Lmax_opt_prec)//!never called
{
	int job, i, k, l;
	Cnoeud *noeud;

//printf("-MAJ_Nodelist(%ld)",Nodelist->taille);
	for(i=Nodelist->taille-1; i>=0;i--)
	{
		
		noeud =(*Nodelist)[i];
		Nodelist->Supprimer_position(i);
		
		if(noeud->partial_Lmax>=Lmax_opt_prec)
		{// On supprime le noeud : il est devenu infaisable
//printf("\tOn coupe [infaisable]");noeud->scheduled.afficher();
			delete noeud;
			continue;
		}
		
		// On reconstruit la liste des travaux candidats
		/////////////////////////////////////////////////
		for(k=noeud->candidate.taille-1; k>=0;k--)
			noeud->toschedule.insert_alafin(noeud->candidate.extract_position_pasdetri(k+1));
		for(k=noeud->toschedule.taille-1; k>=0;k--)
			if(ditilde(noeud->toschedule.element[k])>=noeud->partial_Cmax)
				noeud->candidate.insert_alafin(noeud->toschedule.extract_position_pasdetri(k+1));
		
		// Mise à jour de la liste des candidats du noeud
		// ( en respectant les précédences )
		for(k=0;(int)k<noeud->candidate.taille;k++)
		{
			job=noeud->candidate.element[k];
			for(l=noeud->candidate.taille-1;l>=0;l--) if(l!=k)
				// A-t-on "l-ieme candidat--->job" ?
				if(prec[job-1].rechercher_element(noeud->candidate.element[l])!=0)
					noeud->toschedule.insert_alafin(noeud->candidate.extract_position_pasdetri(l+1));
		}

	if(noeud->Lmax_heur>=Lmax_opt_prec)
		// On recalcule les bornes, et on essaye de couper/delayer
			if(traitement_fils(noeud))
				Nodelist->ajouter_element(noeud);
				// On est obligé de rajouter le noeud qu'on a extrait + tôt
				// si on veut utiliser la fonction 'traitement_fils' (cf spécifs...)
		
	}// Noeud suivant
//printf("FIN MAJ N(%ld)\n",Nodelist->taille);
}


/*---------------------------------------------------------------------------
-----   NAME     : separation                                      -----
-----------------------------------------------------------------------------
-----   Crée la liste des fils d'un père et applique 2 conditions				-----
-----			de dominances de Potts et al. (1983)			                    -----
-----		Attention : les fils ne sont pas finalisés (la liste des				-----
-----			candidats n'est pas mise à jour).															-----
-----------------------------------------------------------------------------
-----   Input   : the node to branch from AND the empty list where the  -----
-----								childs should be created                            -----
-----   needs: the node is already extracted from Nodelist							-----
-----						AND childs is an empty list								              -----
-----   Output  : nothing                                               -----
-----   implies: all done								                                -----   
---------------------------------------------------------------------------*/
void Cenumer::separation(Cnoeud * father, Cnodelist * childs)
{
	C_heap *candidats_reels; //	La liste des candidats pour la position courante:
													//		C'est un sous ensemble de "father->candidate"
	Cnoeud *noeud_fils;
	int k,i, j, val;	bool try_again;

	// (1) construction de la liste des candidats
	///////////////////////////////////////////////////////////////////////////
	candidats_reels = new C_heap(father->candidate.taille);
	*candidats_reels = father->candidate;

	//! This dom condition is about precedence, it should be kept but nodes should be evaluated on lb before being cut
	// (2) on enlève des candidats les travaux conduidant à une solution dominée
	////////////////////////////////////////////////////////////////////////////////
	for (i = candidats_reels->taille - 1; i >= 0; i--) // parcours inverse nécessaire
	{																						//	car la taille peut être modidiée
		try_again = true;

		//		(a)	les travaux ayant un successeur dans toschedule ou dans candidats
		for(j=0; j<father->toschedule.taille ; j++)
		{
			if((succ[candidats_reels->element[i]-1].rechercher_element(father->toschedule.element[j]))!=0)
			{
				// le i-eme elemt de cand_rl a un succssr ds "toschedule"
				if (SearchStrategy == 6)
					candidats_reels->element[i] = -candidats_reels->element[i]; //! Negatif to indicate that this should be cut. We keep it temporarily for to evaluate LB. No 0 elements
				else
					candidats_reels->extract_position_pasdetri(i+1);

				try_again = false;
				break;
			}
		}

		if(try_again)
		{
			for(j=0; j<father->candidate.taille ; j++)
			{
				if (candidats_reels->element[i]>=0 && (succ[candidats_reels->element[i] - 1].rechercher_element(father->candidate.element[j])) != 0)
				{
					// le i-eme elemt de cand_rl a un succssr ds "candidat"
					if (SearchStrategy == 6)
						candidats_reels->element[i] = -candidats_reels->element[i]; //! Negatif to indicate that this should be cut. We keep it temporarily for to evaluate LB.
					else
						candidats_reels->extract_position_pasdetri(i + 1);
					try_again = false;
					break;
				}
			}
		}
	}

	//! This condition is kept since not involved scheduled jobs.
	for(i=candidats_reels->taille-1; i>=0; i--) // parcours inverse nécessaire...
	{
		//	(b) les Ji tq il existe Jj dans candidats_reels tq pi<=pj et wi>=wj
		//	==> p383 col. 2 du papier de PVW83
			for(j=0; j<candidats_reels->taille ; j++){
				int oldI = candidats_reels->element[i];
				int oldJ = candidats_reels->element[j];
				if (candidats_reels->element[i] < 0)
					candidats_reels->element[i] = -candidats_reels->element[i];
				if (candidats_reels->element[j] < 0)
					candidats_reels->element[j] = -candidats_reels->element[j];
				if(i!=j &&
					pi(candidats_reels->element[i])<=pi(candidats_reels->element[j]) &&
					wi(candidats_reels->element[i])>=wi(candidats_reels->element[j]))
				{
					candidats_reels->element[j] = oldJ;
					candidats_reels->extract_position_pasdetri(i + 1);
					break;
				}
				candidats_reels->element[i] = oldI;
				candidats_reels->element[j] = oldJ;
			}
	}

	// (3) création des fils => un pour chaque élément de 'candidats_reels'
	///////////////////////////////////////////////////////////////////////////
	for(j=0; j<candidats_reels->taille; j++)
	{
		bool toBeCut = false;
		if (candidats_reels->element[j] < 0){
			toBeCut = true;
			candidats_reels->element[j] = -candidats_reels->element[j];
		}
		else if (candidats_reels->element[j] == 0){
			printf("Problem:candidats_reels->element[j] == 0!!!!!!Element is expected to start from 1.\nPause\n");
			getch();
		}
		noeud_fils = new Cnoeud();
		*noeud_fils=*father;
		// (i)	trouver la position dans "candidats" de "candidats_reels[j]"
		// (ii)	l'extraire de candidat
		// (iii)l'insérer dans "scheduled"
		noeud_fils->scheduled.insert_alafin(
			noeud_fils->candidate.extract_position_pasdetri(
				noeud_fils->candidate.rechercher_element(candidats_reels->element[j])
						));
		//MAJ du flag associé
		noeud_fils->bool_scheduled[candidats_reels->element[j]-1]=true;
		// (iv)mettre à jour les attributs du noeud
		noeud_fils->partial_wCsum+=wi(
			noeud_fils->scheduled.element[noeud_fils->scheduled.taille-1]
				)*noeud_fils->partial_Cmax;

		noeud_fils->partial_Lmax=__max(noeud_fils->partial_Lmax,
			noeud_fils->partial_Cmax-di(
				noeud_fils->scheduled.element[noeud_fils->scheduled.taille-1]	)	);
		noeud_fils->partial_Cmax-=pi(
			noeud_fils->scheduled.element[noeud_fils->scheduled.taille-1]
				);
		// Mise à jour de la liste des candidats
		for(k=noeud_fils->toschedule.taille-1; k>=0; k--)
			if(ditilde(noeud_fils->toschedule.element[k]) >= noeud_fils->partial_Cmax )
				noeud_fils->candidate.insert_pisurwi_croissant(noeud_fils->toschedule.extract_position_pasdetri(k+1));
		if (toBeCut)
			noeud_fils->Lmax_heur = -1234; //!used as flag
		childs->Inserer_position(noeud_fils,0);
		//nb_nodes++;
	}
	
	//! Test by permuting the last 2 jobs, including the one just branched. Involving a scheduled job, therefore polluting
	// (4) on coupe certains de ces fils (condition (C) du RI )
	///////////////////////////////////////////////////////////////////////////
	val=childs->taille;
	if(	val>0 &&	(*childs)[val-1]->scheduled.taille>1)
	{// On a ordonnancé au moins 2 travaux (et il y a au moins 1 fils)
		j=0; 
		while (j < val)
		{
			int Ji, Jj;// resp. dernier et avant-dernier travail placé

			noeud_fils = (*childs)[j];
			if (noeud_fils->Lmax_heur != -1234){//!skip nodes that are already marked as 'polluted'
				Ji = noeud_fils->scheduled.element[noeud_fils->scheduled.taille - 1];
				Jj = noeud_fils->scheduled.element[noeud_fils->scheduled.taille - 2];
				//if(	Jj>Ji && di(Ji)>=di(Jj) && ditilde(Ji)>=noeud_fils->partial_Cmax+pi(Jj)+pi(Ji) )
				// Version VTK du 30/12/2001
				if (ditilde(Ji) >= noeud_fils->partial_Cmax + pi(Jj) + pi(Ji))
				{ // On regarde s'il est intéressant de permuter les deux travaux
					if (((noeud_fils->partial_Cmax + pi(Jj))*wi(Jj) + (noeud_fils->partial_Cmax + pi(Jj) + pi(Ji))*wi(Ji))
						< ((noeud_fils->partial_Cmax + pi(Ji))*wi(Ji) + (noeud_fils->partial_Cmax + pi(Jj) + pi(Ji))*wi(Jj)))
					{// --- On l'enlève de la liste des fils à créer ---
						if (SearchStrategy == 6)
							noeud_fils->Lmax_heur = -1234;//!marked as polluted
						else{
							childs->Supprimer_position(j); // Ceci NE desalloue PAS le noeud
							delete noeud_fils;
							val--; j--;	// Décrémentations nécessaire à cause de la suppression d'une cellule
						}
					}
				}
			}
			j++;
		}
	}

	delete candidats_reels;
}

/*---------------------------------------------------------------------------
-----   NAME     : separation_enum                                      -----
-----------------------------------------------------------------------------
-----   Crée la liste des fils d'un père et applique 2 conditions				-----
-----			de dominances de Potts et al. (1983)			                    -----
-----		Attention : les fils ne sont pas finalisés (la liste des				-----
-----			candidats n'est pas mise à jour).															-----
-----------------------------------------------------------------------------
-----   Input   : the node to branch from AND the empty list where the  -----
-----								childs should be created                            -----
-----   needs: the node is already extracted from Nodelist							-----
-----						AND childs is an empty list								              -----
-----   Output  : nothing                                               -----
-----   implies: all done								                                -----   
---------------------------------------------------------------------------*/
void Cenumer::separation_enum(Cnoeud * father, Cnodelist * childs)//!never called
{
	C_heap *candidats_reels; //	La liste des candidats pour la position courante:
													//		C'est un sous ensemble de "father->candidate"
	Cnoeud *noeud_fils;
	int i, j, val;	bool try_again;

	// (1) construction de la liste des candidats
	///////////////////////////////////////////////////////////////////////////
	candidats_reels = new C_heap(father->candidate.taille);
	*candidats_reels = father->candidate;

	// (2) on enlève des candidats les travaux conduidant à une solution dominée
	////////////////////////////////////////////////////////////////////////////////
	for(i=candidats_reels->taille-1; i>=0; i--) // parcours inverse nécessaire
	{																						//	car la taille peut être modidiée
		try_again = true;

		//		(a)	les travaux ayant un successeur dans toschedule ou dans candidats
		for(j=0; j<father->toschedule.taille ; j++){
			if((succ[candidats_reels->element[i]-1].rechercher_element(father->toschedule.element[j]))!=0)
			{
				// le i-eme elemt de cand_rl a un succssr ds "toschedule"
				candidats_reels->extract_position_pasdetri(i+1);
				try_again = false;
				break;
			}
		}

		if(try_again){
			for(j=0; j<father->candidate.taille ; j++){
				if((succ[candidats_reels->element[i]-1].rechercher_element(father->candidate.element[j]))!=0)
				{
					// le i-eme elemt de cand_rl a un succssr ds "candidat"
					candidats_reels->extract_position_pasdetri(i+1);
					try_again = false;
					break;
				}
			}
		}
	}

	for(i=candidats_reels->taille-1; i>=0; i--) // parcours inverse nécessaire...
	{
		//	(b) les Ji tq il existe Jj dans candidats_reels tq pi<=pj et wi>=wj et di<=dj
		//	==>Théorème 4 (cf p383 PVW + condition sur di et dj)
			for(j=0; j<candidats_reels->taille ; j++){
				if(i!=j &&
					pi(candidats_reels->element[i])<=pi(candidats_reels->element[j]) &&
					wi(candidats_reels->element[i])>=wi(candidats_reels->element[j]) &&
					di(candidats_reels->element[i])<=di(candidats_reels->element[j]))
				{
					candidats_reels->extract_position_pasdetri(i+1);
					break;
				}
			}
	}

	// (3) création des fils => un pour chaque élément de 'candidats_reels'
	///////////////////////////////////////////////////////////////////////////
	
	for(j=0; j<candidats_reels->taille; j++)
	{
		noeud_fils = new Cnoeud();
		*noeud_fils=*father;
		// (i)	trouver la position dans "candidats" de "candidats_reels[j]"
		// (ii)	l'extraire de candidat
		// (iii)l'insérer dans "scheduled"
		noeud_fils->scheduled.insert_alafin(
			noeud_fils->candidate.extract_position_pasdetri(
				noeud_fils->candidate.rechercher_element(candidats_reels->element[j])
						));
		//MAJ du flag associé
		noeud_fils->bool_scheduled[candidats_reels->element[j]-1]=true;
		// (iv)mettre à jour les attributs du noeud
		noeud_fils->partial_wCsum+=wi(
			noeud_fils->scheduled.element[noeud_fils->scheduled.taille-1]
				)*noeud_fils->partial_Cmax;

		noeud_fils->partial_Lmax=__max(noeud_fils->partial_Lmax,
			noeud_fils->partial_Cmax-di(
				noeud_fils->scheduled.element[noeud_fils->scheduled.taille-1]	)	);
		noeud_fils->partial_Cmax-=pi(
			noeud_fils->scheduled.element[noeud_fils->scheduled.taille-1]
				);

		childs->ajouter_element(noeud_fils);
	}
	
	// (4) on coupe certains de ces fils (condition (C) du RI et condition sur di)
	///////////////////////////////////////////////////////////////////////////
	val=childs->taille;
	if(	val>0 &&	(*childs)[val-1]->scheduled.taille>1)
	{// On a ordonnancé au moins 2 travaux (et il y a au moins 1 fils)
		j=0; 
		while(j<val)
		{
			int Ji, Jj;// resp. dernier et avant-dernier travail placé

			noeud_fils = (*childs)[j];
			Ji=noeud_fils->scheduled.element[noeud_fils->scheduled.taille-1];
			Jj=noeud_fils->scheduled.element[noeud_fils->scheduled.taille-2];
			if(	Jj>Ji && di(Ji)>=di(Jj) && ditilde(Ji)>=noeud_fils->partial_Cmax+pi(Jj)+pi(Ji) )
			{// --- On l'enlève de la liste des fils à créer ---
				childs->Supprimer_position(j); // Ceci NE desalloue PAS le noeud
				delete noeud_fils;
				val--; j--;	// Décrémentations nécessaire à cause de la suppression d'une cellule
			}
			j++;
		}
	}

	delete candidats_reels;
}
/*---------------------------------------------------------------------------
-----   NAME     : calculerUBLB_enum                                    -----
-----------------------------------------------------------------------------
-----   computes the UB and the LB values for the given node            -----
-----------------------------------------------------------------------------
-----   Input   : the node to compute the UB and the LB of              -----
-----   needs: nothing														                      -----
-----   Output  : FALSE if there is no need to branch from this node    -----
-----							TRUE the node should be expand		 										-----
-----   implies: The UB and the LB values are modified if necessary     -----   
---------------------------------------------------------------------------*/
bool Cenumer::calculerUBLB_enum(Cnoeud * node)//! never called
{
	BIGINT i, j, k, pos, LB, UB, taille, date, job;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;
	double *lambda;	bool fin_de_bloc;

	taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux);	//	pas triée
	scheduled = new C_heap(nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
	lambda = (double *) malloc(taille*sizeof(double));
	for(i=0;i<taille; i++)	lambda[i]=-1;

	// Copie courante (triée) de la liste des travaux candidats
	for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwi_croissant(node->candidate.element[i]);
	// Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	*toschedule = node->toschedule;

//printf("B.");

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=taille; date=node->partial_Cmax;
	node->Lmax_heur=node->partial_Lmax;
	LB=node->partial_wCsum; UB=node->partial_wCsum; 
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			free(lambda);	delete scheduled; delete toschedule; delete candidats;
			return false;
		}

		j=candidats->element[0];
		
		// CALCUL DE LAMBDA_pos
		fin_de_bloc=true;
		if(pos!=taille)	
		{
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			for(i=1;i<candidats->taille; i++)
				if(	__min(node->partial_Cmax,ditilde(candidats->element[i])) > 
					__min(node->partial_Cmax,ditilde(candidats->element[0])) ){

					fin_de_bloc=false;
					break;
				}
			
			// ...et parmi "toschedule"
			if(fin_de_bloc)
				for(i=0;i<toschedule->taille; i++)
					if( __min(node->partial_Cmax,ditilde(toschedule->element[i])) >
						__min(node->partial_Cmax,ditilde(candidats->element[0]))){
						fin_de_bloc=false;
						break;
					}
			//calcul du multiplicateur
			if(fin_de_bloc)
				lambda[pos-1]=0;
			else{
				lambda[pos-1]=__max(0,
					pi(j)*(	wi(scheduled->element[scheduled->taille-1])+lambda[pos] )/
						pi( scheduled->element[scheduled->taille-1] )- wi(j)
					);
			}
		}else{// la position 'taille' correspond à une fin de bloc
			lambda[pos-1]=0;
		}

		// affectation du travail choisi
		scheduled->insert_alafin(candidats->extract_pisurwi_croissant());

		// MAJ de LB, UB et date
		node->Lmax_heur=__max(node->Lmax_heur,date-di(j));
		UB+=date*wi(j);
		LB+=__max(0,(signed long)(date*wi(j)+(signed long)(lambda[pos-1]*(double)((signed long)date-(signed long)ditilde(j)))));
		date-=pi(j);
		
		pos--;
	}

	node->LB=LB;	node->wCsum_heur=UB;

	if(LB==UB && node->scheduled.taille!=nbre_travaux)
		// On connait la meilleure solution à partir de ce noeud ( = feuille virtuelle)
	{
		// On conserve une copie du noeud pour les sous-problèmes suivants
		Cnoeud *noeud; 
		noeud = new Cnoeud(*node);
		NodelistSuiv->ajouter_element(noeud);
//printf("\tOn delaye la copie ");noeud->scheduled.afficher();

		// On transforme le noeud en noeud feuille pour le ss-pb courant:
		// (1) création de la séquence
		for(i=0;i<scheduled->taille; i++)
		{
			job=scheduled->element[i];
			node->scheduled.insert_alafin(job);
			node->bool_scheduled[job-1]=true;
		}

//printf("\tfeuilleVirtuelle ");node->scheduled.afficher();

		// (2) on vide "candidate" et "toschedule"
		for(k=node->candidate.taille; k>0; k--)
				node->candidate.extract_position_pasdetri(k);
		for(k=node->toschedule.taille; k>0; k--)
				node->toschedule.extract_position_pasdetri(k);
		// (3) calcul du Lmax
		date=0; node->partial_Lmax=-999999999;
		for(i=0;i<nbre_travaux;i++){
			date+=pi(node->scheduled.element[nbre_travaux-i-1]);
			node->partial_Lmax=__max(node->partial_Lmax,
								(signed long)(date-di(node->scheduled.element[nbre_travaux-i-1])) );
		}
		// (4) autres attributs
		node->partial_Cmax=0; node->partial_wCsum=UB;

	}

	free(lambda); delete scheduled; delete toschedule; delete candidats;
	return true;
}

/*---------------------------------------------------------------------------
-----   NAME     : calculerUBLB                                         -----
-----------------------------------------------------------------------------
-----   computes the UB and the LB values for the given node            -----
-----------------------------------------------------------------------------
-----   Input   : the node to compute the UB and the LB of              -----
-----   needs: nothing														                      -----
-----   Output  : FALSE if there is no need to branch from this node    -----
-----							TRUE the node should be expand		 										-----
-----   implies: The UB and the LB values are modified if necessary     -----   
---------------------------------------------------------------------------*/
bool Cenumer::calculerUBLB(Cnoeud * node)
{
 double LB,LB2,LBposn;
 BIGINT UB,UB2;
 bool res,res2;
 BIGINT Lmaxloc,Lmax2loc;
 
 if (LBStrategy==0)
 { // We only run the initial PVW lower bound
  res=UBLBpvw(node,&LB,&UB,&Lmaxloc);
  LBposn=0;
  //LBposner(node,&LBposn);
  node->LB=__max(LB,LBposn);	
  node->wCsum_heur=UB;
  node->Lmax_heur=Lmaxloc;  
  if ((res==true)&&(UB<BestUB))
  {
	  BestUB=UB;
  }
  if (node->LB==UB) return false;
  else  return res;
  //!any way return false if the node is solved
 }

 if (LBStrategy==1)//! The result table in paper 2004 should be this one
 { // We only run the new lower bound
  res=UBLBnew(node,&LB,&UB,&Lmaxloc);
  LBposner(node,&LBposn);
  node->LB=__max(LB,LBposn);	
  node->wCsum_heur=UB;
  node->Lmax_heur=Lmaxloc;  
  if ((res==true)&&(UB<BestUB))
  {
	  BestUB=UB;
  }
  if (node->LB==UB) return false;
  else  return res;
 }

 if (LBStrategy==2)
 { // We only run the initial PVW lower bound
  res=UBLBpvw(node,&LB,&UB,&Lmaxloc);
  res2=UBLBnew(node,&LB2,&UB2,&Lmax2loc);
  if ((res==true)&&(LB>=LB2))
  {
   node->LB=LB;	
   node->wCsum_heur=__min(UB,UB2);
   node->Lmax_heur=Lmaxloc;  
   if (__min(UB,UB2)<BestUB)
   {
	   BestUB=__min(UB,UB2);
   }
   return res;
	  } else
  {
   node->LB=LB2;	
   node->wCsum_heur=__min(UB2,UB);
   node->Lmax_heur=Lmax2loc;  
   if (__min(UB,UB2)<BestUB) 
   {
	   BestUB=__min(UB,UB2);
   }
   return res2;
  }
 }
 return false;
}

/*---------------------------------------------------------------------------
-----   NAME     : UBLBpvw                                           -----
-----------------------------------------------------------------------------
-----   Computes the UB and the LB values for the given node            -----
-----   using the original Potts & Van Wassenhove bounds                -----
-----------------------------------------------------------------------------
-----   Input   : the node to compute the UB and the LB of              -----
-----   needs: nothing														                      -----
-----   Output  : FALSE if there is no need to branch from this node    -----
-----							TRUE the node should be expand		 										-----
-----   implies: The UB and the LB values are computed if possible      -----   
---------------------------------------------------------------------------*/
bool Cenumer::UBLBpvw(Cnoeud * node, double *LB, BIGINT *UB,  BIGINT *Lmax)
{
	int i, j, k, pos, taille, date, job;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;
	double *lambda;	bool fin_de_bloc;

	taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux);	//	pas triée
	scheduled = new C_heap(nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
	lambda = (double *) malloc((taille+1)*sizeof(double));
	for(i=0;i<taille; i++)	lambda[i]=-1;

	// Copie courante (triée) de la liste des travaux candidats
	for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwi_croissant(node->candidate.element[i]);
	// Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	*toschedule = node->toschedule;

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=taille; date=node->partial_Cmax;
	node->Lmax_heur=node->partial_Lmax;
	*LB=node->partial_wCsum; *UB=node->partial_wCsum; *Lmax=node->Lmax_heur;
	BIGINT tmp = 0;
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			free(lambda);	delete scheduled; delete toschedule; delete candidats;
			return false;
		}

		j=candidats->element[0];
		
		// CALCUL DE LAMBDA_pos
		fin_de_bloc=true;
		if(pos!=taille)	
		{
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			// Version VTK : 21/12/01
			for(i=0;i<candidats->taille; i++)
				if(__min(node->partial_Cmax,ditilde(candidats->element[i]))>date){

					fin_de_bloc=false;
					break;
				}
			/*if (__min(node->partial_Cmax,ditilde(candidats->element[0]))==date) fin_de_bloc=true;
			else fin_de_bloc=false;*/
			/*for(i=1;i<candidats->taille; i++)
				if(	__min(node->partial_Cmax,ditilde(candidats->element[i])) > 
					__min(node->partial_Cmax,ditilde(candidats->element[0])) ){

					fin_de_bloc=false;
					break;
				}*/
			
			// ...et parmi "toschedule"
			/*if(fin_de_bloc)
				for(i=0;i<toschedule->taille; i++)
					if( __min(node->partial_Cmax,ditilde(toschedule->element[i])) >
						__min(node->partial_Cmax,ditilde(candidats->element[0]))){
						fin_de_bloc=false;
						break;
					}*/
			//calcul du multiplicateur
			if(fin_de_bloc)
				lambda[pos-1]=0;
			else{
				lambda[pos-1]=__max(0,
					(double)pi(j)*(	(double)wi(scheduled->element[scheduled->taille-1])+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)wi(j)
					);
			}
		}else{// la position 'taille' correspond à une fin de bloc
			lambda[pos-1]=0;
		}

		// affectation du travail choisi
		scheduled->insert_alafin(candidats->extract_pisurwi_croissant());

		// MAJ de LB, UB et date
		//node->Lmax_heur=__max(node->Lmax_heur,date-di(j));
		*Lmax=__max(*Lmax,date-di(j));
		*UB+=date*wi(j);
		*LB += ((BIGINT)date*wi(j) + ((BIGINT)lambda[pos - 1] * (date - ditilde(j))));
		//tmp += ((BIGINT)date*wi(j));
		//LB+=__max(0,(signed long)(date*wi(j)+(signed long)(lambda[pos-1]*(double)((signed long)date-(signed long)ditilde(j)))));
		date-=pi(j);
		
		pos--;
	}

	if (*UB<LBroot) 
	{
	 printf("UB<LBroot !!! %lld<%lld\n", *UB, LBroot );
	 getch();
	 exit(1);
	}


	//node->LB=LB;	node->wCsum_heur=UB;

	if(*LB==*UB)
	// On connait la meilleure solution à partir de ce noeud
	if(BestUB>*UB){// Elle améliore la meilleure solution
		BestUB=*UB;
		Bestsol=node->scheduled;
		for(i=0;i<scheduled->taille; i++)
			Bestsol.insert_alafin(scheduled->element[i]);

		free(lambda); delete scheduled; delete toschedule; delete candidats;
		return false;
	}

	free(lambda); delete scheduled; delete toschedule; delete candidats;
	return true;
}

/*---------------------------------------------------------------------------
-----   NAME     : UBLBposner                                           -----
-----------------------------------------------------------------------------
-----   Computes the LB value  for the given node            -----
-----   using the Posner's bound                -----
-----------------------------------------------------------------------------
-----   Input   : the node to compute the LB of              -----
-----   needs: nothing														                      -----
-----   Output  : FALSE if there is no need to branch from this node    -----
-----							TRUE the node should be expand		 										-----
-----   implies: The UB and the LB values are computed if possible      -----   
---------------------------------------------------------------------------*/
bool Cenumer::LBposner(Cnoeud * node, double *LB)
{
	int i, r, j, date, k, pos, taille, job, gap;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;	
	C_heap *T;
	double CBRK=0;
	double pitmp[maxtrav];
	double  witmp[maxtrav];
	double LBloc;

	taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux*nbre_travaux);		//	triée par pi/wi croissant
	T = new C_heap(nbre_travaux*nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux*nbre_travaux);	//	pas triée
	scheduled = new C_heap(nbre_travaux*nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
	for (i=0;i<nbre_travaux;i++) 
	{
		pitmp[i]=pi(i+1);
		witmp[i]=wi(i+1);
	}

	// Copie courante (triée) de la liste des travaux candidats
	for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwi_croissant(node->candidate.element[i]);
	// Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	*toschedule = node->toschedule;

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=taille; date=node->partial_Cmax;
	LBloc=node->partial_wCsum; 
	// MAJ des candidats
	for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));
	while(pos>0)
	{// position "pos" = 1..truc

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			delete scheduled; delete toschedule; delete candidats; delete T;
			return false;
		}

		j=candidats->element[0];

		// On construit la liste T
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date-pi(j) )
				T->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if (T->taille!=0) 
		{
			r=T->element[0];
			if ((pitmp[r-1]/witmp[r-1])>(pitmp[j-1]/witmp[j-1]))
			{ // On va chercher celui de plus grand ditilde
			 for (k=1;k<T->taille;k++) 
				 if ((ditilde(T->element[k])>ditilde(r))&&((pitmp[T->element[k]-1]/witmp[T->element[k]-1])>(pitmp[j-1]/witmp[j-1])))
					 r=T->element[k];
			}
		}

		if ((T->taille>0)&&((pitmp[r-1]/witmp[r-1])>(pitmp[j-1]/witmp[j-1])))
		{ // On va préempter le travail j
		 scheduled->insert_alafin(candidats->extract_pisurwi_croissant());

		 gap=date-ditilde(r);
		 LBloc=LBloc+(double)(date)*witmp[j-1]*(double)gap/(double)(pitmp[j-1]);
		 CBRK+=(double)(witmp[j-1]*(double)gap*(pitmp[j-1]-(double)gap))/(double)(pitmp[j-1]);

		 if ((pitmp[j-1]-date+ditilde(r))>0) 
		 {
		  witmp[j-1]=(double)witmp[j-1]*(double)((double)pitmp[j-1]-(double)gap)/(double)(pitmp[j-1]);
		  pitmp[j-1]=pitmp[j-1]+ditilde(r)-date;
		  candidats->insert_pisurwi_croissant(j);
		 } else pos--;
		 date=ditilde(r);
		 // On met à jour la liste candidats
		 while (T->taille!=0)
		 		toschedule->insert_alafin(T->extract_position_pasdetri(1));
	     for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));
		} else
		{
		 // affectation du travail choisi
		 scheduled->insert_alafin(candidats->extract_pisurwi_croissant());

		 // MAJ de LB et date
		 LBloc=LBloc+((double)(date)*(double)witmp[j-1]);
		 date=date-pitmp[j-1];
		 pos--;
		 for(k=T->taille-1; k>=0; k--)
				candidats->insert_pisurwi_croissant(T->extract_position_pasdetri(k+1));

		}		
	}

	(*LB)=LBloc+CBRK;
	delete scheduled; delete toschedule; delete candidats; delete T;
	return true;
}

/*---------------------------------------------------------------------------
-----   NAME     : UBLBnew                                           -----
-----------------------------------------------------------------------------
-----   Computes the UB and the LB values for the given node            -----
-----   using the modification of Potts & Van Wassenhove bounds                -----
-----------------------------------------------------------------------------
-----   Input   : the node to compute the UB and the LB of              -----
-----   needs: nothing														                      -----
-----   Output  : FALSE if there is no need to branch from this node    -----
-----							TRUE the node should be expand		 										-----
-----   implies: The UB and the LB values are computed if possible      -----   
---------------------------------------------------------------------------*/
bool Cenumer::UBLBnew(Cnoeud * node, double *LB, BIGINT *UB, signed BIGINT *Lmax)
{
	BIGINT loop, i, j, k, pos, taille, date, job, travdep, travarr;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;
	double *lambda;	bool fin_de_bloc;
	double LBtmp,oldwi;
	BIGINT UBtmp;
    BIGINT Lmaxtmp;

   taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer
   if (taille==1)
   {
	 date=node->partial_Cmax;
	 if (node->candidate.taille==0)
	 {
	  *UB=*LB=node->partial_wCsum+date*wi(node->toschedule.element[0]); 
	  *Lmax=__max(node->Lmax_heur,date-di(node->toschedule.element[0]));
	 } else
	 {
	  *UB=*LB=node->partial_wCsum+date*wi(node->candidate.element[0]); 
	  *Lmax=__max(node->Lmax_heur,date-di(node->candidate.element[0]));
	 }
	return true;
   } else
   {
   candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
   toschedule = new C_heap(nbre_travaux);	//	pas triée
   scheduled = new C_heap(nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
   lambda = (double *) malloc((taille+1)*sizeof(double));
   *LB=0;
   *UB=99999999;
   for (loop=0;loop<nblambda;loop++)
   if ((node->bool_scheduled[jobdep[loop]-1]==false)&&(node->bool_scheduled[jobarr[loop]-1]==false))
	{
     travdep=jobdep[loop];
	 travarr=jobarr[loop];
	 // On modifie les données
	 oldwi=witmp[travdep-1];
	 witmp[travdep-1]=oldwi+lambdabase[loop];

	 // On calcule la borne
	 for(i=0;i<taille; i++)	lambda[i]=-1;

	 // Copie courante (triée) de la liste des travaux candidats
	 for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwitmp_croissant(node->candidate.element[i]);
	 // Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	 *toschedule = node->toschedule;

	 // Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	 pos=taille; date=node->partial_Cmax;
	 LBtmp=node->partial_wCsum; UBtmp=node->partial_wCsum; Lmaxtmp=node->Lmax_heur;
	 while(pos>0)
	 {// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwitmp_croissant(toschedule->extract_position_pasdetri(k+1));

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			free(lambda);	delete scheduled; delete toschedule; delete candidats;
			witmp[travdep-1]=oldwi;
			return false;
		}

		j=candidats->element[0];
		
		// CALCUL DE LAMBDA_pos
		fin_de_bloc=true;
		if(pos!=taille)	
		{
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			// Version VTK : 21/12/01
			for(i=0;i<candidats->taille; i++)
				if(__min(node->partial_Cmax,ditilde(candidats->element[i]))>date){

					fin_de_bloc=false;
					break;
				}
			//calcul du multiplicateur
			if(fin_de_bloc)
				lambda[pos-1]=0;
			else{
				lambda[pos-1]=__max(0,
					(double)pi(j)*(	(double)witmp[scheduled->element[scheduled->taille-1]-1]+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)witmp[j-1]
					);
			}
		}else{// la position 'taille' correspond à une fin de bloc
			lambda[pos-1]=0;
		}

		// affectation du travail choisi
		scheduled->insert_alafin(candidats->extract_pisurwitmp_croissant());

		// MAJ de LB, UB et date
		//node->Lmax_heur=__max(node->Lmax_heur,date-di(j));
		Lmaxtmp=__max(Lmaxtmp,date-di(j));
		UBtmp+=date*wi(j);
		LBtmp+=(double)((double)date*(double)witmp[j-1]+(double)(lambda[pos-1]*(double)((double)date-(double)ditilde(j))));
		date-=pi(j);
		
		pos--;
	 }

	 if (UBtmp<LBroot) 
	 {
	  printf("UB<LBroot !!!\n");
	  getch();
	  exit(1);
	 }


	 if(LBtmp==UBtmp)
	 // On connait la meilleure solution à partir de ce noeud
	  if(BestUB>UBtmp){// Elle améliore la meilleure solution
		BestUB=UBtmp;
		Bestsol=node->scheduled;
		for(i=0;i<scheduled->taille; i++)
			Bestsol.insert_alafin(scheduled->element[i]);
		*UB=UBtmp;
		*LB=LBtmp;
		*Lmax=Lmaxtmp;

		free(lambda); delete scheduled; delete toschedule; delete candidats;
		witmp[travdep-1]=oldwi;
		return false;
	  }

 	 witmp[travdep-1]=oldwi;
	 scheduled->taille=0;
	 if (*LB<LBtmp)
	 {
	  *LB=LBtmp;
	 }
	 if (*UB>UBtmp)
	 {
	  *UB=UBtmp;
	  *Lmax=Lmaxtmp;
	 }
	}
 delete scheduled;
 free(lambda); delete toschedule; delete candidats;
 }
 return true;
}

/*bool Cenumer::UBLBnew(Cnoeud * node, double *LB, int *UB, signed int *Lmax)
{
	int i, j, k, l,pos, taille, date, job;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;
	double *lambda;	bool fin_de_bloc;
	int candidats_tmp[maxtrav],taille_tmp;

	taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux);	//	pas triée
	scheduled = new C_heap(nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
	lambda = (double *) malloc((taille+1)*sizeof(double));
	for(i=0;i<taille; i++)	lambda[i]=-1;

	// Copie courante (triée) de la liste des travaux candidats
	for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwi_croissant(node->candidate.element[i]);
	// Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	*toschedule = node->toschedule;

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=taille; date=node->partial_Cmax;
	node->Lmax_heur=node->partial_Lmax;
	*LB=node->partial_wCsum; *UB=node->partial_wCsum; *Lmax=node->Lmax_heur;
	l=nbre_travaux;
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			free(lambda);	delete scheduled; delete toschedule; delete candidats;
			return false;
		}

		l--;
		while (node->bool_scheduled[Smith[l]-1]==true) l--;
		j=Smith[l];
		
		// CALCUL DE LAMBDA_pos
		fin_de_bloc=true;
		if(pos!=taille)	
		{
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			// Version VTK : 21/12/01
			for(i=0;i<candidats->taille; i++)
				if(__min(node->partial_Cmax,ditilde(candidats->element[i]))>date){

					fin_de_bloc=false;
					break;
				}
			//calcul du multiplicateur
			if(fin_de_bloc)
			{
				lambda[pos-1]=0;
			}
			else{
				  lambda[pos-1]=__max(0,
					(double)pi(j)*(	(double)wi(scheduled->element[scheduled->taille-1])+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)wi(j)
					);
			}
		}else{// la position 'taille' correspond à une fin de bloc
			lambda[pos-1]=0;
		}

		// affectation du travail choisi
		taille_tmp=candidats->taille;
		for (k=0;k<taille_tmp;k++)
					candidats_tmp[k]=candidats->extract_pisurwi_croissant();
		for (k=0;candidats_tmp[k]!=Smith[l];k++) 
						candidats->insert_pisurwi_croissant(candidats_tmp[k]);
		k++;
		while (k<taille_tmp) 
		{
		 candidats->insert_pisurwi_croissant(candidats_tmp[k]);
		 k++;
		}
		scheduled->insert_alafin(j);

		// MAJ de LB, UB et date
		//node->Lmax_heur=__max(node->Lmax_heur,date-di(j));
		*Lmax=__max(*Lmax,date-di(j));
		*UB+=date*wi(j);
		*LB+=(double)((double)date*(double)wi(j)+(double)(lambda[pos-1]*(double)((double)date-(double)ditilde(j))));
		//LB+=__max(0,(signed long)(date*wi(j)+(signed long)(lambda[pos-1]*(double)((signed long)date-(signed long)ditilde(j)))));
		date-=pi(j);
		
		pos--;
	}

	if (*UB<LBroot) 
	{
	 printf("UB<LBroot !!!\n");
	 getch();
	 //exit(1);
	}


	//node->LB=LB;	node->wCsum_heur=UB;

	if(*LB==*UB)
	// On connait la meilleure solution à partir de ce noeud
	if(BestUB>*UB){// Elle améliore la meilleure solution
		BestUB=*UB;
		Bestsol=node->scheduled;
		for(i=0;i<scheduled->taille; i++)
			Bestsol.insert_alafin(scheduled->element[i]);

		free(lambda); delete scheduled; delete toschedule; delete candidats;
		return false;
	}

	free(lambda); delete scheduled; delete toschedule; delete candidats;
	return true;

}*/

/*bool Cenumer::UBLBnew(Cnoeud * node, double *LB, int *UB, signed int *Lmax)
{
	int i, j, k, pos, taille, date, job;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;
	double *lambda;	bool fin_de_bloc;
	int candidats_tmp[maxtrav],taille_tmp;

	taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux);	//	pas triée
	scheduled = new C_heap(nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
	lambda = (double *) malloc((taille+1)*sizeof(double));
	for(i=0;i<taille; i++)	lambda[i]=-1;

	// Copie courante (triée) de la liste des travaux candidats
	for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwi_croissant(node->candidate.element[i]);
	// Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	*toschedule = node->toschedule;

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=taille; date=node->partial_Cmax;
	node->Lmax_heur=node->partial_Lmax;
	*LB=node->partial_wCsum; *UB=node->partial_wCsum; *Lmax=node->Lmax_heur;
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			free(lambda);	delete scheduled; delete toschedule; delete candidats;
			return false;
		}

		j=candidats->element[0];
		
		// CALCUL DE LAMBDA_pos
		fin_de_bloc=true;
		if(pos!=taille)	
		{
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			// Version VTK : 21/12/01
			for(i=0;i<candidats->taille; i++)
				if(__min(node->partial_Cmax,ditilde(candidats->element[i]))>date){

					fin_de_bloc=false;
					break;
				}
			//calcul du multiplicateur
			if(fin_de_bloc)
			{
				lambda[pos-1]=0;
				j=candidats->extract_pisurwi_croissant();
			}
			else{
				// On va faire en sorte de minimiser la pénalité dans la 
				// LB
				taille_tmp=candidats->taille;
				for (k=0;k<taille_tmp;k++)
					candidats_tmp[k]=candidats->extract_pisurwi_croissant();
				for (k=0;(k<taille_tmp)
						&&((double)pi(candidats_tmp[k])/(double)wi(candidats_tmp[k])>(double)pi(scheduled->element[scheduled->taille-1])/(double)wi(scheduled->element[scheduled->taille-1]));k++);
				if (k==taille_tmp)
				{
				  lambda[pos-1]=__max(0,
					(double)pi(j)*(	(double)wi(scheduled->element[scheduled->taille-1])+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)wi(j)
					);
				  for (k=1;k<taille_tmp;k++) 
					  		candidats->insert_pisurwi_croissant(candidats_tmp[k]);
				}
				 else
				 {
				  for (j=0;j<k;j++) 
						candidats->insert_pisurwi_croissant(candidats_tmp[j]);
				  for (j=k+1;j<taille_tmp;j++)
						candidats->insert_pisurwi_croissant(candidats_tmp[j]);
 				  j=candidats_tmp[k];
				  lambda[pos-1]=__max(0,
					(double)pi(j)*(	(double)wi(scheduled->element[scheduled->taille-1])+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)wi(j)
					);
				 }
			}
		}else{// la position 'taille' correspond à une fin de bloc
			lambda[pos-1]=0;
			j=candidats->extract_pisurwi_croissant();
		}

		// affectation du travail choisi
		scheduled->insert_alafin(j);

		// MAJ de LB, UB et date
		//node->Lmax_heur=__max(node->Lmax_heur,date-di(j));
		*Lmax=__max(*Lmax,date-di(j));
		*UB+=date*wi(j);
		*LB+=(double)((double)date*(double)wi(j)+(double)(lambda[pos-1]*(double)((double)date-(double)ditilde(j))));
		//LB+=__max(0,(signed long)(date*wi(j)+(signed long)(lambda[pos-1]*(double)((signed long)date-(signed long)ditilde(j)))));
		date-=pi(j);
		
		pos--;
	}

	if (*UB<LBroot) 
	{
	 printf("UB<LBroot !!!\n");
	 getch();
	 //exit(1);
	}


	//node->LB=LB;	node->wCsum_heur=UB;

	if(*LB==*UB)
	// On connait la meilleure solution à partir de ce noeud
	if(BestUB>*UB){// Elle améliore la meilleure solution
		BestUB=*UB;
		Bestsol=node->scheduled;
		for(i=0;i<scheduled->taille; i++)
			Bestsol.insert_alafin(scheduled->element[i]);

		free(lambda); delete scheduled; delete toschedule; delete candidats;
		return false;
	}

	free(lambda); delete scheduled; delete toschedule; delete candidats;
	return true;

}*/

/* bool Cenumer::UBLBnew(Cnoeud * node, double *LB, int *UB, signed int *Lmax)
{
	int datetemp, i, j, nb, k, l, pos, taille, date, job, UBc,LBc;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;
	double *lambda;	bool fin_de_bloc;
	int candidats_tmp[maxtrav],taille_tmp;
	double maxcontrib,lambdamax,lambdacur;
	bool bool_scheduled[maxtrav];
    bool foundone;

	taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux);	//	pas triée
	scheduled = new C_heap(nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
	lambda = (double *) malloc((taille+1)*sizeof(double));
	for(i=0;i<taille; i++)	lambda[i]=-1;

	// Copie courante (triée) de la liste des travaux candidats
	for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwi_croissant(node->candidate.element[i]);
	// Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	*toschedule = node->toschedule;
	for (i=0;i<nbre_travaux;i++) bool_scheduled[i]=node->bool_scheduled[i];

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=taille; date=node->partial_Cmax;
	node->Lmax_heur=node->partial_Lmax;
	*LB=node->partial_wCsum; *UB=node->partial_wCsum; *Lmax=node->Lmax_heur;
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			free(lambda);	delete scheduled; delete toschedule; delete candidats;
			return false;
		}

        // Determination du prochain travail candidat
		j=candidats->element[0]; // Le travail choisit par la LB de PV&W
		// CALCUL DE LAMBDA_pos
		fin_de_bloc=true;
		if(pos!=taille)	
		{
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			// Version VTK : 21/12/01
			for(i=0;i<candidats->taille; i++)
				if(__min(node->partial_Cmax,ditilde(candidats->element[i]))>date){
					fin_de_bloc=false;
					break;
				}
			//calcul du multiplicateur
			if(fin_de_bloc)
				lambdamax=0;
			else{
				lambdamax=__max(0,
					(double)pi(j)*(	(double)wi(scheduled->element[scheduled->taille-1])+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)wi(j)
					);
			}
		}else{// la position 'taille' correspond à une fin de bloc
			lambdamax=0;
		}
		maxcontrib=(double)((double)date*(double)wi(j)+(double)(lambdamax*(double)((double)date-(double)ditilde(j))));
		taille_tmp=candidats->taille;
		for (k=0;k<taille_tmp;k++)
			candidats_tmp[k]=candidats->extract_pisurwi_croissant();
		foundone=false;
		for (k=1;k<taille_tmp;k++)
		{ // Pour tous les autres candidats
		 // On la contribution du candidat k
		 // CALCUL DE LAMBDA_pos
		 fin_de_bloc=true;
		 if(pos!=taille)	
		 {
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			// Version VTK : 21/12/01
			for(i=0;i<taille_tmp; i++)
				if(__min(node->partial_Cmax,ditilde(candidats_tmp[i]))>date){
					fin_de_bloc=false;
					break;
				}
			//calcul du multiplicateur
			if(fin_de_bloc)
				lambdacur=0;
			else{
				lambdacur=__max(0,
					(double)pi(candidats_tmp[k])*(	(double)wi(scheduled->element[scheduled->taille-1])+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)wi(candidats_tmp[k])
					);
			}
		 }else{// la position 'taille' correspond à une fin de bloc
			lambdacur=0;
		 }
         if (maxcontrib<((double)date*(double)wi(candidats_tmp[k])+(double)(lambdacur*((double)date-(double)ditilde(candidats_tmp[k])))))
		 {
		  maxcontrib=(double)((double)date*(double)wi(candidats_tmp[k])+(double)(lambdacur*((double)date-(double)ditilde(candidats_tmp[k]))));
		  lambdamax=lambdacur;
		  j=k;
		  foundone=true;
		 }
		}
		// On met à jour les listes 
		if (foundone==false)
			for (k=1;k<taille_tmp;k++)
				candidats->insert_pisurwi_croissant(candidats_tmp[k]);
		 else 
		 {
		  for (k=0;k<j;k++) 
				candidats->insert_pisurwi_croissant(candidats_tmp[k]);
		  for (k=j+1;k<taille_tmp;k++)
				candidats->insert_pisurwi_croissant(candidats_tmp[k]);
		  j=candidats_tmp[j];
		 }
		lambda[pos-1]=lambdamax;
		
		// affectation du travail choisi
		scheduled->insert_alafin(j);
		bool_scheduled[j-1]=true;

		// MAJ de LB, UB et date
		//node->Lmax_heur=__max(node->Lmax_heur,date-di(j));
		*Lmax=__max(*Lmax,date-di(j));
		*UB+=date*wi(j);
		*LB+=maxcontrib;
		//LB+=__max(0,(signed long)(date*wi(j)+(signed long)(lambda[pos-1]*(double)((signed long)date-(signed long)ditilde(j)))));
		date-=pi(j);
		
		pos--;
	}

	if (*UB<LBroot) 
	{
	 printf("UB<LBroot !!!\n");
	 getch();
	 exit(1);
	}


	//node->LB=LB;	node->wCsum_heur=UB;

	if(*LB==*UB)
	// On connait la meilleure solution à partir de ce noeud
	if(BestUB>*UB){// Elle améliore la meilleure solution
		BestUB=*UB;
		Bestsol=node->scheduled;
		for(i=0;i<scheduled->taille; i++)
			Bestsol.insert_alafin(scheduled->element[i]);

		free(lambda); delete scheduled; delete toschedule; delete candidats;
		return false;
	}

	free(lambda); delete scheduled; delete toschedule; delete candidats;
	return true;
}*/

/*bool Cenumer::UBLBnew(Cnoeud * node, double *LB, int *UB, signed int *Lmax)
{
	int datetemp, i, j, nb, k, l, pos, taille, date, job, UBc,LBc;
	C_heap * candidats;	C_heap * toschedule;	C_heap * scheduled;
	double *lambda;	bool fin_de_bloc;
	int candidats_tmp[maxtrav],taille_tmp;
	bool bool_scheduled[maxtrav];
    bool foundone;

	taille=nbre_travaux-node->scheduled.taille;// taille le la séquence à calculer

	candidats = new C_heap(nbre_travaux);		//	triée par pi/wi croissant
	toschedule = new C_heap(nbre_travaux);	//	pas triée
	scheduled = new C_heap(nbre_travaux);		//  classée mais pas triée...(c'est une séquence)
	lambda = (double *) malloc((taille+1)*sizeof(double));
	for(i=0;i<taille; i++)	lambda[i]=-1;

	// Copie courante (triée) de la liste des travaux candidats
	for(i=0; i<node->candidate.taille; i++)
		candidats->insert_pisurwi_croissant(node->candidate.element[i]);
	// Copie courante de la liste des travaux à ordonnancer et pas encore candidats
	*toschedule = node->toschedule;
	for (i=0;i<nbre_travaux;i++) bool_scheduled[i]=node->bool_scheduled[i];

	// Application de l'heuristique de Smith et calcul des mutliplicateurs de Lagrange
	pos=taille; date=node->partial_Cmax;
	node->Lmax_heur=node->partial_Lmax;
	*LB=node->partial_wCsum; *UB=node->partial_wCsum; *Lmax=node->Lmax_heur;
	while(pos>0)
	{// position "pos" = 1..truc

		// MAJ des candidats
		for(k=toschedule->taille-1; k>=0; k--)
			if(ditilde(toschedule->element[k]) >= date )
				candidats->insert_pisurwi_croissant(toschedule->extract_position_pasdetri(k+1));

		if(candidats->taille==0)// Détection de l'infaisabilité de tous les fils possibles
		{
			free(lambda);	delete scheduled; delete toschedule; delete candidats;
			return false;
		}

        // Determination du prochain travail candidat
		j=candidats->element[0]; // Le travail choisit par la LB de PV&W
		taille_tmp=candidats->taille;
		for (k=0;k<taille_tmp;k++)
			candidats_tmp[k]=candidats->extract_pisurwi_croissant();
		if (candidats_tmp[0]!=j)
		{
		 printf("Pb\n");
		 getch();
		}
		foundone=false;
		for (k=1;(k<taille_tmp)&&(foundone==false);k++)
		if (wi(candidats_tmp[k])<wi(j))
		//if ((wi(candidats_tmp[k])<=wi(j))&&(pi(candidats_tmp[k])<pi(j)))
		{ // Pour tous les autres candidats
		 // On calcule UBc
		 nb=0;UBc=0;
		 datetemp=date-pi(candidats_tmp[k]);
		 for (l=nbre_travaux-1;(l>=0)&&(nb<(toschedule->taille+taille_tmp-1));l--)
			 if ((candidats_tmp[k]!=Smith[l])&&(bool_scheduled[Smith[l]-1]==false))
			 {
			  nb++;
			  UBc+=datetemp*wi(Smith[l]);
			  datetemp-=pi(Smith[l]);
			 }
		 // On calcule LBc
		 nb=0;LBc=0;
		 datetemp=date-pi(j);
		 for (l=nbre_travaux-1;(l>=0)&&(nb<(toschedule->taille+taille_tmp-1));l--)
			 if ((j!=WSPT[l])&&(bool_scheduled[WSPT[l]-1]==false))
			 {
			  nb++;
			  LBc+=datetemp*wi(WSPT[l]);
			  datetemp-=pi(WSPT[l]);
			 }
		 // On compare
		 if ((signed int)(date*(wi(j)-wi(candidats_tmp[k])))>((signed int)UBc-(signed int)LBc))
		 {
		  j=candidats_tmp[k];
		  for (l=k;l<taille_tmp-1;l++) candidats_tmp[l]=candidats_tmp[l+1];
		  taille_tmp--;
		  foundone=true;
		 }
		}
		// On met à jour les listes 
		if (foundone==false)
			for (k=1;k<taille_tmp;k++)
				candidats->insert_pisurwi_croissant(candidats_tmp[k]);
		 else 
			for (k=0;k<taille_tmp;k++)
				candidats->insert_pisurwi_croissant(candidats_tmp[k]);

		
		// CALCUL DE LAMBDA_pos
		fin_de_bloc=true;
		if(pos!=taille)	
		{
			// On vérifie si le meilleur candidat est le travail de + grd ditilde
			// ...parmi "candidats"
			// Version VTK : 21/12/01
			for(i=0;i<candidats->taille; i++)
				if(__min(node->partial_Cmax,ditilde(candidats->element[i]))>date){
					fin_de_bloc=false;
					break;
				}
			if (__min(node->partial_Cmax,ditilde(j))>date){
					fin_de_bloc=false;
				}
			//calcul du multiplicateur
			if(fin_de_bloc)
				lambda[pos-1]=0;
			else{
				lambda[pos-1]=__max(0,
					(double)pi(j)*(	(double)wi(scheduled->element[scheduled->taille-1])+(double)lambda[pos] )/
						(double)pi( scheduled->element[scheduled->taille-1] )- (double)wi(j)
					);
			}
		}else{// la position 'taille' correspond à une fin de bloc
			lambda[pos-1]=0;
		}

		// affectation du travail choisi
		scheduled->insert_alafin(j);
		bool_scheduled[j-1]=true;

		// MAJ de LB, UB et date
		//node->Lmax_heur=__max(node->Lmax_heur,date-di(j));
		*Lmax=__max(*Lmax,date-di(j));
		*UB+=date*wi(j);
		*LB+=(double)((double)date*(double)wi(j)+(double)(lambda[pos-1]*(double)((double)date-(double)ditilde(j))));
		//LB+=__max(0,(signed long)(date*wi(j)+(signed long)(lambda[pos-1]*(double)((signed long)date-(signed long)ditilde(j)))));
		date-=pi(j);
		
		pos--;
	}

	if (*UB<LBroot) 
	{
	 printf("UB<LBroot !!!\n");
	 getch();
	 exit(1);
	}


	//node->LB=LB;	node->wCsum_heur=UB;

	if(*LB==*UB)
	// On connait la meilleure solution à partir de ce noeud
	if(BestUB>*UB){// Elle améliore la meilleure solution
		BestUB=*UB;
		Bestsol=node->scheduled;
		for(i=0;i<scheduled->taille; i++)
			Bestsol.insert_alafin(scheduled->element[i]);

		free(lambda); delete scheduled; delete toschedule; delete candidats;
		return false;
	}

	free(lambda); delete scheduled; delete toschedule; delete candidats;
	return true;
}*/

/*---------------------------------------------------------------------------
-----   NAME     : actif_domine                                         -----
-----------------------------------------------------------------------------
-----   check if the given node is dominated by another active node     -----
-----------------------------------------------------------------------------
-----   Input   : the node to check																			-----
-----   needs: nothing														                      -----
-----   Output  : TRUE if the node is dominated by another active node 	-----
-----							FALSE otherwise																				-----
-----   implies:	nothing																								-----   
---------------------------------------------------------------------------*/
bool Cenumer::actif_domine(Cnoeud * node)
{
	int i, j;
	bool dominated, dominate;
	Cnoeud *noeud;
	int val_key;

	val_key=key(node->scheduled);

	if(node->scheduled.taille>0)
	{
	    //if (Levellist[val_key-1].taille>Nbmax) Nbmax=Levellist[val_key-1].taille;
		//Nbmax+=Levellist[val_key-1].taille;
		//fois++;
		for(i=0;i<Levellist[val_key-1].taille;i++)
		{
			noeud=Levellist[val_key-1][i];

				if(noeud->partial_wCsum<=node->partial_wCsum )
				{// Le noeud courant est dominé par un noeud de la liste des noeuds actifs
					dominated = true;
					for(j=0;j<node->scheduled.taille; j++)
					{
						if( node->bool_scheduled[node->scheduled.element[j]-1] !=
								noeud->bool_scheduled[node->scheduled.element[j]-1] )
						{
							dominated=false;
							break;
						}
					}
					if(dominated)
					{
						CutActive++;
						return true;
					}
				}else{// Le noeud courant domine un noeud de la liste des noeuds actifs
					
					dominate = true;
					for(j=0;j<node->scheduled.taille; j++)
					{
						if( node->bool_scheduled[node->scheduled.element[j]-1] !=
								noeud->bool_scheduled[node->scheduled.element[j]-1] )
						{
							dominate=false;
							break;
						}
					}
					if(dominate)	
					{
						Levellist[val_key-1].Supprimer_position(i);
						nblevel--;
						CutActive++;
						delete noeud;
						//printf("coupe(%d/%d) ",node->scheduled.taille,val_key);
					}	
				}
		}
	}

	return false;
}

/*---------------------------------------------------------------------------
-----   NAME     : cut_domine																						-----
-----------------------------------------------------------------------------
-----   check if the given node is dominated by a already-expanded node -----
-----																																		-----
-----		A UTILISER AVEC LA VERSION DE BASE UNIQUEMENT										-----
-----		(i.e. pour le problème 1/ditilde/wCsum)													-----
-----------------------------------------------------------------------------
-----   Input   : the node to check																			-----
-----   needs: nothing														                      -----
-----   Output  : TRUE if the node is dominated by a already-expanded node --
-----							FALSE otherwise																				-----
-----   implies:	nothing																								-----   
---------------------------------------------------------------------------*/
bool Cenumer::cut_domine(Cnoeud * node)
{
	int i, j;	bool dominated;	Cnoeud *noeud;
	int taille=node->scheduled.taille,index2;

	if (taille>0)
		 { // We compare with done nodes
		  index2=100; 
	      if (DBAdd(node->scheduled.element, node->partial_wCsum, node->partial_Cmax, node->scheduled.taille, 1, &index2) == -1) 
		  { // The node v is dominated
			CutDone++;
			return true;
		  }
		 }

/*	if(taille>0)
	for(i=0;i<(Donelist[taille-1])->taille;i++)
	{
		noeud = (*Donelist[taille-1])[i];
		if(noeud->partial_wCsum<=node->partial_wCsum )
		{ // The current node is maybe dominated
			dominated = true;
			for(j=0;j<node->scheduled.taille; j++)
			{
				if( node->bool_scheduled[node->scheduled.element[j]-1] !=
						noeud->bool_scheduled[node->scheduled.element[j]-1] )
				{
					dominated=false;
					break;
				}
			}
			if(dominated)
			{
				CutDone++;
				return true;
			}
		} else
		{
			dominated = true;
			for(j=0;j<node->scheduled.taille; j++)
			{
				if( node->bool_scheduled[node->scheduled.element[j]-1] !=
						noeud->bool_scheduled[node->scheduled.element[j]-1] )
				{
					dominated=false;
					break;
				}
			}
			if(dominated)	
			{ // We delete the saved node
			 Donelist[taille-1]->Supprimer_position(i);
			 delete noeud;
			}
		}
	}*/


	return false;
}

/*---------------------------------------------------------------------------
-----   NAME     : cut_domine_enum_wCsum																-----
-----------------------------------------------------------------------------
-----   check if the given node is dominated by a already-expanded node -----
-----																																		-----
-----		A UTILISER AVEC LE PB D'ENUMERATION UNIQUEMENT									-----
-----																																		-----
-----------------------------------------------------------------------------
-----   Input   : un pointeur sur le noeud "node" à tester							-----
-----							un pointeur (=NULL) sur un noeud											-----
-----							(le 2ème est utilisé pour le retour de param)					-----
-----   needs: "node" est un noeud cohérent															-----
-----   Output  : un booléen qui vaut:																	-----
-----							VRAI si un élément de "Donelist" domine "node" sur le -----
-----							wCsum, et FAUX sinon																	-----
-----					(et l'argument "dominant" est éventuellement modifié:			-----
-----   implies:	"dominant" contient l'adresse du 1er noeud de					-----
-----							"Donelist" qui domine "node" sur les 2 critères, s'il -----
-----							existe, et NULL sinon.																-----
---------------------------------------------------------------------------*/
bool Cenumer::cut_domine_enum_wCsum(Cnoeud * node, Cnoeud * dominant)//!never called
{
	int i, j;
	bool dominated;
	// Indique que "node" est dominé sur le wCsum par 1 element de Donelist

	// 'dominant' est le 1er noeud de Donelist qui domine 'node' sur les 2 crit

	Cnoeud *noeud;

	int taille=node->scheduled.taille;
	dominated = false;

	if(taille>0)
	for(i=0;i<(Donelist[taille-1])->taille;i++)
	{
		noeud = (*Donelist[taille-1])[i];
		if(noeud->partial_wCsum<=node->partial_wCsum )
		{
			dominated = true;
			for(j=0;j<node->scheduled.taille; j++)
			{
				if( node->bool_scheduled[node->scheduled.element[j]-1] !=
						noeud->bool_scheduled[node->scheduled.element[j]-1] )
				{
					dominated=false;
					break;
				}
			}
			if(dominated && noeud->partial_Lmax<=node->partial_Lmax)
			{
				dominant = noeud;
				break;
			}
		}
	}
	return dominated;
}



/*---------------------------------------------------------------------------
-----   NAME     : precedence									                          -----
-----------------------------------------------------------------------------
-----   Ajoute une contrainte de précédence 														-----
-----------------------------------------------------------------------------
-----   Input   : le predecesseur a et le successeur b                  -----
-----		needs		: 1<=a<=nbre_travaux AND 1<=b<=nbre_travaux	AND a!=b		-----
-----				 AND a n'est pas déjà successeur de b, ni b successeur de		-----
-----   Output  : nothing																								-----
-----   inplies : a est ajouté à prec[b-1] AND b est ajouté à succ[a-1] -----
-----			 AND les précédences sont "propagées" sur les éléments de			-----
-----						 prec[a-1] et de succ[b-1] si elles n'y sont pas déjà		-----
---------------------------------------------------------------------------*/
void Cenumer::precedence(int a, int b)
{
	succ[a-1].insert_alafin(b);	// insertion en O(1)
	prec[b-1].insert_alafin(a);	// insertion en O(1)
}

/*---------------------------------------------------------------------------
-----   NAME     : initialize_1ditilde                                  -----
-----------------------------------------------------------------------------
-----   This function generates (1) reduced deadlines (2) precedence-   -----
-----	constraints. Both are at least satisfied by one opt solution			-----
-----		This function implements the initialization of PVW83.						-----
-----------------------------------------------------------------------------
-----   Input   : Nothing                                               -----
-----   needs: The data have been read                                  -----
-----   Output  : bool=false if WSPT is known to be optimal							-----
-----	  (only a Sufficient Condition is checked)												-----
---------------------------------------------------------------------------*/
void Cenumer::initialize_1ditilde()
{
	bool again=true, iter;
	C_heap *S;				// ensemble S  de PVW83 (TH1)
	C_heap *Sprime;		// ensemble S' de PVW83 (TH1)
	C_heap *tas_EDD, *liste_EDD;
	BIGINT i, j, k, s, t, val, mini, job,val2;

	tas_EDD = new C_heap(nbre_travaux);
	liste_EDD = new C_heap(nbre_travaux);
			

	for(i=0;i<nbre_travaux;i++)
		if(ditilde(i+1)>somme_pi)
			set_ditilde(i+1,somme_pi);

	// Théorème 1 de PVW83
	//////////////////////
	S = new C_heap(nbre_travaux);
	for(job=1; job<=nbre_travaux; job++)
		S->insert_pisurwi_croissant(job);

	Sprime = new C_heap(nbre_travaux);
	Sprime->insert_ditilde_decroissant(S->extract_pisurwi_croissant());
	
	while( S->taille>0 && ditilde(S->element[0]) <= ditilde(Sprime->element[0]) )
		{Sprime->insert_ditilde_decroissant(S->extract_pisurwi_croissant());}

	for(j=0; j<Sprime->taille;j++)
		for(i=0; i<S->taille;i++)
			if(ditilde(S->element[i])<=ditilde(Sprime->element[j]))

				precedence(S->element[i],Sprime->element[j]);
	// Fin du théorème 1

	for(i=0;i<nbre_travaux;i++)
		tas_EDD->insert_ditilde_decroissant(i+1);

	// création de la liste EDD (pour TH3)
	for(i=0;i<nbre_travaux;i++)
		liste_EDD->insert_alafin(tas_EDD->extract_ditilde_decroissant());
	
	while(again)// Tant qu'on génère de nouvelles contraintes
	{
		again=false;

		// Théorème 2 de PVW83
		///////////////////////////////////////
		iter=true;
		t=0;
		s=0;
		while (t<nbre_travaux)
		{
         while (s<nbre_travaux)
		 {
 		  if ((pi(s+1)<=pi(t+1)) && (wi(s+1)>=wi(t+1)) && (s!=t))
		  {
			val=somme_pi;
			for(k=0;k<succ[s].taille;k++)	val-=pi(succ[s].element[k]);
			if((mini=__min(ditilde(s+1),val))<=ditilde(t+1))
			{
				if((!prec[t].rechercher_element(s+1))&&(!prec[s].rechercher_element(t+1)))
				{
					precedence(s+1,t+1);
					again=true;
					iter=false;
				}
				set_ditilde(s+1,mini);
			}
		  }
		  if (iter==false)
		  {
		   iter=true;
		   t=0;
		   s=0;
		  } else s++; 
		 }
		 t++;
		}

		// Réduction des deadlines
		// NB:une réduction est faite aussi ds TH2 ci-dessus
		//////////////////////////////////////////
	    if(again)
			for(i=0; i<nbre_travaux; i++)		// i+1 est un travail
			{
			 val2=somme_pi;
			 for(j=0;j<succ[i].taille;j++)	  // succ[i].element[j] est un travail
			 {
				val=ditilde(succ[i].element[j])-pi(succ[i].element[j]);
				for(k=0;k<nbre_travaux;k++)
				{	// k+1 est un travail
					if(k!=i && k+1!=succ[i].element[j] && // ces deux conditions permettent
																								//	de gagner du temps ds la recherche
						 (succ[i].rechercher_element(k+1)!=0) &&
						 (prec[succ[i].element[j]-1].rechercher_element(k+1)!=0) 	)
						 val-=pi(k+1);
				}
				set_ditilde(i+1,__min(ditilde(i+1),val));
			    val2-=pi(succ[i].element[j]);
			 }
			 set_ditilde(i+1,__min(ditilde(i+1),val2));
			}
			
		//Recalcul de EDD car des ditilde ont pu changer
		for(i=0;i<nbre_travaux;i++)
			tas_EDD->insert_ditilde_decroissant(liste_EDD->extract_position_pasdetri(liste_EDD->taille));
		for(i=0;i<nbre_travaux;i++)
			liste_EDD->insert_alafin(tas_EDD->extract_ditilde_decroissant());

		// Théorème 3 de PVW83
		/////////////////////////////////////////////
		val=0;
		for(i=0;i<nbre_travaux;i++)
		{
			val+=pi(liste_EDD->element[i]); //val = somme des pi de edd(1] à edd(i+1)
			for(j=i+1;j<nbre_travaux;j++)
				if(val+pi(liste_EDD->element[j])>ditilde(liste_EDD->element[i]))
					for(k=0; k<=i; k++)
						/*if(j!=k && di(liste_EDD->element[k])<=di(liste_EDD->element[j]) &&
							(prec[liste_EDD->element[j]-1].rechercher_element(liste_EDD->element[k]))==0
							)*/
						// Version VTK du 30/12/2001
						if((prec[liste_EDD->element[j]-1].rechercher_element(liste_EDD->element[k]))==0)

						{
							precedence(liste_EDD->element[k],liste_EDD->element[j]);
							again=true;
						}
		}
	}
	delete S; delete Sprime;
	delete tas_EDD; delete liste_EDD;
}
