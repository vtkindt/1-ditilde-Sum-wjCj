/*---------------------------------------------------------------------------
-----   TITRE   : Classe pour gérer les listes                                                  -----
-----------------------------------------------------------------------------
-----   AUTEUR  : LAUGT D. / TERICNET F. / T'KINDT V.           VERSION 1.0             -----
-----   DATE    : 30/05/2000    ---------------------------------------------
-----------------------------------------------------------------------------
-----   LECTEUR : LAUGT D. / TERICNET F. / T'KINDT V.   DATE : 30/05/2000       -----
-----------------------------------------------------------------------------
-----   CORPS DE LA CLASSE Cliste                                                                               -----
---------------------------------------------------------------------------*/

/* CLASSE Cliste
        DOCUMENTATION
                variables : liste: polong inteur sur le premier élément de la liste,
                                        taille: entier, le nombre d'élément dans la liste
                fonction d'abstraction:néant
                Structure:néant
                nécessite:néant*/


#include "Cliste.hpp"
//#include <stdio.h>
        
/*CORPS*/

/*---------------------------------------------------------------------------
-----   NOM     : ~Cliste                                                                                                       -----
-----------------------------------------------------------------------------
-----   Destructeur de la classe Cliste                                                         -----
-----   permet de détruire l'objet                                                                              -----
-----------------------------------------------------------------------------
-----   ENTREE  : rien                                                                                                  -----
-----   SORTIE  : rien                                                                                                  -----
---------------------------------------------------------------------------*/
template <class T> Cliste<T>::~Cliste()
{
        TCell<T> *ptr,*ptr_suiv;

        // On détruit toutes les cellules
        ptr=liste;
        for (unsigned long int i=0;i<taille;i++)
        {
                ptr_suiv=ptr->suiv;
                delete ptr;
                ptr=ptr_suiv;
        }
		//printf("Je détruit une liste\n");
		nbobj--;
}

        
/*---------------------------------------------------------------------------
-----   NOM     : operator =                                                                                            -----
-----------------------------------------------------------------------------
-----   Redéfinition de l'opérateur =, permet d'affecter une liste              -----
-----------------------------------------------------------------------------
-----   ENTREE  : la liste à recopier                                                                   -----
-----   SORTIE  : l'objet initialisé                                                                    -----
-----------------------------------------------------------------------------
-----   Peut générer l'exception: bad_alloc                                                             -----
---------------------------------------------------------------------------*/
template <class T> Cliste<T>& Cliste<T>::operator =(const Cliste<T> &L)
{
        TCell<T> *ptr,*ptr_suiv;
        unsigned  int i;

        if (this != &L)
        {
                // L'objet à recopier n'est pas lui même
                // On détruit toutes les cellules
                ptr=liste;
                for (i=0;i<taille;i++)
                {
                        ptr_suiv=ptr->suiv;
                        delete ptr;
                        ptr=ptr_suiv;
                }
                taille=0;
                                indpos=99999999;

                // On copie la liste L
                ptr=L.liste;
                for (i=0;i<L.taille;i++)
                {
                        ajouter_element(ptr->val);
                        ptr=ptr->suiv;
                }
                indpos=99999999;
        }
        return *this;
}


/*---------------------------------------------------------------------------
-----   NOM     : Cliste                                                                                                        -----
-----------------------------------------------------------------------------
-----   Constructeur de recopie de la classe Cliste                                     -----
-----   permet d'initaliser l'objet                                                                             -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'objet à recopier                                                                    -----
-----   SORTIE  : rien                                                                                                  -----
-----------------------------------------------------------------------------
-----   Peut générer l'exception bad_alloc ou xalloc                                    -----
---------------------------------------------------------------------------*/
template <class T> Cliste<T>::Cliste(const Cliste<T> &L)
{
    taille=0;
    indpos=99999999;
    *this=L;
	nbobj++;
}


/*---------------------------------------------------------------------------
-----   NOM     : operator []                                           -----
-----------------------------------------------------------------------------
-----   Redéfinition de l'opérateur []                                  -----
-----   permet d'accéder au ième élément d'une liste                    -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'indice de l'élement à accéder                       -----
-----							0<=indice<=...																				-----
-----   SORTIE  : rien                                                  -----
-----------------------------------------------------------------------------
-----   Peut générer l'exception: "Acces impossible"                    -----
---------------------------------------------------------------------------*/
template <class T>  T  * Cliste<T>::operator [](unsigned  int indice)
{
        //Cexception E;
        TCell<T> *ptr;
        unsigned  int i;
        
        //if (indice<0 || indice>=taille) throw(E.modifier_valeur(Acces_impossible));
        // On peut accéder à l'élément dans la liste

        if (indpos<=indice)
        { // On poursuit le parcours de la liste à partir de la cellule encours
         ptr=encours;
         for (i=0;i<(indice-indpos);i++) ptr=ptr->suiv;
         // ptr polong inte sur l'élément en position indice
         indpos=indice;
         encours=ptr;
        }else
        {
         ptr=liste;
         for (i=0;i<indice;i++) ptr=ptr->suiv;
         // ptr polong inte sur l'élément en position indice
         indpos=indice;
         encours=ptr;
        }
        return(ptr->val);
}


