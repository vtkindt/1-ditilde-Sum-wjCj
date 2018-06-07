#pragma once
#include "common.h"
//	Cette classe, toute con, représente un vecteur de critères [wCsum, Lmax]

class C_VectCrit
{
public:
	static BIGINT cpteur; //compteur d'instances
	BIGINT wCsum;
	BIGINT Lmax;		

public:
	// Constructeurs et Destructeurs
	C_VectCrit();
	C_VectCrit(BIGINT wCsum, BIGINT Lmax);
	~C_VectCrit();
	void modifier(BIGINT wCsum, BIGINT Lmax);
};
