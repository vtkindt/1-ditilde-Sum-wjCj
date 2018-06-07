#pragma once
#include "common.h"

extern int SearchStrategy;
extern int CutActive,CutDone;
extern long long nbPolluted;
extern int LBStrategy;
extern int WSPT[maxtrav], Smith[maxtrav];
extern int nblambda;
extern int jobdep[maxtrav*maxtrav];
extern int jobarr[maxtrav*maxtrav];
extern double lambdabase[maxtrav*maxtrav];
extern double witmp[maxtrav];
extern BIGINT lcpteur;

extern int ditilde(int travail);
extern int initial_ditilde(int travail);
extern int pi(int travail);
extern int wi(int travail);
extern int di(int travail);
extern int nb_trav();
extern int nbre_travaux;
extern BIGINT somme_pi;
extern void set_ditilde(int,int);
extern void lire_donnees(char * chemin);