/*---------------------------------------------------------------------------
-----   NOM     : Inserer_position                                      -----
-----------------------------------------------------------------------------
-----   Cette fonction permet d'ajouter un élément en ième position     -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'élément à insérer et son indice                     -----
									0<=indice=<taille                                     -----
-----   SORTIE  : rien                                                  -----
---------------------------------------------------------------------------*/
template <class T> void Cliste<T>::Inserer_position(T *val,unsigned  int indice)
{

  TCell<T> *ptr,*parcours;
  unsigned  int i;

  // Copie de la cellule
  ptr=new TCell<T>;
  ptr->val=val;
  ptr->suiv=NULL;

  if (indice==0)
  {
          //Insertion en début de liste
          ptr->suiv=liste;
          liste=ptr; 
	if (taille==0) last=liste;
          indpos++;
  } else if (indice==taille)
		{
      ptr->suiv=NULL;
      last->suiv=ptr;
			last=ptr;		 
		}else
    {
      // Parcours jusqu'au i-1ème élément
      if (indice<=indpos)
      {
       parcours=liste;
       for (i=1;i<indice;i++) parcours=parcours->suiv;
      } else
      {
       parcours=encours;
       for (i=1;i<(indice-indpos);i++) parcours=parcours->suiv;
      }

      // Insertion à la ième position
      ptr->suiv=parcours->suiv;
      parcours->suiv=ptr;
      indpos=indice;
      encours=parcours->suiv;
    }
    taille++;
}


/*---------------------------------------------------------------------------
-----   NOM     : Ajouter_element                                       -----
-----------------------------------------------------------------------------
-----   Cette fonction permet d'ajouter un élément en fin de liste      -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'élément à insérer                                   -----
-----   SORTIE  : rien                                                  -----
-----------------------------------------------------------------------------
-----   Peut générer l'exception: "bad alloc"                           -----
---------------------------------------------------------------------------*/
template <class T> void Cliste<T>::ajouter_element(T * valeur)
{
        //Cexception E;
        TCell<T> *ptr,*parcours;
     //   int loop=0;

        // Copie de la cellule
        ptr=new TCell<T>;
        ptr->val=valeur;
        ptr->suiv=NULL;

        // Insertion à la dernière position
/*      if (taille==0) liste=ptr;
        else
        {
		 if (indpos==(taille-1))
		 {
		  encours->suiv=ptr;
		  encours=ptr;
		  indpos++;
		 } else
		 {
          if (indpos<taille-1)
                {
                 parcours=encours;
                 loop=indpos;
                } else
                {
                 parcours=liste;
                }
          while (loop<taille-1) 
          {
                 parcours=parcours->suiv;
                 loop++;
          }
          indpos=loop;
          encours=parcours;
          ptr->suiv=parcours->suiv;
          parcours->suiv=ptr;
		 }
        }*/
      if (taille==0) 
	  {
		  liste=ptr;
		  last=liste;
	  } else
        { 
		  ptr->suiv=NULL;
          last->suiv=ptr;
		  last=ptr;
		 }
 taille++;
}

/*---------------------------------------------------------------------------
-----   NOM     : Supprimer_position                                                                            -----
-----------------------------------------------------------------------------
-----   Cette fonction permet de supprimer un élément en ième position  -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'indice de l'élément à supprimer                                             -----
-----   SORTIE  : rien                                                                                                  -----
-----------------------------------------------------------------------------
-----   Peut générer l'exception: "Suppression impossible"                              -----
---------------------------------------------------------------------------*/
template <class T> void Cliste<T>::Supprimer_position(unsigned  int indice)
{
        //Cexception E;
        TCell<T> *ptr,*parcours;
        unsigned int i,depart;

        //if (indice<0 || indice>=taille) throw(E.modifier_valeur(Suppression_impossible));
        // On peut supprimer l'élément dans la liste
        
        if (indice==0)
        {
                //Suppression en début de liste
                ptr=liste;
                liste=ptr->suiv;
				if (taille==1) last=NULL;
                delete ptr;
        } else
        {
			    if (indice<=indpos)
				{
					parcours=liste;
					depart=0;
				} else
				{
				    parcours=encours;
					depart=indpos;
				}
                // Parcours jusqu'au i-1ème élément
                for (i=depart;i<indice-1;i++) parcours=parcours->suiv;

                // Suppression du ième élément          
                ptr=parcours->suiv;
                parcours->suiv=ptr->suiv;
				if (ptr==last) last=parcours;
                delete ptr;
        }
        taille--;
        if (indice==indpos) indpos=99999999;
        if (indice<indpos) indpos--;
        
}

/*---------------------------------------------------------------------------
-----   NOM     : Modifier_position                                                                                 -----
-----------------------------------------------------------------------------
-----   Cette fonction permet de modifier un élément en ième position   -----
-----------------------------------------------------------------------------
-----   ENTREE  : l'indice de l'élément à modifier et sa valeur                 -----
-----   SORTIE  : rien                                                                                                  -----
-----------------------------------------------------------------------------
-----   Peut générer l'exception: "Suppression impossible"                              -----
---------------------------------------------------------------------------*/
template <class T> void Cliste<T>::Modifier_position(unsigned  int valeur, unsigned  int indice)
{
        //Cexception E;
        TCell<T> *ptr;
        unsigned  int loop=0;

        //if (indice<0 || indice>=taille) throw(E.modifier_valeur(Suppression_impossible));
        // On peut supprimer l'élément dans la liste
        
        if (indice<indpos)
        {
         ptr=liste;
         if (indice !=0)
          while (loop<indice) 
          {
           ptr=ptr->suiv;
           loop++;
          }
        } else
        {
         ptr=encours;
         for (loop=0;loop<(indice-indpos);loop++) ptr=ptr->suiv;
        }

        ptr->val=valeur;
}


template <class T>  BIGINT Cliste<T>::nbobj=0;
template <class T> BIGINT TCell<T>::nbobj=0;
