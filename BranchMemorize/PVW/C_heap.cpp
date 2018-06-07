#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <conio.h>
#include "C_heap.hpp"


int C_heap::cpteur=0;

// Constructeurs
// et Destructeur
////////////////
C_heap::C_heap()
{
	cpteur++;
	taille=0;
	taille_max=nbre_travaux;

	element=(int*)malloc(taille_max*sizeof(int));//! Leak reported 'many'
	if (element==NULL)
	{
	 printf("Plus assez de mémoire\n");
	 _getch();
	 //exit(1);
	}
}

C_heap::C_heap(int new_taille_max)
{
	cpteur++;
	taille=0;
	taille_max=new_taille_max;

	element=(int*)malloc(taille_max*sizeof(int));
	if (element==NULL)
	{
	 printf("Plus assez de mémoire\n");
	 getch();
	 //exit(1);
	}
}

C_heap::~C_heap()
{
	cpteur--;
	free(element);
}

// Opérateurs
////////////////

C_heap & C_heap::operator = (C_heap & father)
{
 if (this != & father)
  {// The father is not the child

		taille=father.taille;
		for(int i=0;i<taille;i++)
			element[i]=father.element[i];
  }
 return *this;
}


//////////////////////////////////////////////////////////
//											rechercher_element
//////////////////////////////////////////////////////////
// Recherche un élément et renvoie sa position en cas de succes
//////////////////////////////////////////////////////////
// I: l'élément à rechercher
// needs : nothing
// O: la position de l'element s'il est trouvé, 0 sinon
// implies: nothing
//////////////////////////////////////////////////////////
int C_heap::rechercher_element(int value)
{
	for(int i=0; i<taille;i++)
		if(element[i]==value)
			return i+1;
	return 0;
}

//Autres
/////////
void C_heap::afficher()
{
	for(int i=1;i<taille+1;i++) printf("%d ",element[i-1]);
	printf("\n");
}

// Là il devrait y avoir tout plein de préconditions !!
void C_heap::echanger(int index1, int index2)
{
	int element_temp;
	element_temp=element[index1-1];
	element[index1-1]=element[index2-1];
	element[index2-1]=element_temp;
}

