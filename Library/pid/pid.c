/*
PID Library

Original Author: Greg Young, Z-World.

	The PID (Proportional Integral Derivative) function is used in mainly
	control applications. PIDCalc performs one iteration of the PID
	algorithm.

*/
#include <stdlib.h>
#include <stdio.h>
#include "LPC21xx_SFE.h"
#include "main.h"
#include "pid.h"

/*=========================================================================*\
   Perform One PID Iteration
\*=========================================================================*/
double PIDCalc(PID *pp, double NextPoint)
{	
	double	dError,	Error;

	pp->SumError += (Error = pp->SetPoint - NextPoint);
	dError = pp->LastError - pp->PrevError;
	pp->PrevError = pp->LastError;
	pp->LastError = Error;
	return   (pp->Proportion * Error
		   +  pp->Integral * pp->SumError
		   +  pp->Derivative * dError
		   );
}

/*=========================================================================*\
   Initialize PID Structure
\*=========================================================================*/
void PIDInit(PID *pp)
{     
	memset ( pp,0,sizeof(PID) );
}
