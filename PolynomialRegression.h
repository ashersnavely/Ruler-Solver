#ifndef _POLYNOMIAL_REGRESSION_H
#define _POLYNOMIAL_REGRESSION_H

//Not mine, credit to some place on the internet at a time I had too little sleep to care.. I just cleaned it up mostly I think

//Polynomial Fit
#include<iostream>
#include<iomanip>
#include<cmath>
using namespace std;
static vector<double> polynomialRegression(vector<int> xVec, vector<int> yVec, int degree) {
    if (xVec.size() != yVec.size()) { return vector<double>(); }

    //Array that will store the values of sigmaX(xi),sigmaX(xi^2),sigmaX(xi^3)....sigmaX(xi^2n)
    double sigmaX[2 * degree + 1];
    for (int i = 0; i < 2 * degree + 1; i++) {
        sigmaX[i] = 0;
        for (int j = 0; j < xVec.size(); j++) {
            //consecutive positions of the array will store dataPairCount,sigmaX(xi),sigmaX(xi^2),sigmaX(xi^3)....sigmaX(xi^2n)
            sigmaX[i] = sigmaX[i] + pow(xVec[j], i);
        }
    }

    //normMtrx is the Normal matrix(augmented) that will store the equations, 'coeff' is for value of the final coefficients
    double normMtrx[degree + 1][degree + 2];
    for (int i = 0; i <= degree; i++) {
        for (int j = 0; j <= degree; j++) {
            //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
            normMtrx[i][j] = sigmaX[i + j];
        }
    }

    //Array to store the values of sigmaX(yi),sigmaX(xi*yi),sigmaX(xi^2*yi)...sigmaX(xi^degree*yi)
    double sigmaY[degree + 1];
    for (int i = 0; i < degree + 1; i++) {
        sigmaY[i] = 0;
        for (int j = 0; j < xVec.size(); j++) {
            //consecutive positions will store sigmaX(yi),sigmaX(xi*yi),sigmaX(xi^2*yi)...sigmaX(xi^degree*yi)
            sigmaY[i] = sigmaY[i] + pow(xVec[j], i) * yVec[j];
        }
    }

    for (int i=0; i <= degree; i++) {
        //load the values of sigmaY as the last column of normMtrx(Normal Matrix but augmented)
        normMtrx[i][degree + 1] = sigmaY[i];
    }
    //degree is made degree+1 because the Gaussian Elimination part below was for degree equations,
        //but here degree is the degree of polynomial and for degree degree we get degree+1 equations
    degree++;

    //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
    for (int i = 0; i < degree; i++) {
        for (int k = i + 1; k < degree; k++) {
            if (normMtrx[i][i] < normMtrx[k][i]) {
                for (int j = 0; j <= degree; j++) {
                    double temp = normMtrx[i][j];
                    normMtrx[i][j] = normMtrx[k][j];
                    normMtrx[k][j] = temp;
                }
            }
        }
    }

    //loop to perform the gauss elimination
    for (int i = 0; i < degree - 1; i++) {
        for (int k = i + 1; k < degree; k++) {
            double t = normMtrx[k][i] / normMtrx[i][i];
            for (int j = 0; j <= degree; j++) {
                //make the elements below the pivot elements equal to zero or elimnate the variables
                normMtrx[k][j] = normMtrx[k][j] - t * normMtrx[i][j];
            }
        }
    }

    double coeff[degree + 1];
    //back-substitution
    for (int i = degree - 1; i >= 0; i--) {
        //xVec is an array whose values correspond to the values of xVec,yVec,z..
        //make the variable to be calculated equal to the rhs of the last equation
        coeff[i] = normMtrx[i][degree];
        for (int j = 0; j < degree; j++) {
            //then subtract all the lhs values except the coefficient of the variable whose value is being calculated
            if (j != i) {
                coeff[i] = coeff[i] - normMtrx[i][j] * coeff[j];
            }
        }
        //now finally divide the rhs by the coefficient of the variable to be calculated
        coeff[i] = coeff[i] / normMtrx[i][i];
    }

    return vector<double>(coeff, coeff + (sizeof(coeff) / sizeof(*coeff)));
}

#endif
