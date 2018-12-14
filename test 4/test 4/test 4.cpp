#include "pch.h"
#include <iostream>
#include <Eigen/Dense>
using Eigen::MatrixXd;
using namespace std;

int main()
{
	char typeArr[3] = { 'V' , 'R' , 'R' };
	float firstNodeArr[3] = { 1 , 1 , 2 };
	float secondNodeArr[3] = { 0 , 2 , 0 };
	float valueArr[3]{ 5 , 1, 6 };

	//z matrix (n + m) x 1
	int n = 2, m = 1;
	int numOfComponents = 3;
	MatrixXd z = MatrixXd::Zero(n + m , 1);
	
	int lastIndex = 0;
	int rowCounter = n ;
	for (int k = lastIndex; k < numOfComponents; k++) { //reads the next voltage source
		if (typeArr[k] == 'V') {
			lastIndex = k + 1;
			z(rowCounter, 0) = valueArr[k];
			rowCounter++;
			break;
		}
	}
	cout << z << endl;
	return 0;
}


