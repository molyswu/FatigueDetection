#pragma once
#ifdef __cplusplus
extern "C" {
#endif


#include "matrix.h"

/*
standard Kalman filter (in G. Welch' and G. Bishop's notation):

  x(k)=A*x(k-1)+B*u(k)+w(k)  p(w)~N(0,Q)
  z(k)=H*x(k)+v(k),   p(v)~N(0,R)
*/
typedef struct TxKalman
{
	int MP;                     /* number of measurement vector dimensions */
	int DP;                     /* number of state vector dimensions */
	int CP;                     /* number of control vector dimensions */
	
    TxMat* statePre;           /* predicted state (x'(k)):
                                    x(k)=A*x(k-1)+B*u(k) */
    TxMat* statePost;          /* corrected state (x(k)):
                                    x(k)=x'(k)+K(k)*(z(k)-H*x'(k)) */
    TxMat* transitionMatrix;   /* state transition matrix (A) */
    TxMat* controlMatrix;      /* control matrix (B)
                                   (it is not used if there is no control)*/
    TxMat* measurementMatrix;  /* measurement matrix (H) */
    TxMat* processNoiseCov;   /* process noise covariance matrix (Q) */
    TxMat* measurementNoiseCov; /* measurement noise covariance matrix (R) */
    TxMat* errorCovPre;       /* priori error estimate covariance matrix (P'(k)):
                                    P'(k)=A*P(k-1)*At + Q)*/
    TxMat* gain;                /* Kalman gain matrix (K(k)):
                                    K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R)*/
    TxMat* errorCovPost;      /* posteriori error estimate covariance matrix (P(k)):
                                    P(k)=(I-K(k)*H)*P'(k) */
    TxMat* temp1;               /* temporary matrices */
    TxMat* temp2;
    TxMat* temp3;
    TxMat* temp4;
    TxMat* temp5;
	TxMat* temp6;
	TxMat* temp7;
	TxMat* temp8;
	TxMat* temp9;
} TxKalman;


/* Creates Kalman filter and sets A, B, Q, R and state to some initial values */
TxKalman* txCreateKalman( int dynam_params, int measure_params,
                                 int control_params);

/* Releases Kalman filter state */
void  txReleaseKalman( TxKalman** _kalman);

/* Updates Kalman filter by time (predicts future state of the system) */
const TxMat*  txKalmanPredict( TxKalman* kalman,
                                      const TxMat* control);

/* Updates Kalman filter by measurement
   (corrects state of the system and internal matrices) */
const TxMat*  txKalmanCorrect( TxKalman* kalman, const TxMat* measurement );



#ifdef __cplusplus
}
#endif
