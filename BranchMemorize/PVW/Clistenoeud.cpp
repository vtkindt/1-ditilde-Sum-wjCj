/*---------------------------------------------------------------------------
-----   TITLE   : Classe pour g�rer les listes de noeuds                -----
-----------------------------------------------------------------------------
-----   AUTEUR  : T'KINDT V. / LAUGT D. / TERICNET F. VERSION 1.0       -----
-----							Carl ESSWEIN																					-----
-----   DATE    : 15/10/2001    ---------------------------------------------
-----------------------------------------------------------------------------
-----   LECTEUR : T'KINDT V. / LAUGT D. / TERICNET F. DATE : 15/10/2001 -----
-----							Carl ESSWEIN																					-----
-----------------------------------------------------------------------------
-----   INTERFACE DE LA CLASSE Cnodelist                                -----
---------------------------------------------------------------------------*/

#include "Clistenoeud.hpp"      

BIGINT Cnodelist::cpteur=0;

/*---------------------------------------------------------------------------
-----   NOM     : Cnodelist                                            -----
-----------------------------------------------------------------------------
-----   Constructeur de la classe Cnodelist                              -----
-----   permet de cr�er l'objet                                      -----
-----------------------------------------------------------------------------
-----   ENTREE  : rien                                                  -----
-----   SORTIE  : rien                                                  -----
---------------------------------------------------------------------------*/
Cnodelist::Cnodelist()
{
	cpteur++;
	taille=0;	liste=NULL;
	indpos=999999999;	avindpos=999999999;
}

/*---------------------------------------------------------------------------
-----   NOM     : ~Cnodelist                                            -----
-----------------------------------------------------------------------------
-----   Destructeur de la classe Cnodelist                              -----
-----   permet de d�truire l'objet                                      -----
-----------------------------------------------------------------------------
-----   ENTREE  : rien                                                  -----
-----   SORTIE  : rien                                                  -----
---------------------------------------------------------------------------*/
//!Leak solved (fils_tries in cenumer.cpp): nothing should be released since nothing is allocated in the constructor: they are freed externally
Cnodelist::~Cnodelist()
{
	cpteur--;
//
//        TCellNode *ptr,*ptr_suiv;
//
//        // On d�truit toutes les cellules
//        ptr=liste;
//        for (int i=0;i<taille;i++)
//        {
//					ptr_suiv=ptr->suiv;
//					delete ptr->val;
//          delete ptr;
//          ptr=ptr_suiv;
//        }
}

        
/*---------------------------------------------------------------------------
-----   NOM     : operator =                                            -----
-----------------------------------------------------------------------------
-----   Red�finition de l'op�rateur =, permet d'affecter une liste      -----
-----------------------------------------------------------------------------
-----   ENTREE  : la liste � recopier                                   -----
-----   SORTIE  : l'objet initialis�                                    -----
-----------------------------------------------------------------------------
-----   Peut g�n�rer l'exception: bad_alloc                             -----
---------------------------------------------------------------------------*/
Cnodelist & Cnodelist::operator =(const Cnodelist &L)
{
       
	TCellNode *ptr,*ptr_suiv;
	Cnoeud *tmp_val;
  int i;

  if (this != &L)
  {
    // L'objet � recopier n'est pas lui m�me
    // On d�truit toutes les cellules
    ptr=liste;
    for (i=0;i<taille;i++)
    {
      ptr_suiv=ptr->suiv;
			//delete ptr->val;
      delete ptr;
      ptr=ptr_suiv;
    }
    taille=0;

    // On copie la liste L
    ptr=L.liste;
    for (i=0;i<L.taille;i++)
    {
      tmp_val=new Cnoeud;
			*tmp_val=(*ptr->val);  
			ajouter_element(tmp_val);
			ptr=ptr->suiv;
    }
    indpos=999999999;
    avindpos=999999999;
  }
  return *this;
}


/*---------------------------------------------------------------------------
-----   NOM     : Cnodelist                                             -----
-----------------------------------------------------------------------------
-----   Constructeur de recopie de la classe Cliste                     -----
-----   permet d'initaliser l'objet                                     -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'objet � recopier                                    -----
-----   SORTIE  : rien                                                  -----
-----------------------------------------------------------------------------
-----   Peut g�n�rer l'exception bad_alloc ou xalloc                    -----
---------------------------------------------------------------------------*/
Cnodelist::Cnodelist(const Cnodelist &L)
{       
	indpos=99999999;	avindpos=99999999;
	taille=0;	*this=L;
}


