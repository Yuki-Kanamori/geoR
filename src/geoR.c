/*
  ===========================
  
  #include "stdio.h"
  #include "strings.h"
  #include "iostream.h"
*/

#include <stdio.h>
#include <stdlib.h> /* for malloc & free */
#include "memory.h"
#include <math.h> 
#include <R.h>

#define Integer long
#define Real double


void diag_quadraticform_XAX(Real *lower, Real *diag, Real *xvec, 
			    Integer *nx, Integer *n, Real *res)
     /*
       This function computes quadratic forms of the type x'Ax
       where A is a symmetric matrix.
       If X is a matrix (passed to C in a vector form) the quadratic
       form is computed for each column of X
       
       lower  : lower triangle of the A matrix
       diag   : diagonal of the A matrix
       xvec   : the vector(s) for the quad. form.
       nx     : number of vectors of type x 
       (= number of quadratic forms to be computed)
       n      : size of each vector
       res    : store the result(s)
       
       author : Paulo J.Ribeiro Jr , 03/11/00
     */
     
{
  Integer pos;
  Integer register i,j,k;
  Real xij, xii;
  
  for(k=0; k<*nx; k++){
    
    pos = 0;
    xij = 0.0;
    for (j=0; j<(*n-1); j++) {
      for (i=j+1; i<*n; i++) {
	xij += (xvec[(k*(*n) + j)] * xvec[(k*(*n) + i)] * lower[pos++]) ;
      }
    }
    
    pos = 0;
    xii = 0.0;
    for (i=0; i<*n; i++){
      xii += (xvec[(k*(*n) + i)] * xvec[(k*(*n) + i)] * diag[i]) ;
    }
    
    res[k] = xii + 2*xij ;    
  }
  
}

void bilinearform_XAY(Real *lower, Real *diag, Real *xvec, 
		       Real *yvec, Integer *nx, Integer *ny, 
		       Integer *n, Real *res)
     /*
       This function computes retangular forms of the type x'Ay
       where A is a symmetric matrix.
       X and Y can be matrices in whic cases quadratic formas are computed
       for each row of X combined with each column of Y
       
       lower  : lower triangle of the A matrix
       diag   : diagonal of the A matrix
       xvec   : the vector(s) for the quad. form.
       yvec   : the vector y in the quad. form.
       nx     : number of vectors of type x
       (= number of quadratic forms to be computed)
       ny     : number of vectors in yvec
       n      : dimension of the matix A 
       (also size of each vector yvec and xvec)
       res    : store the result(s)
       
       author: Paulo J.Ribeiro Jr , 03/11/00
     */
     
{
  Integer pos;  
  Integer register i,j,k,l;
  Real xyij, xyji, xyii;
  
  for (l=0; l<*ny; l++){
    
    for(k=0; k<*nx; k++){
      pos = 0;
      xyij = 0.0;
      xyji = 0.0;
      for (j=0; j<(*n-1); j++) {
	for (i=j+1; i<*n; i++) {
	  xyji += (xvec[(k*(*n) + j)] * lower[pos] * yvec[(l*(*n) + i)]) ;
	  xyij += (xvec[(k*(*n) + i)] * lower[pos] * yvec[(l*(*n) + j)]) ;
	  pos++ ;
	}
      }
      xyii = 0.0;
      for (i=0; i<*n; i++){
	xyii += (xvec[(k*(*n) + i)] * diag[i] * yvec[(l*(*n) + i)]) ;
      }
      res[(l * (*nx) + k)] = xyii + xyij + xyji ;
    }
  }
}

void loccoords(Real *xloc, Real *yloc, Real *xcoord, Real *ycoord, 
	       Integer *nl, Integer *nc, Real *res) 
     /* This function computes the distance between each data location
	to each of the prediction location
	
        xloc, yloc     : xy coordinates of the prediction locations
        xcoord, ycoord : xy coordinates of the data points
	nl, nc         : number of prediction locations and data locations
	res            : stores the results to be returned, 
	a vector with distances
     */   
     
{ 
  Integer register i,j, ind;
  Real register dx,dy;
  
  ind = 0;
  for (j=0; j<*nl; j++) {  
    for (i=0; i<*nc; i++) {
      dx = (xloc[j] - xcoord[i]) ;
      dy = (yloc[j] - ycoord[i]) ;
      res[ind++] = sqrt(dx*dx + dy*dy) ;
    }
  }
  
}

