#include <stdio.h>
#include "C_VectCrit.hpp"


BIGINT C_VectCrit::cpteur=0;

C_VectCrit::C_VectCrit()
{
	cpteur++;
}

C_VectCrit::C_VectCrit(BIGINT val1, BIGINT val2)
{
	cpteur++;
	wCsum=val1;
	Lmax=val2;
}

C_VectCrit::~C_VectCrit()
{
	cpteur--;
}

void C_VectCrit::modifier(BIGINT val1, BIGINT val2)
{
	wCsum=val1;
	Lmax=val2;
}