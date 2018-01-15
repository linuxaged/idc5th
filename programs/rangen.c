#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "idc.h"

/*******************************************************************************
*NOTICE:                                                                       *
*This code is believed by the author to be bug free.  You are free to use and  *
*modify this code with the understanding that any use, either direct or        *
*derivative, must contain acknowledgement of its author and source.  The author*
*makes no warranty of any kind, expressed or implied, of merchantability or    *
*fitness for a particular purpose.  The author shall not be held liable for any*
*incidental or consequential damages in connection with or arising out of the  *
*furnishing, performance, or use of this software.  This software is not       *
*authorized for use in life support devices or systems.                        *
********************************************************************************/

/* The uniform random number generator used in this routine was written by
W.P. Petersen, IPS, ETH Zuerich, 19 Mar. 92 */

/* Common Block Declarations */

struct klotz0_1_ {
    double buff[607];
    int ptr;
};

#define klotz0_1 (*(struct klotz0_1_ *) &klotz0_)
#define min(a,b) (a<b)?a:b

struct klotz1_1_ {
    double xbuff[1024];
    int first, xptr;
};

#define klotz1_1 (*(struct klotz1_1_ *) &klotz1_)

/* Initialized data */

struct {
    int fill_1[1214];
    int e_2;
    } klotz0_ = { {0}, 0 };

struct {
    double fill_1[1024];
    int e_2[2];
    double e_3;
    } klotz1_ = { {0}, 0, 0, 0. };


/* Table of constant values */



float rangen(int choice) 

    {float x, x1, x2, w, y, r;
     double a[1];
     int n;
      n=1;

     if (choice == 0)	 /* Uniform random number with mean 0 and variance 1 */
	{
         zufall_(n,a);
         r = (float) a[0];
         x = (r - .5) * sqrt(12.);
	}
     else if (choice == 1)   /* Gaussian random number gen with mean=0,var=1 */
	{while(1)
	    {
             zufall_(n,a);
             x1 = (float) a[0];
             zufall_(n,a);
	     x2 = (float) a[0];
	     x1 = (x1-.5)*2.0;
	     x2 = (x2-.5)*2.0;
	     w = x1*x1+x2*x2;
	     if (w < 1.0 && w > 0.0) break;
	    }
	 x = x1* sqrt(-2.0*log(w)/w);
	 x *= x2/x1;
	}
     else if (choice == 2)		/* Laplacian random number generator */
	{
         zufall_(n,a);
         r = (float) a[0];
         x=pow(r,sqrt(.5));
	 y=log(x);
         zufall_(n,a);
         r = (float) a[0];
	 if (0.5 - r >= 0)
	    x = myabs(y);
	 else
	    x = -1.0 * myabs(y);
	}
     else if (choice == 3)		     /* Gamma random number generator*/
	{
         zufall_(n,a);
         x = (float) a[0];
	 x = pow((-log(x)/1.5),1.6);
         zufall_(n,a);
	 if (0.5 - (float) a[0] >= 0)
	    x = myabs(x / 1.4799);
	 else
	    x = -1.0 * myabs(x / 1.4799);
	}

     return x;
    }
							   /* end of randgen */


