#pragma once

class Solvers
{
public:
	static vector<double> solveQuartic(double a,double b,
		double c,double d,double e);
    static vector<double> solveQuadric(double a,double b,double c);

private:
	static double solveCubicForQuartic(double p,double q,double r);	
};