void tgangle(Real *xloc, Real *yloc, Integer *nl, Real *res) 
     /* This function computes the tangent of the (azimuth) 
        angle between pairs of locations
	
        xloc, yloc     : xy coordinates of the locations
	nl,            : number of locations
	res            : stores the results to be returned, 
	                 a vector with tangent of the angles
     */   
     
{ 
  Integer register i,j, ind;
  Real register dx,dy;
  
  ind = 0;
  for (j=0; j<*nl; j++) {  
    for (i=j+1; i<*nl; i++) {
      dx = (xloc[i] - xloc[j]) ;
      dy = (yloc[i] - yloc[j]) ;
      res[ind] = dx/dy ;
      ind++ ;
    }
  }
  
}

void distdiag(Real *xloc, Real *yloc, Integer *nl, Real *res) 
     /* This function computes the distances between locations
	including the diagonal term
	
        xloc, yloc     : xy coordinates of the locations
	nl,            : number of locations
	res            : stores the results to be returned, 
	                 a vector with distances
     */   
     
{ 
  Integer register i,j, ind;
  Real register dx,dy;
  
  ind = 0;
  for (j=0; j<*nl; j++) {  
    for (i=j; i<*nl; i++) {
      if(i==j)
	res[ind] = 0.0 ;
      else{
	dx = (xloc[j] - xloc[i]) ;
	dy = (yloc[j] - yloc[i]) ;
	res[ind] = sqrt(dx*dx + dy*dy) ;
      }
      ind++ ;
    }
  }
  
}

void binit(Integer *n, Real *xc, Real *yc, Real *sim, 
	   Integer *nbins, Real *lims, Integer *robust, 
	   Real *maxdist, Integer *cbin, Real *vbin,
	   Integer *sdcalc, Real *sdbin)
{
  
  Integer register i, j, ind=0;
  Real register v=0.0;
  Real dist=0.0, dx=0.0, dy=0.0;
  
  for (j=0; j < *n; j++)
    { 
      for (i=j+1; i<*n; i++) 
	{
	  dx = xc[i] - xc[j];
	  dy = yc[i] - yc[j];
	  dist = sqrt(dx*dx + dy*dy);
	  
	  if(dist <= *maxdist)
	    {
	      v = sim[i] - sim[j];
	      if (*robust) v = sqrt(sqrt(v*v));
	      else v = (v*v)/2.0;
	      ind = 0;
	      while (dist > lims[ind] && ind <= *nbins ) ind++ ;
	      if (dist <= lims[ind])
		{
		  vbin[(ind-1)]+= v; 
		  cbin[(ind-1)]++;
		  if(*sdcalc) sdbin[(ind-1)] += v*v;
		}
	    }
	}
    }
  
  for (j=0; j < *nbins; j++) 
    {
      if (cbin[j]){
	if(*sdcalc)
	  { 
	    sdbin[j] = sqrt((sdbin[j] - ((vbin[j] * vbin[j])/cbin[j]))/(cbin[j] - 1));
	  }
	vbin[j] = vbin[j]/cbin[j];
	if (*robust) {
	  vbin[j] = vbin[j] * vbin[j];
	  vbin[j] = (vbin[j] * vbin[j])/(0.914 + (0.988/cbin[j]));
	}
      }
    }
  
}

     
void lower_DIAGminusXAX(Real *lower, Real *diag, Real *xvec, 
	      Integer *nxcol, Integer *n, Real *Dval, Real *res)
     /*
       This function computes the lower triangle and diagonal
       of forms (D - X'AX), where D is a diagonal matrix, 
       A is a symmetric matrix and X a retangular matrix.
              
       lower  : lower triangle of the A matrix
       diag   : diagonal of the A matrix
       xvec   : matrix X in a vector form
       nxcol  : number of coluns of matrix X
       n      : dimension of the matix A 
       Dval   : element of the diagonal matrix
       res    : store the result
       
       author : Paulo J.Ribeiro Jr , 01/12/00
     */
     
