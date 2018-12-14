#include "pch.h"
#include <iostream>
#include <Eigen/Dense>
using Eigen::MatrixXd;
using namespace std;

int main()
{
	char typeArr[4] = { 'V' , 'V' , 'R' , 'R' };
	float firstNodeArr[4] = { 1 ,1, 1 , 2 };
	float secondNodeArr[4] = { 0 ,2, 2 , 0 };
	float valueArr[4]{ 5 , 6 ,1, 6 };

	//N x N
	int n = 2, m = 2;
	int numOfComponents = 4;
	MatrixXd z = MatrixXd::Zero(n + m, 1);

	int rowCounter = n;
	for (int k = 0; k < numOfComponents; k++) { //reads the next voltage source
		if (typeArr[k] == 'V') {
			z(rowCounter, 0) = valueArr[k];
			rowCounter++;
		}
	}
	cout << z << endl;
	return 0;
}


