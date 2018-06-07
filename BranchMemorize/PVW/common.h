#pragma once
#define BIGINT  long long
#define BIGINT_MAX LLONG_MAX

#define HASHITEMS 500	//! default 500. should be set biger: 2000. Finally i decide to keep this unchanged and only change dbdimension
#define NB	200			
#define DBMaximumDimension 5500000	//! default 150000, set to 6000000 when testing memo, set to 5500000 when pmax=1000


#define DBMAXJOBS NB
#define maxtrav NB
#define HASH 1