{
  Integer pos;  
  Integer register i,j,k,l;
  Real xyij, xyji, xyii;
  
  for (l=0; l<*nxcol; l++){
    
    for(k=l; k<*nxcol; k++){
      pos = 0;
      xyij = 0.0;
      xyji = 0.0;
      for (j=0; j<(*n-1); j++) {
	for (i=j+1; i<*n; i++) {
	  xyji += (xvec[(k*(*n) + j)] * lower[pos] * xvec[(l*(*n) + i)]) ;
	  xyij += (xvec[(k*(*n) + i)] * lower[pos] * xvec[(l*(*n) + j)]) ;
	  pos++ ;
	}
      }
      xyii = 0.0;
      for (i=0; i<*n; i++){
	xyii += (xvec[(k*(*n) + i)] * diag[i] * xvec[(l*(*n) + i)]) ;
      }
      if (k > l)
	res[(*nxcol * l - (l * (l+1)/2) + k)] = -1 * (xyii + xyij + xyji) ;
      else
	res[(*nxcol * l - (l * (l+1)/2) + k)] = (*Dval) - (xyii + xyij + xyji) ;
    }
  }
}

void lower_R0minusXAXplusBvar(Real *lower, Real *diag, Real *xvec, 
			      Integer nxcol, Integer n, Real *Dval, 
			      Real *Blower, Real *Bdiag, Real *bvec, 
			      Integer Bsize, Real *ss, Real *res)
     /*
       This function computes the lower triangle and diagonal
       of forms (R0 - X'AX + b'Bb), where R0 is the cov.  matrix
       between prediction points; 
       A is a symmetric matrix and X a retangular matrix corresponding
       to the reduction in variance due to the data;
       B is symmetric and b is a retaulkar matrix correspond in the 
       increase in the variance due to the unknown mean
              
       lower  : lower triangle of the A matrix
       diag   : diagonal of the A matrix
       xvec   : matrix X in a vector form
       nxcol  : number of coluns of matrix X
       n      : dimension of the matrix A 
       Dval   : element of the diagonal matrix
       Blower : lower triangle  of the Var(beta) matrix 
       Bdiag  : diagonal  of the Var(beta) matrix 
       bvec   : t(b) matrix in vector form
       Bsize  : dimension of mean vector beta and its variance matrix
       res    : input lower triangle of R0 and store the result

       
       author : Paulo J.Ribeiro Jr , 04/12/00
     */
     
{
  Integer pos, bpos, indpos=0;  
  Integer register i,j,k,l;
  Real xyij, xyji, xyii;
  Real bxyij, bxyji, bxyii;
  
  for (l=0; l<nxcol; l++){
    
    for(k=l; k<nxcol; k++){
      
      /*      
	      Computing lower triangle (including diagonal) of XAX
      */
      
      pos = 0;
      xyij = 0.0;
      xyji = 0.0;
      for (j=0; j<(n-1); j++) {
	for (i=j+1; i<n; i++) {
	  xyji += (xvec[(k * n + j)] * lower[pos] * xvec[(l * n + i)]) ;
	  xyij += (xvec[(k * n + i)] * lower[pos] * xvec[(l * n + j)]) ;
	  pos++ ;
	}
      }
      xyii = 0.0;
      for (i=0; i<n; i++){
	xyii += (xvec[(k * n + i)] * diag[i] * xvec[(l * n + i)]) ;
      }
      
      /*
	Computing lower triangle (including diagonal) of bBb
      */

      bpos = 0;
      bxyij = 0.0;
      bxyji = 0.0;
      bxyii = 0.0;
      if (Bsize == 1){
	bxyii = bvec[l] * bvec[k] * (*Bdiag) ;
      }      
      else{
	for (j=0; j<(Bsize-1); j++) {
	  for (i=j+1; i<Bsize; i++) {
	    bxyji += (bvec[(k*(Bsize) + j)] * Blower[bpos] * bvec[(l*(Bsize) + i)]) ;
	    bxyij += (bvec[(k*(Bsize) + i)] * Blower[bpos] * bvec[(l*(Bsize) + j)]) ;
	    bpos++ ;
	  }
	}
	for (i=0; i<Bsize; i++){
	  bxyii += (bvec[(k*(Bsize) + i)] * Bdiag[i] * bvec[(l*(Bsize) + i)]) ;
	}
      }
      
      /*      
	      Computing lower triangle (including diagonal) of (R0 - XAX + bBb)
	      
	      indpos = (nxcol * l - (l * (l+1)/2) + k) ;
	      Rprintf("\n indpos=%d ", indpos);
      */
      
      if (k > l){
	res[indpos] += (-1 * (xyii + xyij + xyji) + (bxyii + bxyij + bxyji)) ;
	res[indpos] *= *ss ;
      }
      else{
	res[indpos] *= (*Dval) ;
	res[indpos] += (-1 * (xyii + xyij + xyji)  + (bxyii + bxyij + bxyji)) ;
	res[indpos] *= *ss ;
      }
      indpos++ ;
    }
  }
}


