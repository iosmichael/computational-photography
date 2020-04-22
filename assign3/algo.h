#pragma once

#include <eigen3/Eigen/Dense>

using namespace Eigen;
/**
 * @file algo.h
 * 
 * @brief This file consists of all the algorithm related functions used for
 * high dynamic range burst photography project
 * @author Michael Liu
 * Contact: x4liu@eng.ucsd.edu
 */

/**
 */
float evalPoly(VectorXf coeff, float x);

VectorXf polyDerivative(VectorXf coeff);

/**
 * * Given the function
 * @param n: max degree of coefficients for analysis 
 * @param e1: the first camera exposure
 * @param x1: the first camera response value
 * @param e2: the second camera exposure
 * @param x2: the second camera response value
 * @param coeff: the coefficients returned by the algorithm
 * solve the array of coefficients
 * 
 * include two constraints in solving the matrix:
 * 0 = 1/e1 sum(coeff cam1) - 1/e2 sum(coeff cam2)
 * 0 = 1/e1 (coeff_n cam1_n + sum(coeff_n-1 cam1_n-1) - 1/e2 (coeff_n cam2_n + sum(coeff_n-1 cam2_n-1))
 * simplify:
 * sum_n-1(coeff (1/e1 (cam1-cam1_n) - 1/e2(cam2 - cam2_n))) = cam2_n / e2 - cam1_n / e1
 * 1/e1 (cam1_n-1 - cam1_n) - 1/e2 (cam2_n-1 - cam2_n) coeff = 1/e2 cam2_n - 1/e1 cam1_n
 */
void estimateCameraResponseInv();