int zufall_(n, a)
int n;
double *a;
{
    int buffsz = 607;

    int left, aptr, bptr, aptr0, i, k, q;
    double t;
    int nn, vl, qq, k273, k607, kptr;


/* portable lagged Fibonacci series uniform random number */
/* generator with "lags" -273 und -607: */
/* W.P. Petersen, IPS, ETH Zuerich, 19 Mar. 92 */


    aptr = 0;
    nn = n;

L1:

    if (nn <= 0) {
	return 0;
    }

/* factor nn = q*607 + r */

    q = (nn - 1) / 607;
    left = buffsz - klotz0_1.ptr;

    if (q <= 1) {

/* only one or fewer full segments */

	if (nn < left) {
            kptr = klotz0_1.ptr;
	    for (i = 0; i < nn; ++i) {
		a[i + aptr] = klotz0_1.buff[kptr + i];
	    }
	    klotz0_1.ptr += nn;
	    return 0;
	} else {
            kptr = klotz0_1.ptr;
/* #pragma _CRI ivdep */
	    for (i = 0; i < left; ++i) {
		a[i + aptr] = klotz0_1.buff[kptr + i];
	    }
	    klotz0_1.ptr = 0;
	    aptr += left;
	    nn -= left;
/*  buff -> buff case */
	    vl = 273;
	    k273 = 334;
	    k607 = 0;
	    for (k = 0; k < 3; ++k) {
/* #pragma _CRI ivdep */
		for (i = 0; i < vl; ++i) {
		   t = klotz0_1.buff[k273+i]+klotz0_1.buff[k607+i];
		   klotz0_1.buff[k607+i] = t - (double) ((int) t);
		}
		k607 += vl;
		k273 += vl;
		vl = 167;
		if (k == 0) {
		    k273 = 0;
		}
	    }
	    goto L1;
	}
    } else {

/* more than 1 full segment */

        kptr = klotz0_1.ptr;
/* #pragma _CRI ivdep */
	for (i = 0; i < left; ++i) {
	    a[i + aptr] = klotz0_1.buff[kptr + i];
	}
	nn -= left;
	klotz0_1.ptr = 0;
	aptr += left;

/* buff -> a(aptr0) */

	vl = 273;
	k273 = 334;
	k607 = 0;
	for (k = 0; k < 3; ++k) {
	    if (k == 0) {
/* #pragma _CRI ivdep */
		for (i = 0; i < vl; ++i) {
		    t = klotz0_1.buff[k273+i]+klotz0_1.buff[k607+i];
		    a[aptr + i] = t - (double) ((int) t);
		}
		k273 = aptr;
		k607 += vl;
		aptr += vl;
		vl = 167;
	    } else {
/* #pragma _CRI ivdep */
		for (i = 0; i < vl; ++i) {
		    t = a[k273 + i] + klotz0_1.buff[k607 + i];
		    a[aptr + i] = t - (double) ((int) t);
		}
		k607 += vl;
		k273 += vl;
		aptr += vl;
	    }
	}
	nn += -607;

/* a(aptr-607) -> a(aptr) for last of the q-1 segments */

	aptr0 = aptr - 607;
	vl = 607;

	for (qq = 0; qq < q-2; ++qq) {
	    k273 = aptr0 + 334;
/* #pragma _CRI ivdep */
	    for (i = 0; i < vl; ++i) {
		t = a[k273 + i] + a[aptr0 + i];
		a[aptr + i] = t - (double) ((int) t);
	    }
	    nn += -607;
	    aptr += vl;
	    aptr0 += vl;
	}

/* a(aptr0) -> buff, last segment before residual */

	vl = 273;
	k273 = aptr0 + 334;
	k607 = aptr0;
	bptr = 0;
	for (k = 0; k < 3; ++k) {
	    if (k == 0) {
/* #pragma _CRI ivdep */
		for (i = 0; i < vl; ++i) {
		    t = a[k273 + i] + a[k607 + i];
		    klotz0_1.buff[bptr + i] = t - (double) ((int) t);
		}
		k273 = 0;
		k607 += vl;
		bptr += vl;
		vl = 167;
	    } else {
/* #pragma _CRI ivdep */
		for (i = 0; i < vl; ++i) {
		    t = klotz0_1.buff[k273 + i] + a[k607 + i];
		    klotz0_1.buff[bptr + i] = t - (double) ((int) t);
		}
		k607 += vl;
		k273 += vl;
		bptr += vl;
	    }
	}
	goto L1;
    }
} /* zufall_ */


int zufalli_(seed)
int seed;
{
    /* Initialized data */

    int kl = 9373;
    int ij = 1802;

    /* Local variables */
    int i, j, k, l, m;
    double s, t;
    int ii, jj;


/*  generates initial seed buffer by linear congruential */
/*  method. Taken from Marsaglia, FSU report FSU-SCRI-87-50 */
/*  variable seed should be 0 < seed <31328 */


    if (seed != 0) {
	ij = seed;
    }

    i = ij / 177 % 177 + 2;
    j = ij % 177 + 2;
    k = kl / 169 % 178 + 1;
    l = kl % 169;
    for (ii = 0; ii < 607; ++ii) {
	s = 0.;
	t = .5;
	for (jj = 1; jj <= 24; ++jj) {
	    m = i * j % 179 * k % 179;
	    i = j;
	    j = k;
	    k = m;
	    l = (l * 53 + 1) % 169;
	    if (l * m % 64 >= 32) {
		s += t;
	    }
	    t *= (double).5;
	}
	klotz0_1.buff[ii] = s;
    }
    return 0;
} /* zufalli_ */


