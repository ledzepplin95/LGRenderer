#pragma once

class QMC
{
private:
	QMC(void);
	static int nextPrime(int p);

public:
    static double riVDC(int bits,int r);
	static double riS(int i,int r);
	static double riLP(int i,int r);
	static double halton(int d,int i);
	static double mod1(double x);
	static int sigma(int i,int order);
	static int getFibonacciRank(int n);
	static int fibonacci(int k);
	static double fibonacciLattice(int k,int i,int d);
	static double reducedCPRotation(int k,int d,double x0,double x1);
	static double korobovLattice(int m,int i,int d);
	static void initial();

	static int MAX_SIGMA_ORDER;

private:	
	static int NUM;
	static vector<vector<int>> SIGMA;
	static int PRIMES[128];
	static int FIBONACCI[47];
	static double FIBONACCI_INV[47];
	static double KOROBOV[128];

};
