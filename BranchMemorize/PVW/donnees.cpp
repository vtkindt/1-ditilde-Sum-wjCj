#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "donnees.h"

struct {
 int ditilde[maxtrav];
 int initial_ditilde[maxtrav];
 int pi[maxtrav];
 int wi[maxtrav];
 int di[maxtrav];
} donnees ;

int nbre_travaux=0;
BIGINT somme_pi=0;
BIGINT lcpteur=0;

int SearchStrategy;
//! Pour des raison d'hygiène
int CleanStrategy = 0;
extern int DomStrategy; 

int CutActive = 0, CutDone = 0;long long nbPolluted = 0;
int LBStrategy;
int WSPT[maxtrav], Smith[maxtrav];
int nblambda;
int jobdep[maxtrav*maxtrav];
int jobarr[maxtrav*maxtrav];
double lambdabase[maxtrav*maxtrav];
double witmp[maxtrav];


int nb_trav()
{
 return(nbre_travaux);
}

int ditilde(int travail)
{
 return(donnees.ditilde[travail-1]);
}

int initial_ditilde(int travail)
{
 return(donnees.initial_ditilde[travail-1]);
}

void set_ditilde(int travail, int value)
{
 donnees.ditilde[travail-1]=value;
}

int pi(int travail)
{ 
 return(donnees.pi[travail-1]);
}

int wi(int travail)
{ 
 return(donnees.wi[travail-1]);
}

int di(int travail)
{ 
 return(donnees.di[travail-1]);
}

void lire_donnees(char * chemin)		
{
 FILE *fichier;
 int num,ditild,p,w,i,j,d;
 
 fichier=fopen(chemin,"rt");
 if (fichier==NULL)
 {
  printf("Fichier %s introuvable\n", chemin);
  exit(-1) ;
 }

 nbre_travaux=0; somme_pi=0;

 while (!feof(fichier))
 {
  // Je lis les données
  fscanf(fichier,"%d %d %d %d %d\n",&num,&p,&w,&d,&ditild);
  // Je détermine la position d'insertion du travail
	// ==> classement par pi/wi DECROISSANT puis par ditilde DECROISSANT

  somme_pi+=p;
  j=0;
  while ((j<nbre_travaux)
			&&(((double)donnees.pi[j]/(double)donnees.wi[j]>(double)p/(double)w)||
				 (((double)donnees.pi[j]/(double)donnees.wi[j]==(double)p/(double)w)&&(donnees.ditilde[j]>ditild))))
		 j++;

  // Je décale
  for (i=nbre_travaux;i>j;i--)
  {
	donnees.ditilde[i]=donnees.ditilde[i-1];
	donnees.initial_ditilde[i]=donnees.initial_ditilde[i-1];
	donnees.pi[i]=donnees.pi[i-1];
	donnees.wi[i]=donnees.wi[i-1];
	donnees.di[i]=donnees.di[i-1];
  }
  // J'insère
	donnees.ditilde[j]=ditild;
	donnees.initial_ditilde[j]=ditild;
	donnees.pi[j]=p;  
	donnees.wi[j]=w; 
	donnees.di[j]=d; 

  nbre_travaux++;
 }
 fclose(fichier);

 // Lecture de la stratégie de recherche
 fichier=fopen("pvw.ini","rt");
 fscanf(fichier,"%d\n",&SearchStrategy);
 fscanf(fichier, "%d\n", &LBStrategy);
 fscanf(fichier, "%d\n", &CleanStrategy);
 fscanf(fichier, "%d\n", &DomStrategy);
 fclose(fichier);
 printf("Stra=%d. LBStra=%d. CleanStra=%d\n", SearchStrategy, LBStrategy, CleanStrategy);
 for (i=0;i<nbre_travaux;i++) witmp[i]=wi(i+1);
}