// Primitives d'insertion / Extraction
////////////////////////////////////////


	//////////////////////////////////////////////////////////
	//											insert_alafin
	//////////////////////////////////////////////////////////
	// Insère en position quelconque ( = en O(1) )
	// Utile quand le tas est utilisé comme une liste non triée
	//////////////////////////////////////////////////////////
	// I: l'élément à insérer
	// needs : taille<taille_max
	// O: nothing
	// implies: l'élément est inséré quelquepart
	//////////////////////////////////////////////////////////
	void C_heap::insert_alafin(int value)
	{
		element[taille]=value;
		taille++;
	}

	//////////////////////////////////////////////////////////
	//											extract_position_pasdetri
	//////////////////////////////////////////////////////////
	// Extrait l'élément en position donnée ( = en O(1) )
	// Utile quand le tas est utilisé comme une liste non triée
	//////////////////////////////////////////////////////////
	// I: la position de l'élément à extraire
	// needs : index<=taille
	// O: l'élément extrait
	// implies: taille'='taille-1;
	//////////////////////////////////////////////////////////
	int C_heap::extract_position_pasdetri(int index)
	{
		int value=element[index-1];
		if(index!=taille)
			element[index-1]=element[taille-1];
		taille--;
		return value;
	}
										

	/////////////////////////////////////////
	void C_heap::insert_croissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (element[i-1]>element[(int)(i/2)-1]) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}

	/////////////////////////////////////////
	int C_heap::extract_croissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (element[2*i-1]>element[2*i]) )	j=2*i;
			else	j=2*i+1;
			if (element[i-1]<element[j-1])
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	void C_heap::insert_decroissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (element[i-1]<element[(int)(i/2)-1]) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}
	
	/////////////////////////////////////////////
	int C_heap::extract_decroissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (element[2*i-1]<element[2*i]) )	j=2*i;
			else	j=2*i+1;
			if (element[i-1]>element[j-1])
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}
	
	/*/////////////////////////////////////////////////////////
	*									insert_pi_croissant
	*
	* Insère un élément dans le tas et le range à sa place
	*		lorsque le tas est trié par pi croissant
	*	(+grd élément à la racine)
	*
	* I: l'element a inserer
	* needs : 0 <= taille < taille_max
	* O: nothing
	* implies: l'element est insere
	********************************************************/
	void C_heap::insert_pi_croissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (pi(element[i-1])>pi(element[(int)(i/2)-1])) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_pi_croissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus grand pi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_pi_croissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (pi(element[2*i-1])>pi(element[2*i])) )	j=2*i;
			else	j=2*i+1;
			if (pi(element[i-1])<pi(element[j-1]))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	//////////////////////////////////////////////////////////
	//									insert_pisurwi_croissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par pi/wi croissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max ET wi!=0
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_pisurwi_croissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (	(((double)pi(element[i-1])/(double)wi(element[i-1])) >
				((double)pi(element[(int)(i/2)-1])/(double)wi(element[(int)(i/2)-1]))) 
				|| ((((double)pi(element[i-1])/(double)wi(element[i-1])) ==
				   ((double)pi(element[(int)(i/2)-1])/(double)wi(element[(int)(i/2)-1])))
					&&((ditilde(element[i-1])>ditilde(element[(int)(i/2)-1]))))))
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_pisurwi_croissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus grand pi/wi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0, wi!=0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_pisurwi_croissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			
			if( (2*i==taille) || (((double)pi(element[2*i-1])/(double)wi(element[2*i-1])) > 
					((double)pi(element[2*i])/(double)wi(element[2*i])) )||((((double)pi(element[2*i-1])/(double)wi(element[2*i-1])) == 
					((double)pi(element[2*i])/(double)wi(element[2*i])) )&&((ditilde(element[2*i-1])) > 
					(ditilde(element[2*i])) )))
				j=2*i;
			else	j=2*i+1;
			if ((((double)pi(element[i-1])/(double)wi(element[i-1])) < 
					((double)pi(element[j-1])/(double)wi(element[j-1]))  )
					||((((double)pi(element[i-1])/(double)wi(element[i-1])) ==
					((double)pi(element[j-1])/(double)wi(element[j-1]))  )&&((ditilde(element[i-1])) < 
					(ditilde(element[j-1]))  )))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	//////////////////////////////////////////////////////////
	//									insert_pidisurwi_croissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par pi/wi croissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max ET wi!=0
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_pidisurwi_croissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (	(((double)(pi(element[i-1])+ditilde(element[i-1])/100.0)/(double)wi(element[i-1])) >
				((double)(pi(element[(int)(i/2)-1])+ditilde(element[(int)(i/2)-1])/100.0)/(double)wi(element[(int)(i/2)-1]))) 
				|| ((((double)(pi(element[i-1])+ditilde(element[i-1])/100.0)/(double)wi(element[i-1])) ==
				   ((double)(pi(element[(int)(i/2)-1])+ditilde(element[(int)(i/2)-1])/100.0)/(double)wi(element[(int)(i/2)-1])))
					&&((ditilde(element[i-1])>ditilde(element[(int)(i/2)-1]))))))
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_pidisurwi_croissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus grand pi/wi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0, wi!=0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_pidisurwi_croissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			
			if( (2*i==taille) || (((double)(pi(element[2*i-1])+ditilde(element[2*i-1])/100.0)/(double)wi(element[2*i-1])) > 
					((double)(pi(element[2*i])+ditilde(element[2*i])/100.0)/(double)wi(element[2*i])) )||((((double)(pi(element[2*i-1])+ditilde(element[2*i-1])/100.0)/(double)wi(element[2*i-1])) == 
					((double)(pi(element[2*i])+ditilde(element[2*i])/100.0)/(double)wi(element[2*i])) )&&((ditilde(element[2*i-1])) > 
					(ditilde(element[2*i])) )))
				j=2*i;
			else	j=2*i+1;
			if ((((double)(pi(element[i-1])+ditilde(element[i-1])/100.0)/(double)wi(element[i-1])) < 
					((double)(pi(element[j-1])+ditilde(element[j-1])/100.0)/(double)wi(element[j-1]))  )
					||((((double)(pi(element[i-1])+ditilde(element[i-1])/100.0)/(double)wi(element[i-1])) ==
					((double)(pi(element[j-1])+ditilde(element[j-1])/100.0)/(double)wi(element[j-1]))  )&&((ditilde(element[i-1])) < 
					(ditilde(element[j-1]))  )))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	//////////////////////////////////////////////////////////
	//									insert_pisurwitmp_croissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par pi/wi croissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max ET wi!=0
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_pisurwitmp_croissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (	(((double)pi(element[i-1])/(double)witmp[element[i-1]-1]) >
				((double)pi(element[(int)(i/2)-1])/(double)witmp[element[(int)(i/2)-1]-1])) 
				|| ((((double)pi(element[i-1])/(double)witmp[element[i-1]-1]) ==
				   ((double)pi(element[(int)(i/2)-1])/(double)witmp[element[(int)(i/2)-1]-1]))
					&&((ditilde(element[i-1])>ditilde(element[(int)(i/2)-1]))))))
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_pisurwitmp_croissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus grand pi/wi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0, wi!=0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_pisurwitmp_croissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			
			if( (2*i==taille) || (((double)pi(element[2*i-1])/(double)witmp[element[2*i-1]-1]) > 
					((double)pi(element[2*i])/(double)witmp[element[2*i]-1]) )||((((double)pi(element[2*i-1])/(double)witmp[element[2*i-1]-1]) == 
					((double)pi(element[2*i])/(double)witmp[element[2*i]-1]) )&&((ditilde(element[2*i-1])) > 
					(ditilde(element[2*i])) )))
				j=2*i;
			else	j=2*i+1;
			if ((((double)pi(element[i-1])/(double)witmp[element[i-1]-1]) < 
					((double)pi(element[j-1])/(double)witmp[element[j-1]-1])  )
					||((((double)pi(element[i-1])/(double)witmp[element[i-1]-1]) ==
					((double)pi(element[j-1])/(double)witmp[element[j-1]-1])  )&&((ditilde(element[i-1])) < 
					(ditilde(element[j-1]))  )))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	//////////////////////////////////////////////////////////
	//									insert_ditilde_croissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par ditilde croissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_ditilde_croissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (ditilde(element[i-1])>ditilde(element[(int)(i/2)-1])) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_ditilde_croissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus grand ditilde et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_ditilde_croissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (ditilde(element[2*i-1])>ditilde(element[2*i])) )	j=2*i;
			else	j=2*i+1;
			if (ditilde(element[i-1])<ditilde(element[j-1]))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	//////////////////////////////////////////////////////////
	//									insert_wi_croissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par wi croissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_wi_croissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (wi(element[i-1])>wi(element[(int)(i/2)-1])) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_wi_croissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus grand wi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_wi_croissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (wi(element[2*i-1])>wi(element[2*i])) )	j=2*i;
			else	j=2*i+1;
			if (wi(element[i-1])<wi(element[j-1]))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

