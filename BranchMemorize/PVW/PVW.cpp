//! Notes by Lei about the modifications
//! Search //!Leak for modif on solving memory leak
//! The config on Code Generation->C++ Exception Activation is changed from EHsc to EHa for catching access violation by "delete" 

#include <fstream>
#include "Cenumer.hpp"
#include "donnees.h"
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <new.h>
#include "Stat.h"
//#define VLD_FORCE_ENABLE		// Enable this line to force leak detection in release version. Less info is provided than debug version
//#include "vld.h"				// Comment this line to disable leak detection
extern int CleanStrategy;
extern BIGINT TimesClean;
//! K perm dominance: look at the last k scheduled jobs, permutate to find dominant sol
//! The last four bits stands for (left to right): Whether add dominant sol to db; whether enable k perm; whether enumerate all k perm or just stop when finding the first; whether enable dom conditon based on unscheduled job (not used for this pb)
//! set to 12 to enable default predictive memo.
int DomStrategy = 0;
long long NbAddKPerm = 0;
extern long long NbCleanMin, NbCleanAvg, NbCleanMax;

// Some parameters to know:

void main(int argn, char*argv[] )
{
	setvbuf(stdout, NULL, _IONBF, 0);
	double  temps, tempscpu;
	clock_t debut;
	Cenumer *resolution; FILE *fic;
	time_t now;
	time(&now); //ctime(&now)
	printf("In solver (%s ", ctime(&now));

	if(argn>1)
		lire_donnees(argv[1]);
	else lire_donnees("donnees.dat");
	//lire_donnees("../../Output/data/Data_80_0.7_1.4_8.dat");
	//SearchStrategy = 6; //Stra fixed to 6!!!!\n
	//CleanStrategy = 3;
	printf("Stra=%d. LBstra%d, CleanStra=%d, DomStra=%d.\n", SearchStrategy, LBStrategy, CleanStrategy, DomStrategy);
	fflush(stdout);
	Stat<> cputime, walltime, maxram;
	maxram.updateMaxRam();
	//debut=clock();
	walltime.getWallDuration();
	cputime.getCpuDuration();
	resolution = new Cenumer();
	resolution->solve_1ditilde(0,0,0);

	//temps=((double)(clock()-debut)/CLOCKS_PER_SEC);
	temps = walltime.getWallDuration();
	tempscpu = cputime.getCpuDuration();
	maxram.updateMaxRam();
	printf("%lld noeuds en %.2lf sec.\n",resolution->nb_nodes, tempscpu);
	//printf("Noeuds max dans la condition (active): %f\n",(float)resolution->Nbmax/(float)resolution->fois);
	//printf("Noeuds max dans la condition (done): %f\n",Nbdatabase);
	//printf("Change 3 io file names and exe from pvw to posner.\n");
	fic=fopen("sol.txt","wt"); 
	fprintf(fic,"%lf\n",tempscpu); // temps
	fprintf(fic,"%ld\n",resolution->BestUB); // Opt
	fprintf(fic,"%ld\n",resolution->LBroot); // LBroot
	fprintf(fic,"%ld\n",resolution->nb_nodes); // Nbnoeuds
	//! Newly added
	fprintf(fic, "%ld\n", CutActive);
	fprintf(fic, "%ld\n", CutDone);
	fprintf(fic, "%lf\n", temps); //! cpu time
	fprintf(fic, "%lld\n", maxram.maxRam); //! max ram
	fprintf(fic, "%lld\n", NbAddKPerm); //!
	fprintf(fic, "%lld\n", nbPolluted); //!
	fclose(fic);

	//fic=fopen("statpvw.txt","wt");
    //fprintf(fic,"%ld\n",CutActive);
    //fprintf(fic,"%ld\n",CutDone);
	//fclose(fic);
	//! print
	fprintf(stdout, "%lf\n", tempscpu); // temps cpu
	fprintf(stdout, "%ld\n", resolution->BestUB); // Opt
	fprintf(stdout, "%ld\n", resolution->LBroot); // LBroot
	fprintf(stdout, "%ld\n", resolution->nb_nodes); // Nbnoeuds
	fprintf(stdout, "%ld\n", CutActive);
	fprintf(stdout, "%ld\n", CutDone);
	fprintf(stdout, "%lf\n", temps); //! wall time
	fprintf(stdout, "%lld\n", maxram.maxRam); //! max ram
	if (TimesClean > 0){
		printf("TimesClean:%lld\n", TimesClean);
		printf("NbCleanMinAvgMAx : %lld, %lld, %lld\n", NbCleanMin, NbCleanAvg, NbCleanMax);
	}
	printf("NbAddKPerm = %lld\n", NbAddKPerm);
	printf("nbPolluted=%lld\n", nbPolluted); //!
	if (temps - tempscpu > 100) { 
		printf("Warning: time diff = %d.\n", temps - tempscpu); 
		getchar();
	}

	//if(SearchStrategy==3 || SearchStrategy ==4)PrintDB();
	printf("==============\n");

	//!Leak solved. No need to enable the following line since here is the end of program. Moreover there are pbs inside when calling delete
	delete resolution;
	exit(0);
}



