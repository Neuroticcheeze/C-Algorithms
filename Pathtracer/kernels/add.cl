#include "cashew_std.clinc"

typedef struct _brdf_in
{
	half3 rayDir;
	
} brdf_in;

KERNEL simple_add(global const int* A, global const int* B, global int* C, int d, private const brdf_in inData)
{
	global int *p = C + gidX;

    deref(p) = A[gidX] + B[gidX];
	deref(p) += d;
	deref(p) = (int)pow(	(double)gidX,	cast_deref(double, p) * 0.01	);
}