/*---------------------------------------------------------------------------
-----   NOM     : operator []                                           -----
-----------------------------------------------------------------------------
-----   Red�finition de l'op�rateur []                                  -----
-----   permet d'acc�der au i�me �l�ment d'une liste                    -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'indice de l'�lement � acc�der                       -----
-----   SORTIE  : rien                                                  -----
-----------------------------------------------------------------------------
-----   Peut g�n�rer l'exception: "LN_Acces impossible"                 -----
---------------------------------------------------------------------------*/
Cnoeud  * Cnodelist::operator [](int indice)
{

  TCellNode *ptr;  TCellNode *avptr;
  int i;
  

  if (indpos<=indice)
  { // On poursuit le parcours de la liste � partir de la cellule encours
		 avptr=avencours;
		 ptr=encours;
         for (i=0;i<(indice-indpos);i++) 
		 {
			 avptr=ptr;
			 ptr=ptr->suiv;
		 }
         // ptr pointe sur l'�l�ment en position indice
         indpos=indice;
         encours=ptr;
		 if (indice!=0)
		 {
          avindpos=indice-1;
          avencours=avptr;
		 }
        }else
        {
		 ptr=liste;
         for (i=0;i<indice;i++) 
		 {
			 avptr=ptr;
			 ptr=ptr->suiv;
		 }
         // ptr pointe sur l'�l�ment en position indice
         indpos=indice;
         encours=ptr;
		 if (indice!=0)
		 {
          avindpos=indice-1;
          avencours=avptr;
		 }
  }
  return(ptr->val);

}


/*---------------------------------------------------------------------------
-----   NOM     : Ajouter_element                                       -----
-----------------------------------------------------------------------------
-----   Cette fonction permet d'ajouter un �l�ment en fin de liste      -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'�l�ment � ins�rer                                   -----
-----   SORTIE  : rien                                                  -----
-----------------------------------------------------------------------------
-----   Peut g�n�rer l'exception: "bad alloc"                           -----
---------------------------------------------------------------------------*/
void Cnodelist::ajouter_element(Cnoeud * valeur)
{
	
	TCellNode *avparcours,*ptr,*parcours;
	int loop=0;

	// Copie de la cellule
	ptr=new TCellNode;
	ptr->val=valeur;
	ptr->suiv=NULL;

	// Insertion � la derni�re position
	if (taille==0) liste=ptr;
	else
	{
		if (indpos<taille)
		{				 
			parcours=encours;
			avparcours=avencours;
			loop=indpos;
		} else
		{
			parcours=liste;
			loop=0;
		}
		while (loop<taille-1) 
		{
			avparcours=parcours;
			parcours=parcours->suiv;
			loop++;
		}
		indpos=loop;
		encours=parcours;
		if (taille>1)
		{
			avindpos=loop-1;
			avencours=avparcours;
		}
		ptr->suiv=parcours->suiv;
		parcours->suiv=ptr;
	}
	taille++;
}