void chol(Real *inmatrix, Integer N)

{
  Integer register Drow, Dcol, Dcol2;
  Real register sum;
  Real register *Pcol, *anothercol;

  /* 
     returns L where L L'=inmatrix


     NR function choldc, sec. 2.9
     i=dcol, j=drow, k=dcol2
  */
  
  for(Dcol=0;Dcol<N;Dcol++) {
    Pcol=inmatrix + Dcol * N - ((Dcol * (Dcol+1))/2);

    for(Drow=Dcol;Drow<N;Drow++){
      for(sum=Pcol[Drow],Dcol2=Dcol-1;Dcol2>=0;Dcol2--) {
	anothercol = inmatrix + N * Dcol2 - ((Dcol2 * (Dcol2+1))/2);
	/*
	sum -= inmatrix[matref(Drow,Dcol2,N)]*inmatrix[matref(Dcol,Dcol2,N)];
	*/	
	sum -= anothercol[Drow]*anothercol[Dcol];
      }
      if (Drow == Dcol) {
	if (sum<=0.0) {
	  error("%s%ld%s%e", "chol: matrix not pos def, diag[" , Drow , "]= " , sum);
	  return;
	}
	Pcol[Drow]=sqrt(sum);
      } else Pcol[Drow]=sum/Pcol[Dcol];
    }
  }
  /* 
     at this point the diagonal and lower triangle of inmatrix
     contain the cholesky decomp 
  */

  return;
}


void mvnorm(Real *means, Real *Q, Real *nscores, Integer N, 
	    Integer Nsims, Real *Vsqglchi) {
  
  /* 
     returns means + chol(Q) %*% nscores
  */
  
  Integer register Drow, Dcol, Dsim, i ;
  Real *Vsim = (Real*) malloc(sizeof(Real)*N) ;


  chol(Q, N);  
  
  /*
    multiplyLower(X, Q, nscores, *N);
  */
  for(Dsim=0;Dsim<Nsims; Dsim++){    
    for(Drow=0;Drow<N;++Drow) {
      Vsim[Drow] = means[Drow] ;
      for(Dcol=0;Dcol<=Drow;++Dcol) {
	Vsim[Drow] += Q[ N * Dcol - ((Dcol * (Dcol+1))/2) + Drow] * nscores[((N * Dsim) + Dcol)];
      }
    }
    for(i=0;i<N;i++){
      nscores[(((N) * Dsim) + i)] = Vsim[i] * Vsqglchi[Dsim];
    }
  }
  
  free(Vsim);
  
  return;
  
}

/*
  on exit, for each simulation, nscores = means + chol(varmatrix) %*% nscores
*/

void kb_sim(Real *means, Real *nscores,
	    Real *lowerA, Real *diagA, 
	    Real *Xmatrix, Integer *Nbig, 
	    Integer *Nsmall, Real *Dval, 
	    Integer *Nsims, Real *Vsqglchi, Real *ss,
	    Real *Blower, Real *Bdiag, Real *bvec, 
	    Integer *Bsize, Real *R0lower) {
  /*  
      varmatrix was replaced by R0lower in the input
      
      Integer Nvarmatrix = *Nbig * (*Nbig +1)/2;
      Real *varmatrix = (Real*) malloc(sizeof(Real)*Nvarmatrix);
  */
  lower_R0minusXAXplusBvar(lowerA, diagA, Xmatrix, *Nbig, *Nsmall, Dval, 
			   Blower, Bdiag, bvec, *Bsize, ss, R0lower);
  
  mvnorm(means, R0lower, nscores, *Nbig, *Nsims, Vsqglchi);
  
  
  /*  
      free(varmatrix);
  */
  
}



void multiplyLower(Real *X, Real *A, Real *B, Integer *N) {
  
  Integer register Drow, Dcol;
  
  for(Drow=0;Drow<*N;++Drow) {
    X[Drow]=0;
    for(Dcol=0;Dcol<=Drow;++Dcol) {
      X[Drow] += A[(*N)*Dcol - ((Dcol*(Dcol+1))/2) + Drow] * B[Dcol];
    }
  }
  
  
}

