// m x n and n x m matrixes

#include "pch.h"
#include <iostream>
#include <Eigen/Dense>
using Eigen::MatrixXd;
using namespace std;

int main()
{
	char typeArr[3] = { 'V' , 'V' , 'R' };
	float firstNodeArr[3] = { 1 , 1 , 2 };
	float secondNodeArr[3] = { 0 , 2 , 0 };
	float valueArr[3]{ 5 , 1, 6 };

	int n = 3, m = 2;
	MatrixXd A = MatrixXd::Zero(n, m); //initialize all coefficient to 0
	MatrixXd B = MatrixXd::Zero(m, n);
	
	int lastIndex =0;
	for (int i = 0; i < m; i++) {//coulmns //FIFO (first in first out) method in dealing with Vs
		for (int k = lastIndex; k < n; k++) { //reads the next voltage source
			if (typeArr[k] == 'V') {
				lastIndex = k + 1;
				if(firstNodeArr[k] != 0){
					A(firstNodeArr[k], i) = 1;
					B(i, firstNodeArr[k]) = 1;
				}
				if (secondNodeArr[k] != 0) {
					A(secondNodeArr[k], i) = -1;
					B(i, secondNodeArr[k]) = -1;
				}
				break;
			}
		}
	}
	cout << A << endl;
	cout << B << endl;
}

