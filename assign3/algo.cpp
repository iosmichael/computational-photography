#include "algo.h"
#include <iostream>
#include <cmath>

// the coefficients are ordered in the ascending order
float evalPoly(VectorXf coeff, float x){
    VectorXf v(coeff.rows());
    for (size_t i = 0; i < coeff.rows(); i++) {
        v(i) = pow(x, i);
    }
    std::cout << "the coefficients are: " << coeff << std::endl;
    std::cout << "the constructed vector is: " << v << std::endl;
    std::cout << "the result is: " << coeff.transpose() * v << std::endl;
    float y = (coeff.transpose() * v)(0,0);
    return y;
}

// the derivatives of the polynomial coefficients
VectorXf polyDerivative(VectorXf coeff){
    VectorXf derivatives(coeff.rows()-1);
    for (size_t i = 0; i < coeff.rows()-1; i++) {
        derivatives(i) = coeff(i+1) * (i+1);        
    }
    return derivatives;
}

void estimateCameraResponseInv(){
    // float x1_N = pow(x1, n), x2_N = pow(x2, n);
    // float y = 1/e1 * x1_N + 1/e2 * x2_N;
    // Eigen::MatrixXf v1(1, n-1), v2(1, n-1);
    // for (size_t i = 1; i < n; i++) {
    //     v1(0, i-1) = pow(x1, i) - x1_N;
    //     v2(0, i-1) = pow(x2, i) - x2_N;
    // }
    // Eigen::MatrixXf A = v1 / e1 - v2 / e2;
    // Eigen::MatrixXf Ay = A * y;
    // A = A * A.transpose();
    // Eigen::MatrixXf coeffs = A.inverse() * Ay;
    // std::cout << "coefficients are: " << std::endl;
    // std::cout << coeffs << std::endl;
    // for (size_t i = 0; i < n; i++) {
    //     coeff[i] = coeffs(0, i);
    // }
}