/*
PID Library

Original Author: Greg Young, Z-World.

	The PID (Proportional Integral Derivative) function is used in mainly
	control applications. PIDCalc performs one iteration of the PID
	algorithm.

*/
#ifndef pid_h
#define pid_h

typedef struct PID	{
	double	SetPoint;         //	Desired Value

	double	Proportion;			//	Proportional Const
	double	Integral;			//	Integral Const
	double	Derivative;			//	Derivative Const

	double	LastError;			//	Error[-1]
	double	PrevError;			//	Error[-2]
	double	SumError;			//	Sums of Errors
}	PID;

/*=========================================================================*\
   Perform One PID Iteration
\*=========================================================================*/

double PIDCalc(PID *pp, double NextPoint);

/*=========================================================================*\
   Initialize PID Structure
\*=========================================================================*/

void PIDInit(PID *pp);

#endif
