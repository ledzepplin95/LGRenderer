#include "StdAfx.h"
#include "Solvers.h"

vector<double> Solvers::solveQuadric(double a,double b,double c) 
{
	vector<double> sols;
	double disc=b*b-4.0*a*c;
	if(disc<0.0)
		return sols;
	disc=sqrt(disc);
	double q=(b<0.0)?-0.5*(b-disc):-0.5*(b+disc);
	double t0=q/a;
	double t1=c/q;		
	if(t0>t1)
	{
		sols.push_back(t1);
		sols.push_back(t0);
	}
	else
	{
		sols.push_back(t0);
		sols.push_back(t1);
	}

	return sols;
}

vector<double> Solvers::solveQuartic(double a,double b,double c,double d,double e)
{
	double inva=1.0/a;
	double c1=b*inva;
	double c2=c*inva;
	double c3=d*inva;
	double c4=e*inva;	
	double c12=c1*c1;
	double p=-0.375*c12+c2;
	double q=0.125*c12*c1-0.5*c1*c2+c3;
	double r=-0.01171875*c12*c12+0.0625*c12*c2-0.25*c1*c3+c4;
	double z=solveCubicForQuartic(-0.5*p,-r,0.5*r*p-0.125*q*q);
	double d1=2.0*z-p;
	if(d1<0.0) 
	{
		if (d1>1.0e-10)
			d1=0.0;
		else
			return vector<double>();
	}
	double d2;
	if(d1<1.0e-10) 
	{
		d2=z*z-r;
		if(d2<0.0)
			return vector<double>();
		d2=sqrt(d2);
	} 
	else 
	{
		d1=sqrt(d1);
		d2=0.5*q/d1;
	}
	double q1=d1*d1;
	double q2=-0.25*c1;
	double pm=q1-4.0*(z-d2);
	double pp=q1-4.0*(z+d2);
	if(pm>=0.0 && pp>=0.0) 
	{		
		pm=sqrt(pm);
		pp=sqrt(pp);
		vector<double> results;
		results.resize(4);
		results[0]=-0.5*(d1+pm)+q2;
		results[1]=-0.5*(d1-pm)+q2;
		results[2]=0.5*(d1+pp)+q2;
		results[3]=0.5*(d1-pp)+q2;		
		for(int i=1; i<4; i++)
		{
			for(int j=i; j>0 && results[j-1]>results[j]; j--) 
			{
				double t=results[j];
				results[j]=results[j-1];
				results[j-1]=t;
			}
		}

		return results;
	}
	else if(pm>=0.0)
	{
		pm=sqrt(pm);
		vector<double> results;
		results.resize(2);
		results[0]=-0.5*(d1+pm)+q2;
		results[1]=-0.5*(d1-pm)+q2;

		return results;
	} 
	else if(pp>=0.0)
	{
		pp=sqrt(pp);
		vector<double> results;
		results.resize(2);
		results[0]=0.5*(d1-pp)+q2;
		results[1]=0.5*(d1+pp)+q2;

		return results;
	}

	return vector<double>();
}

double Solvers::solveCubicForQuartic(double p,double q,double r) 
{
	double A2=p*p;
	double Q=(A2-3.0*q)/9.0;
	double R=(p*(A2-4.5*q)+13.5*r)/27.0;
	double Q3=Q*Q*Q;
	double R2=R*R;
	double d=Q3-R2;
	double an=p/3.0;
	if(d>=0.0)
	{
		d=R/sqrt(Q3);
		double theta=acos(d)/3.0;
		double sQ=-2.0*sqrt(Q);

		return sQ*cos(theta)-an;
	} 
	else
	{
		double sQ=pow(sqrt(R2-Q3)+fabs(R),1.0/3.0);
		if(R<0.0)
			return (sQ+Q/sQ)-an;
		else
			return -(sQ+Q/sQ)-an;
	}
}
