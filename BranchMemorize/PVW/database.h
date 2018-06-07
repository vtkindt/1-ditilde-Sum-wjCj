///////////////////////////////////////////////////////////////////////
//
// Database.h: interface for the Database class. 
// (Working: dont worry of the code in an header file, please!!!!)
//
///////////////////////////////////////////////////////////////////////

#ifndef MYDATABASE

#define MYDATABASE
#include <stdlib.h>
#include <stdio.h>
#include "common.h"


extern int IsOn; // ON;OFF
struct S_ItemD {

	short Jobs;
	int Seq[DBMAXJOBS];
	BIGINT CSum;
	BIGINT CMax;
	BIGINT Hash;
	char Done;
	char NbUsed;
};	

typedef struct S_ItemD ItemD;


extern BIGINT Dimension;			// DB Dimension
extern int N_Jobs;				// Number of jobs
//extern unsigned nb_task;
extern int Nbdatabase;

extern ItemD *ItemsD;

extern int *Indexes;			 // The indexes vector
	
extern int Starting[HASHITEMS*DBMAXJOBS];  // The starting points for each
									// dimension of the subsequence
									// 0 = empty list.

extern int AllocDB(BIGINT Dimmax, int Jobs);
extern int FreeDB(void);

extern int DBAdd(int *Seq, BIGINT CS, BIGINT C2, int Len, int DoNotAdd, int *index2);
							// DoNotAdd =	1 -> only the dominance testing is done.		
							//				-1-> Always add.
							//				0 -> Both.
							// Returns  1 if added (non dominated)
							//			-1 if dominated.
//!MEMO Add (t0, jobset, induced Csum)
 int DBAddPb(int *Seq, int Len, int t0, BIGINT sol, int isLB);

//!MEMO Search a solved pb in Memo. If found, return sol, otherwise return -1.
int DBSearchPb(int *Seq, int Len, int t0, int* isLB);
extern int GetActualDBDimension();
extern int PrintDB(void);

extern int DomTest(int N, BIGINT CSa, BIGINT CSb); // 1  - b is dominated
														// 2  - a is dominated
														// 0  - equal
														// -1 - non dominated


extern int SeqTest(int N, int *A, int *B); // 1 - Same jobs, 0 - no.
	
extern int DBDelete(int *Index, int *LastIndex);	// Delete Items[index], pointed by Lastindex 
											// -1 = first of the list

extern int RemovalCriteria(void);	
						// Removes an item from the db. Used when the DB is full.
extern unsigned int DBGenerate(int *Seq, BIGINT CS, BIGINT C2, int Len);





#endif 