void cor_diag(Real *xloc, Real *yloc, Integer *nl, Integer *cornr, Real *phi, Real *kappa, Real *res) 

     /* This function computes the lower triangle of correlation or distance 
	matrix for a set of  locations.
	(including the diagonal term of the matrix).
	
	ONLY EXPONENTIAL/SPHERICAL/GAUSSIAN MODELS IMPLEMENTED SO FAR
	 0: compute distances only
	 1: PURE NUGGET
	 2: EXPONENTIAL
	 3: SPHERICAL
	 4: GAUSSIAN
	 5: WAVE (hole effect)
	 6: CUBIC
	 7: POWER
	 8: POWERED EXPONENTIAL
	 9: CAUCHY
	10: GNEITING
	11: CIRCULAR
	12: MATERN (NOT YET IMPLEMENTED)
	13: GNEITING-MATERN (NOT YET IMPLEMENTED)

	WARNING: codes above must be the same as in the geoR/geoS function
                 "cor.number"
 
        xloc, yloc     : xy coordinates of the locations
	nl,            : number of locations
	cornr          : a number indicating the correlation model
	phi            : parameter of the correlation function (scale parameter)
	kappa          : extra parameter for some correlation functions (shape parameter)
	res            : stores the results to be returned, a vector with the 
                         lower triangle of the correlation or distance matrix
     */   
     
{ 

#define TWOPI 6.28318530717959

  Integer register i,j, ind;
  Real register dx,dy;
  Real h, hphi, hphi2, hphi4;
  
  ind = 0;
  for (j=0; j<*nl; j++) {  
    for (i=j; i<*nl; i++) {
      if(i == j){
	if(*cornr > 0) res[ind] = 1.0 ;
	else res[ind] = 0.0 ;
      }
      else{
	dx = (xloc[j] - xloc[i]) ;
	dy = (yloc[j] - yloc[i]) ;
	h  = sqrt(dx*dx + dy*dy) ;
	if(*cornr > 0){
	  if(*phi > 0){
	    hphi  = h/(*phi) ;
	    switch(*cornr)
	      {
		/* pure nugget */
	      case 1: 
		res[ind] = 0 ;
		break;
		
		/* exponential */
	      case 2:
		res[ind] = exp(-hphi) ;
		break;
		
		/* spherical */
	      case 3:
		if (h < *phi) 
		  res[ind] = 1 - (1.5 * hphi) + (0.5 * hphi *hphi * hphi) ;
		else
		  res[ind] = 0 ;
		break;
		
		/* Gaussian */
	      case 4:
		res[ind] = exp(-(hphi * hphi)) ;
		break;
		
		/* wave (hole effect) */
	      case 5:
		res[ind] = (*phi/h) * sin(hphi) ;
		break;
		
		/* cubic */
	      case 6:
		if (h < *phi){
		  hphi2 = hphi * hphi ;
		  hphi4 = hphi2 * hphi2 ;
		  res[ind] = 1 - ((7 * hphi2) -
				  (8.75 * hphi2 * hphi) + 
				  (3.5 * hphi4 * hphi) - 
				  (0.75 * hphi4 * hphi2 * hphi)) ;
		}
		else
		  res[ind] = 0 ;
		break;
		
		/* power             */
	      case 7:
		res[ind] = exp(*phi * log(h)) ;
		break;
		
		/* powered.exponential */
	      case 8:
		res[ind] = exp(-1 *  pow(hphi, *kappa))  ;
		break;
		
		/* cauchy */
	      case 9:
		res[ind] = pow((1 + (hphi * hphi)), (-1 * *kappa)) ;
		break;
		
		/* gneiting */
	      case 10:
		hphi4 = 1 - hphi;
		if (hphi4 > 0) hphi4 = pow(hphi4, 8);
		else hphi4 = 0 ;
		hphi2 = hphi * hphi ;
		res[ind] = (1 + 8 * hphi + 25 * hphi2 + 32 * (hphi2 * hphi)) * hphi4 ;
		break;
		
		/* circular (NOT IMPLEMENTED: CHECK asin) */
	      case 11:
		if(h < *phi){
		  res[ind] =  1 - (2 * (hphi * sqrt(1 - hphi * hphi)
					+ asin(hphi)))/(TWOPI/2) ;
		}
		else
		  res[ind] = 0 ;
		break;
		
		/* matern (for integer kappa) NOT YET IMPLEMENTED 
		   case 11:
		   res[ind] = ;
		   break;
		*/
		/* gneiting-matern (for integer kappa) NOT YET IMPLEMENTED
		   case 12:
		   res[ind] =  ;
		   break;
		*/
	      }
	  }
	  else res[ind] = 0;
	}
	else
	  res[ind] = h ;
      }
      ind++ ;
    }
  }
  
}

