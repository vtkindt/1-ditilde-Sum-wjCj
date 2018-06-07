//! Replaced "posner" by "pvw"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include "testutils.h"
#pragma warning(disable:4996)
#define PMAX 1000
#define WMAX 100

Config c;
Stat s;

void generateByCopy(int n, double L, double R, int ite);
void generate(int n, double L, double R, int ite);
void main(int argn, char *argv[] )
{
	//printf("Using fake args."); char* args[] = { "pvw.exe",  "6", "0", "40","pvw.exe","0","1" };	argv = args;	argn = 7;
	//! Treat cmd arg. stra, lbstra, start size, exe, cleanstra
	if (argn == 2) {
		c.readConfig(argv[1]);
		c.printConfig();
	}
	else if (argn != 7){
		printf("Prefer to run with : tester.exe [config file]\n\n");
		printf("Otherwise Args:  [BrStra] [LBstra=0] [StartSize] [exe] [CleaningStra] [DomStra]\n");
		printf("To run the Sips solver:  10 0 [StartSize] [exe] 0 0\n");
		printf("[BrStra]: 0/1/2 : depth-/best-/breadth- no meomo; 3/4/5 : depth/best/breadth memo; 6 : solution memo.\n");
		printf("[LBstra]: should always be 0\n");
		printf("[StartSize]: starting size of instances\n");
		printf("[exe]: the solver exe (pvw.exe)\n");
		printf("[CleaningStra]: defaut 0: no cleaning; 2 : BEFO; 3 : LUFO.\n"); //! 1: old lufo, when a new dominant sol is added, nbUsed=0.
		printf("[DomStra]: Dominance Strategy, used to enable Predictive Memo. 4 : enable k-perm predictive search. 12 : do as 4 and in addition add the found solution into database. \n");
		exit(0);
	}
	else {
		c.VERSION = atoi(argv[1]);
		c.LB_STRA = 0;
		c.SIZE_START = atoi(argv[3]);
		c.SOLVER_PATH = string(argv[4]);
		c.CLEAN_STRA = atoi(argv[5]);
		c.DOM_STRA = atoi(argv[6]);
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	// Print headers to files
	FILE *fichier = fopen(c.OUTPUT_FILE.c_str(), "at");
	fprintf(fichier, "Tests of Sips\n Done on a PC HPZ400 3.06Ghz 8G RAM\n");
	// Header info should always be updated.
	if(c.VERSION == 10)
		fprintf(fichier, "Ins\t Sol\t TCpu\t #Nodes\t #Arcs\t TWall\t Ram\n");
	else {
		fprintf(fichier, "Ins\t Sol\t TCpu\t #Nodes\t TWall\t Ram\t lb\tcutActive\tcutDone\tnbKpermAdded\tnbPolluted\n");
	}
	fprintf(fichier, "--------------------------------------------------------------------\n");
	fclose(fichier);

	fichier = fopen(c.STAT_FILE.c_str(), "at");
	fprintf(fichier, "These tests are done on a PC HPZ400 3.06Ghz 8G RAM\n");
	fprintf(fichier, "Number of iterations for a fixed value of n : %d\n\n", c.N_INS_PER_SETTING);
	fprintf(fichier, "SzIns nbIns  Nodes_min   Nodes_moy  Nodes_max  Time_min  Time_moy  Time_max\n");
	fprintf(fichier, "--------------------------------------------------------------------\n");
	fclose(fichier);

	FILE *fResIn;
	if (c.VERSION == 30) {
		fResIn = fopen(c.INPUT_FILE.c_str(), "r");
		if (!fResIn) {
			fprintf(stderr, "Problem on opening input file %s. Exiting...\n", c.INPUT_FILE.c_str());
			exit(0);
		}
	}
	char buf[20], insfile[100];
	for (int insSize = c.SIZE_START; insSize <= c.MAX_SIZE_PB; insSize += c.SIZE_STEP)
	{
		//srand(1);
		//getchar();
		int insId = 0;
		for (double L = 0.6; L <= 1 + EPSILON; L += 0.1) {
			for (double R = 0.2; R <= 1.6 + EPSILON; R += 0.2) {
				if (L + R / 2 > 1 + EPSILON)
				{
					//printf("LR=%f,%f\n", L,R);
					//if (L < 1-EPSILON && R < 1.6-EPSILON) continue;//! Only consider new pairs. 
					for (int j = 0; j < c.N_INS_PER_SETTING; j++)
					{
						//printf("j=%d\n", j);
						insId++;
						if (c.VERSION == 20) {
							generate(insSize, L, R, j); continue; // generation only
						}
						if (insSize == c.SIZE_START && insId < c.INS_START)continue;
						if (c.VERSION != 30)
							generateByCopy(insSize, L, R, j);
						sprintf_s(insfile, "data\\%d\\SDT_%d_%1.1lf_%1.1lf_%d.dat", insSize, insSize, L, R, j);
						printf("Jeux n°%ld\n", j + 1);
						fflush(stdout);

						Result r;
						// Make stat
						if (c.VERSION == 30) {
							//read from res file
							int szTmp = insSize - 1, idTmp = j - 1;
							double ll, rr;
							int ctr = 0;
							while (true) {
								//if(c.NB_COL_INPUT_FILE==10)
								ctr = fscanf(fResIn, "data\\%d\\SDT_%d_%1.1lf_%1.1lf_%d.dat\t%d\t%lf\t%lld\t%lld\t%lf\t%lld\n",
									&szTmp,&szTmp, &ll, &rr, &idTmp, &r.sol, &r.tCpu, &r.nbNodes, &r.nbArcs, &r.tWall, &r.ram);
								printf("%d, %d, %f, %f, %d\n", ctr, szTmp, ll, rr, idTmp);
								if (ctr != c.NB_COL_INPUT_FILE || (szTmp == insSize && idTmp == j)) break;
								else continue;
								//else {
									//fprintf(stderr, "Cannot find result from file for instance %s. Exiting...\n", datadir);
									//exit(0);
								//}
							}
							if (ctr != c.NB_COL_INPUT_FILE) { L = R = 2;  c.MAX_SIZE_PB = insSize - 1; --insId; break; }
							s.addResult(r, insSize);
						}
						else if (c.VERSION == 10)
						{
							printf("Sips solver is running...\n"); fflush(stdout);
							sprintf_s(buf, "%d", insSize);
							spawnl(P_WAIT, c.SOLVER_PATH.c_str(), c.SOLVER_PATH.c_str(), c.INS_FILE, buf, NULL);
							fichier = fopen(c.SOLVER_SOL_FILE.c_str(), "rt");
							fscanf_s(fichier, "%lf\n", &r.tCpu);
							fscanf_s(fichier, "%d\n", &r.sol);
							//fscanf_s(fichier, "%d\n", &lb);
							fscanf_s(fichier, "%lld\n", &r.nbNodes);
							fscanf_s(fichier, "%lld\n", &r.nbArcs);
							fscanf_s(fichier, "%lf\n", &r.tWall);
							fscanf_s(fichier, "%lld\n", &r.ram);
							fclose(fichier);
							s.addResult(r, insSize);

							fichier = fopen(c.OUTPUT_FILE.c_str(), "at");
							fprintf(fichier, "%s\t%d\t%lf\t%lld\t%lld\t%lf\t%lld\n",
								insfile, r.sol, r.tCpu, r.nbNodes, r.nbArcs, r.tWall, r.ram);
							fclose(fichier);
							if (r.tCpu > c.TIME_LIM) { L = R = 2;  c.MAX_SIZE_PB = insSize - 1; break; }
						}
						else // Run bb algorithms
						{
							fichier = fopen("pvw.ini", "wt");
							fprintf(fichier, "%d\n", c.VERSION);
							fprintf(fichier, "%d\n", c.LB_STRA);
							fprintf(fichier, "%d\n", c.CLEAN_STRA);
							fprintf(fichier, "%d\n", c.DOM_STRA);
							fclose(fichier);
							spawnl(P_WAIT, c.SOLVER_PATH.c_str(), c.SOLVER_PATH.c_str(), c.INS_FILE.c_str(), NULL);
							fichier = fopen(c.SOLVER_SOL_FILE.c_str(), "rt");
							fscanf_s(fichier, "%lf\n", &r.tCpu);
							fscanf_s(fichier, "%d\n", &r.sol);
							fscanf_s(fichier, "%d\n", &r.lb);
							fscanf_s(fichier, "%lld\n", &r.nbNodes);
							fscanf(fichier, "%lld\n", &r.cutActive);
							fscanf(fichier, "%lld\n", &r.cutDone);
							fscanf(fichier, "%lf\n", &r.tWall); //! cpu time
							fscanf(fichier, "%lld\n", &r.ram); //! max ram
							fscanf(fichier, "%lld\n", &r.nbKpermAdded); //!
							fscanf(fichier, "%lld\n", &r.nbPolluted); //!
							fclose(fichier);
							s.addResult(r, insSize);

							fichier = fopen(c.OUTPUT_FILE.c_str(), "at");
							fprintf(fichier, "%s\t%d\t%.2f\t%lld\t%.2f\t%lld\t%d\t%lld\t%lld\t%lld\t%lld\n",
								insfile, r.sol, r.tCpu, r.nbNodes, r.tWall, r.ram,r.lb, r.cutActive, r.cutDone, r.nbKpermAdded, r.nbPolluted);
							fclose(fichier);
							if (r.tCpu > c.TIME_LIM) { L = R = 2;  c.MAX_SIZE_PB = insSize - 1; break; }
						}
					}//for (int j = 0; j < c.N_INS_PER_SETTING; j++)
				}//if (L + R / 2 > 1+EPSILON)
			}//R
		}//L
	    //! Results by size
		auto minAvgMaxNodes = s.getMinAvgMax(insSize, &Result::nbNodes);
		auto minAvgMaxT = s.getMinAvgMax(insSize, &Result::tCpu);
		fichier = fopen(c.STAT_FILE.c_str(), "at");
		fprintf(fichier, "%d\t%d\t%lld\t%9.2lf\t%lld\t%3.3lf\t%3.3lf\t%3.3lf\n",
			insSize, insId, get<0>(minAvgMaxNodes), get<1>(minAvgMaxNodes), get<2>(minAvgMaxNodes),
			get<0>(minAvgMaxT), get<1>(minAvgMaxT), get<2>(minAvgMaxT));
		fclose(fichier);
	}
}


////////////////////////////////////////////////////////////////////
// Use already generated files
////////////////////////////////////////////////////////////////////
void generateByCopy(int n, double L, double R, int ite) // PREC : n>0 ...
{
	FILE *file;
	char nom[500];
	sprintf_s(nom, "data\\%d\\SDT_%d_%1.1lf_%1.1lf_%d.dat", n, n, L, R, ite);

	if ((file = fopen(nom, "r")) == NULL) {
		printf("** ouverture impossible de %s **", nom); exit(-6);
	}
	FILE* copyto = fopen(c.INS_FILE.c_str(), "w");
	if (file == NULL) {
		printf("** création impossible (donnees.dat)**"); exit(-6);
	}
	char buf[20000];
	int count = fread(buf, 1, 20000, file);
	printf("Data file : %s\n", nom);
	if (count != fwrite(buf, 1, count, copyto)) {
		perror("Data file copy failed. Pause\n");
		getchar();
	}
	fclose(file);
	fclose(copyto);
}

////////////////////////////////////////////////////////////////////
// Cette fonction génère les donnée initiales 
// Les fichiers de données sont stockés dans un dossier (data_gen) et 
//  il faut utiliser generateByCopy() pour les utiliser.
////////////////////////////////////////////////////////////////////
void generate(int n, double L, double R, int ite)
{
	FILE *file;
	int k, *p, *w, *d, *pris, somme_p, totsommep, j;
	int mini, maxi, mind, posd;
	short faisable = 0;
	char nom[500];

	p = (int*)malloc(n * sizeof(int));
	w = (int*)malloc(n * sizeof(int));
	d = (int*)malloc(n * sizeof(int));
	pris = (int*)malloc(n * sizeof(int));

	// création du fichier
	//if((file=fopen("donnees.dat","w+"))==NULL)
	sprintf_s(nom, ".\\data_gen\\%d\\SDT_%d_%1.1lf_%1.1lf_%d.dat",n, n, L, R, ite);
	if ((file = fopen(nom, "w+")) == NULL)
	{
		printf("** ouverture impossible **"); exit(-6);
	}

	while (faisable == 0)
	{
		// génération des données
		somme_p = 0;
		for (k = 1; k<n + 1; k++)
		{
			p[k - 1] = (1 + ((int)rand() * (PMAX-1)) / RAND_MAX);	// p_min=1 et p_max=100 
			somme_p += p[k - 1];
			w[k - 1] = (1 + ((int)rand() * (WMAX-1)) / RAND_MAX);	// w_min=1 et w_max=10 
		}

		mini = (int)__max((somme_p*L - somme_p * R / 2), 0);	// d_min
		maxi = (int)(somme_p*L + somme_p * R / 2);	// d_max
		totsommep = somme_p;

		for (k = 1; k<n + 1; k++)
		{
			int randomm = rand();
			d[k - 1] = (mini + ((int)randomm*(maxi - mini)) / RAND_MAX);
		}
		// Test de faisabilité
		for (k = 0; k<n; k++) pris[k] = 0;
		somme_p = 0;
		for (k = 0; k<n; k++)
		{
			mind = 999999;
			posd = 999999;
			for (j = 0; j<n; j++)
				if ((pris[j] == 0) && (d[j]<mind))
				{
					mind = d[j];
					posd = j;
				}
			pris[posd] = 1;
			somme_p += p[posd];
			if (somme_p > mind)
			{
				if (somme_p <= maxi)
					d[posd] = (somme_p + ((int)rand()*(maxi - somme_p)) / RAND_MAX);
				else d[posd] = (somme_p + ((int)rand()*(totsommep - somme_p)) / RAND_MAX);
				pris[posd] = 0;
				somme_p -= p[posd];
				k--;
			}
			//break;
		}
		if (k == n) faisable = 1;
	}

	for (k = 1; k<n + 1; k++)
		fprintf(file, "%d %lu %lu %lu %lu\n", k, p[k - 1], w[k - 1], d[k - 1], d[k - 1]);
	fclose(file);
	free(p); free(w); free(d); free(pris);
}