/*---------------------------------------------------------------------------
-----   NOM     : Ajouter_element_LB_cissant                            -----
-----------------------------------------------------------------------------
-----   Cette fonction permet d'ajouter un �l�ment selon l'ordre des    -----
-----   LB croissantes.                                                 -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'�l�ment � ins�rer                                   -----
-----   SORTIE  : la position d'insertion                               -----
---------------------------------------------------------------------------*/
int Cnodelist::ajouter_element_LB_croissant(Cnoeud * valeur)
{
 				TCellNode *ptr,*parcours;
        int loop;

        // Copie de la cellule
        ptr=new TCellNode;
        ptr->val=valeur;
        ptr->suiv=NULL;

        // On recherche la position d'insertion
        if ((indpos<taille)&&(encours->val->LB<=valeur->LB))
                {
                 loop=indpos;
                 parcours=encours;
                }else
                {
                 loop=0;
                 parcours=liste;
                }
        if ((loop<taille)&&(parcours->val->LB<=valeur->LB))
        {
         loop++;
         while ((loop<taille)&&((parcours->suiv)->val->LB<valeur->LB))
         {
			    parcours=parcours->suiv;
                loop++;
         }
        }

        // Insertion 
        if (loop==0)
        {
         ptr->suiv=liste;
         liste=ptr;
        } else
        {
         ptr->suiv=parcours->suiv;
         parcours->suiv=ptr;
        }
        indpos=loop;
        encours=ptr;
		if (loop>0)
		{
		 avindpos=loop-1;
		 avencours=parcours;
		} else avindpos=999999;
        taille++;

 return(loop);
}
/*---------------------------------------------------------------------------
-----   NOM     : Ajouter_element_LB_decroissant                            -----
-----------------------------------------------------------------------------
-----   Cette fonction permet d'ajouter un �l�ment selon l'ordre des    -----
-----   LB decroissantes.                                                 -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'�l�ment � ins�rer                                   -----
-----   SORTIE  : la position d'insertion                               -----
-----------------------------------------------------------------------------
-----   Peut g�n�rer l'exception: "bad alloc"                           -----
---------------------------------------------------------------------------*/
int Cnodelist::ajouter_element_LB_decroissant(Cnoeud * valeur)
{
 
				TCellNode *ptr,*parcours;
        int loop;

        // Copie de la cellule
        ptr=new TCellNode;
        ptr->val=valeur;
        ptr->suiv=NULL;

        // On recherche la position d'insertion
        if ((indpos<taille)&&(encours->val->LB>=valeur->LB))
                {
                 loop=indpos;
                 parcours=encours;
                }else
                {
                 loop=0;
                 parcours=liste;
                }
        if ((loop<taille)&&(parcours->val->LB>=valeur->LB))
        {
         loop++;
         while ((loop<taille)&&((parcours->suiv)->val->LB>valeur->LB))
         {
			    parcours=parcours->suiv;
                loop++;
         }
        }

        // Insertion 
        if (loop==0)
        {
         ptr->suiv=liste;
         liste=ptr;
        } else
        {
         ptr->suiv=parcours->suiv;
         parcours->suiv=ptr;
        }
        indpos=loop;
        encours=ptr;
		if (loop>0)
		{
		 avindpos=loop-1;
		 avencours=parcours;
		} else avindpos=999999;
        taille++;

 return(loop);
}
/*---------------------------------------------------------------------------
-----   NOM     : Ajouter_element_NIVcroissant                            -----
-----------------------------------------------------------------------------
-----   Cette fonction permet d'ajouter un �l�ment selon l'ordre des    -----
-----   niveaux croissants.                                                 -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'�l�ment � ins�rer                                   -----
-----   SORTIE  : la position d'insertion                               -----
-----------------------------------------------------------------------------
-----   Peut g�n�rer l'exception: "bad alloc"                           -----
---------------------------------------------------------------------------*/
int Cnodelist::ajouter_element_NIVcroissant(Cnoeud * valeur)
{
        TCellNode *ptr,*parcours;
        int loop;

        // Copie de la cellule
        ptr=new TCellNode;
        ptr->val=valeur;
        ptr->suiv=NULL;

        // On recherche la position d'insertion
        if ((indpos<taille)&&(encours->val->scheduled.taille<=valeur->scheduled.taille))
                {
                 loop=indpos;
                 parcours=encours;
                }else
                {
                 loop=0;
                 parcours=liste;
                }
        if ((loop<taille)&&(parcours->val->scheduled.taille<=valeur->scheduled.taille))
        {
         loop++;
         while ((loop<taille)&&((parcours->suiv)->val->scheduled.taille<valeur->scheduled.taille))
         {
                parcours=parcours->suiv;
                loop++;
         }
        }

        // Insertion 
        if (loop==0)
        {
         ptr->suiv=liste;
         liste=ptr;
        } else
        {
         ptr->suiv=parcours->suiv;
         parcours->suiv=ptr;
        }
        indpos=loop;
				encours=ptr;
				if (loop>0)
				{
				 avindpos=loop-1;
				 avencours=parcours;
				} else avindpos=999999;
						taille++;

 return(loop);
}

