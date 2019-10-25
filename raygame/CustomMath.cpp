#include "CustomMath.h"

float ABS(float _ChangingNumber)
{
	return (_ChangingNumber > 0) ? _ChangingNumber : -_ChangingNumber;
}

float NABS(float _ChangingNumber)
{
	return -ABS(_ChangingNumber);
}
