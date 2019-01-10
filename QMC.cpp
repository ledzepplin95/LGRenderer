#include "StdAfx.h"

#include "QMC.h"
#include "assert.h"
#include "StrUtil.h"

class vinitQMC
{
public:
	vinitQMC(void);		
};

int QMC::MAX_SIGMA_ORDER=15;
int QMC::NUM=128;
vector<vector<int>> QMC::SIGMA(128);
int QMC::PRIMES[128]={0};
int QMC::FIBONACCI[47]={0};
double QMC::FIBONACCI_INV[47]={0.0};
double QMC::KOROBOV[128]={0.0};
static vinitQMC ini;

QMC::QMC(void)
{
}

vinitQMC::vinitQMC()
{
	QMC::initial();
}

void QMC::initial()
{
	StrUtil::PrintPrompt("初始化Faure不规则表...");	
	PRIMES[0]=2;
	for(int i=1; i<NUM; i++)
	{
		PRIMES[i]=nextPrime(PRIMES[i-1]);
	}
	
	vector<vector<int>> table;
	table.resize(PRIMES[NUM-1]+1);
    table[2].resize(2);
	table[2][0]=0;
	table[2][1]=1;
	for(unsigned int i=3; i<=PRIMES[NUM-1]; i++)
	{
		table[i].resize(i);
		if((i&1)==0)
		{
			vector<int> prev=table[i>>1];			
			for(int j=0; j<prev.size(); j++)
				table[i][j]=2*prev[j];
			for(int j=0; j<prev.size(); j++)
				table[i][prev.size()+j]=2*prev[j]+1;
		}
		else
		{
			vector<int> prev=table[i-1];
			int med=(i-1)>>1;		
			for(int j=0;j<med; j++)
				table[i][j]=prev[j]+((prev[j]>=med)?1:0);
			table[i][med]=med;
			for(int j=0; j<med; j++)
				table[i][med+j+1]=prev[j+med]+((prev[j+med]>=med)?1:0);
		}
	}	
	for(int i=0; i<NUM; i++)
	{
		int p=PRIMES[i];
		SIGMA[i].resize(p);
		std::copy(table[p].begin(),table[p].end(),SIGMA[i].begin());
	}	
	StrUtil::PrintPrompt("初始化格子表...");
	FIBONACCI[0]=0;
	FIBONACCI[1]=1;
	for(int i=2;i<47;i++) 
	{
		FIBONACCI[i]=FIBONACCI[i-1]+FIBONACCI[i-2];
		FIBONACCI_INV[i]=1.0/FIBONACCI[i];
	}
	KOROBOV[0]=1;
	for(int i=1; i<NUM; i++)
	{
		KOROBOV[i]=203*KOROBOV[i-1];
	}
}

int QMC::nextPrime(int pp) 
{
	int p=pp;
	p=p+(p&1)+1;
	while(TRUE) 
	{
		int div=3;
		BOOL isPrime=TRUE;
		while( isPrime && ((div*div)<=p) )
		{
			isPrime=((p%div)!=0);
			div+=2;
		}
		if(isPrime)
			return p;
		p+=2;
	}
}

double QMC::riVDC(int bits,int r) 
{
	bits=(bits<<16) | (bits>>16);
	bits=((bits&0x00ff00ff)<<8) | ((bits&0xff00ff00)>>8);
	bits=((bits&0x0f0f0f0f)<<4) | ((bits&0xf0f0f0f0)>>4);
	bits=((bits&0x33333333)<<2) | ((bits&0xcccccccc)>>2);
	bits=((bits&0x55555555)<<1) | ((bits&0xaaaaaaaa)>>1);
	bits^=r;

	return (double)(bits&0xFFFFFFFFL)/(double)0x100000000L;
}

double QMC::riS(int i,int r)
{
	for(int v=1<< 31; i!=0; i>>=1, v^=v>>1)
		if((i&1)!=0) r^=v;

	return (double)(r&0xFFFFFFFFL)/(double)0x100000000L;
}

double QMC::riLP(int i,int r) 
{
	for(int v=1<<31; i!=0; i>>=1, v|=v>>1)
		if((i&1)!=0) r^=v;

	return (double)(r&0xFFFFFFFFL)/(double)0x100000000L;
}

double QMC::halton(int d,int i)
{	
	switch(d) 
	{
	case 0:
		{
			i=(i<<16) | (i>>16);
			i=((i&0x00ff00ff)<<8) | ((i&0xff00ff00)>>8);
			i=((i&0x0f0f0f0f)<<4) | ((i&0xf0f0f0f0)>>4);
			i=((i&0x33333333)<<2) | ((i&0xcccccccc)>>2);
			i=((i&0x55555555)<<1) | ((i&0xaaaaaaaa)>>1);

			return (double)(i&0xFFFFFFFFL)/(double)0x100000000L;
		}
	case 1: 
		{
			double v=0.0;
			double inv=1.0/3.0;
			double p;
			int n;
			for(p=inv, n=i; n!=0; p*=inv, n/=3)
				v+=(n%3)*p;

			return v;
		}
	default:
		break;
	}
	const int& base=PRIMES[d];
	const vector<int>& perm=SIGMA[d];
	double v=0.0;
	double inv=1.0/base;
	double p;
	int n;
	for(p=inv, n=i; n!=0; p*=inv, n/=base)
	{
		int nn=n%base;
		v+=perm[nn]*p;
	}

	return v;
}

double QMC::mod1(double x) 
{	
	return x-(int)x;
}

int QMC::sigma(int i,int order)
{
	ASSERT( order>0 && order<32 );
	ASSERT( i>=0 && i<(1<<order) );
	i=(i<<16) | (i>>16);
	i=((i&0x00ff00ff)<<8) | ((i&0xff00ff00)>>8);
	i=((i&0x0f0f0f0f)<<4) | ((i&0xf0f0f0f0)>>4);
	i=((i&0x33333333)<<2) | ((i&0xcccccccc)>>2);
	i=((i&0x55555555)<<1) | ((i&0xaaaaaaaa)>>1);

	return (unsigned int)i>>(32-order);
}

int QMC::getFibonacciRank(int n)
{
	int k=3;
	while(FIBONACCI[k]<=n)
		k++;

	return k-1;
}

int QMC::fibonacci(int k) 
{
	return FIBONACCI[k];
}

double QMC::fibonacciLattice(int k,int i,int d) 
{
	return d==0?i*FIBONACCI_INV[k]:
		mod1((i*FIBONACCI[k-1])*FIBONACCI_INV[k]);
}

double QMC::reducedCPRotation(int k,int d,double x0,double x1) 
{
	int j1=FIBONACCI[2*((k-1)>>2)+1];
	int j2=FIBONACCI[2*((k+1)>>2)];
	if(d==1) 
	{
		j1=((j1*FIBONACCI[k-1])%FIBONACCI[k]);
		j2=((j2*FIBONACCI[k-1])%FIBONACCI[k])-FIBONACCI[k];
	}

	return (x0*j1+x1*j2)*FIBONACCI_INV[k];
}

double QMC::korobovLattice(int m,int i,int d) 
{
	return mod1(i*KOROBOV[d]/(1<<m));
}