/******************************************************
*******************************************************/
									
	//////////////////////////////////////////////////////////
	//									insert_pi_decroissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par pi décroissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_pi_decroissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (pi(element[i-1])<pi(element[(int)(i/2)-1])) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_pi_decroissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus petit pi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_pi_decroissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (pi(element[2*i-1])<pi(element[2*i])) )	j=2*i;
			else	j=2*i+1;
			if (pi(element[i-1])>pi(element[j-1]))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	//////////////////////////////////////////////////////////
	//									insert_pisurwi_decroissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par pi/wi decroissant
	//	(+grd petit à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max ET wi!=0
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_pisurwi_decroissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (	((double)pi(element[i-1])/wi(element[i-1])) <
				((double)pi(element[(int)(i/2)-1])/wi(element[(int)(i/2)-1]))))
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_pisurwi_decroissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus petit pi/wi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0, wi!=0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_pisurwi_decroissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || ((double)pi(element[2*i-1])/wi(element[2*i-1])) < 
					((double)pi(element[2*i])/wi(element[2*i])) )
				j=2*i;
			else	j=2*i+1;
			if (((double)pi(element[i-1])/wi(element[i-1])) > 
					((double)pi(element[j-1])/wi(element[j-1]))  )
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}



	//////////////////////////////////////////////////////////
	//									insert_ditilde_decroissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par ditilde decroissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_ditilde_decroissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (ditilde(element[i-1])<ditilde(element[(int)(i/2)-1])) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}

	//////////////////////////////////////////////////////////
	//									insert_ditilde_puis_pisurwi_decroissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par ditilde decroissant (et pi/wi décroissant
	//	en cas d'ex-aequos) 	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_ditilde_puis_pisurwi_decroissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (
			ditilde(element[i-1]) < ditilde(element[(int)(i/2)-1]) ||
				( ditilde(element[i-1]) == ditilde(element[(int)(i/2)-1]) &&
					(double)pi(element[i-1])/wi(element[i-1]) <
					(double)pi(element[(int)(i/2)-1])/wi(element[(int)(i/2)-1])
				)
			)
		)
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}

	//////////////////////////////////////////////////////////
	//									extract_ditilde_decroissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus petit ditilde et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_ditilde_decroissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (ditilde(element[2*i-1])<ditilde(element[2*i])) )	j=2*i;
			else	j=2*i+1;
			if (ditilde(element[i-1])>ditilde(element[j-1]))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

	//////////////////////////////////////////////////////////
	//									insert_wi_decroissant
	//////////////////////////////////////////////////////////
	// Insère un élément dans le tas et le range à sa place
	//	lorsque le tas est trié par wi decroissant
	//	(+grd élément à la racine)
	//////////////////////////////////////////////////////////
	// I: l'element a inserer
	// needs : 0 <= taille < taille_max
	// O: nothing
	// implies: l'element est insere
	//////////////////////////////////////////////////////////
	void C_heap::insert_wi_decroissant(int value)
	{
		taille++;
		element[taille-1]=value;

		// Tri du tas
		int i=taille;
		while( (i>1) && (wi(element[i-1])<wi(element[(int)(i/2)-1])) )
		{
			echanger(i, (int)i/2);
			i=i/2;
		}	
	}


	//////////////////////////////////////////////////////////
	//									extract_wi_decroissant
	//////////////////////////////////////////////////////////
	// Extrait du tas l'element de plus petit wi et le renvoie
	//////////////////////////////////////////////////////////
	// I: nothing
	// needs : taille > 0
	// O: l'element extrait
	// implies: l'element est extrait de la liste, taille'='taille-1
	//////////////////////////////////////////////////////////
	int C_heap::extract_wi_decroissant()
	{
		int maximum=element[0];
		int i=1;
		bool fini=false;

		element[0]=element[taille-1];
		taille--;
		while( (i<=taille/2) && !fini )
		{
			int j;
			if( (2*i==taille) || (wi(element[2*i-1])<wi(element[2*i])) )	j=2*i;
			else	j=2*i+1;
			if (wi(element[i-1])>wi(element[j-1]))
			{
				echanger(i,j);
				i=j;
			}
			else	fini=true;
		}
		return maximum;
	}