/*---------------------------------------------------------------------------
-----   NOM     : Supprimer_position                                    -----
-----------------------------------------------------------------------------
-----   Cette fonction permet de supprimer un �l�ment en i�me position  -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'indice de l'�l�ment � supprimer (0<=indice<=...)    -----
-----   SORTIE  : rien                                                  -----
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/
void Cnodelist::Supprimer_position(int indice)
{
	TCellNode *ptr, *parcours, *avparcours;
	int i, loop;

	if (indice == 0)
	{
		static BIGINT ctr = 0;
		//printf("Supprimer %d\n", ++ctr);

		//Suppression en d�but de liste
		ptr = liste;
		liste = ptr->suiv;
		parcours = liste;
		delete ptr;
	}
	else
	{
		// Parcours jusqu'au i-1�me �l�ment
		if (indice > indpos)
		{
			loop = indpos;
			parcours = encours;
			avparcours = avencours;
		}
		else
			if (indice == indpos)
			{
				loop = avindpos;
				parcours = avencours;
			}
			else
			{
				loop = 0;
				parcours = liste;
			}
		for (i = loop; i < indice - 1; i++) parcours = parcours->suiv;

		// Suppression du i�me �l�ment          
		ptr = parcours->suiv;
		parcours->suiv = ptr->suiv;
		parcours = parcours->suiv;
		delete ptr;
	}
	taille--;
	if (indice == indpos)
	{
		if (indice < taille)
			encours = parcours;
		else
		{
			indpos = 999999;
			avindpos = 999999;
		}
	}
	else
		if (indice == avindpos)
		{
			indpos = avindpos = 999999;
		}
		else
		{
			if (indice < indpos) indpos--;
			if (indice < avindpos) avindpos--;
		}
}

/*---------------------------------------------------------------------------
-----   NOM     : Inserer_position                                      -----
-----------------------------------------------------------------------------
-----   Cette fonction permet d'ajouter un �l�ment en i�me position     -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'�l�ment � ins�rer et son indice                                     -----
-----   SORTIE  : rien                                                                                                  -----
-----------------------------------------------------------------------------
-----   Peut g�n�rer l'exception: "Ajout impossible"                                    -----
-----                            l'exception: bad_alloc                                                         -----
---------------------------------------------------------------------------*/
void Cnodelist::Inserer_position(Cnoeud *val,int indice)
{
        TCellNode *ptr,*parcours;
        int i;
		//printf("insert %d\n", indice);
	      // Copie de la cellule
        ptr=new TCellNode;
        ptr->val=val;
        ptr->suiv=NULL;

        if (indice==0)
        {
                //Insertion en d�but de liste
                ptr->suiv=liste;
                liste=ptr; 
                indpos++;
				if (indpos==1)
				{
				 avindpos=0;
				 avencours=liste;
				}
        }
        else
        {
                // Parcours jusqu'au i-1�me �l�ment
                if (indice<=indpos)
                {
                 parcours=liste;
                 for (i=1;i<indice;i++) 
					 parcours=parcours->suiv;
                } else
                {
                 parcours=encours;
                 for (i=1;i<(indice-indpos);i++) 
					 parcours=parcours->suiv;
                }
        
                // Insertion � la i�me position
                ptr->suiv=parcours->suiv;
                parcours->suiv=ptr;
                indpos=indice;
                encours=parcours->suiv;
				avindpos=indice-1;
				avencours=parcours;
        }
        taille++;
}


/*---------------------------------------------------------------------------
-----   NOM     : Concatene                                             -----
-----------------------------------------------------------------------------
-----   This function concatene two lists of nodes										  -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'�l�ment � ins�rer                                   -----
-----   SORTIE  : rien                                                  -----
---------------------------------------------------------------------------*/
void Cnodelist::concatene(Cnodelist * First)
{

	TCellNode *avparcours, *parcours;
	int loop = 0;

	if (taille == 0)
	{
		liste = First->liste;
		taille = First->taille;
	}
	else
	{
		//On se rend en derni�re position
		if (indpos < taille)
		{
			parcours = encours;
			avparcours = avencours;
			loop = indpos;
		}
		else
		{
			parcours = liste;
			loop = 0;
		}
		while (loop < taille - 1)
		{
			avparcours = parcours;
			parcours = parcours->suiv;
			loop++;
		}
		indpos = loop;
		encours = parcours;
		if (taille > 1)
		{
			avindpos = loop - 1;
			avencours = avparcours;
		}
		// On est en derni�re position, on concatene
		parcours->suiv = First->liste;
		taille += First->taille;
	}
}
