#include "pch.h"
#include <iostream>
#include <Eigen/Dense>
using Eigen::MatrixXd;
using namespace std;

int main()
{
	/*integeration matrix :
	G(NxN) C(N x M) 
	C(MxN) D(NxN)
	*/
	char typeArr[4] = { 'V' , 'V' , 'R' , 'R' };
	float firstNodeArr[4] = { 1 ,1, 1 , 2 };
	float secondNodeArr[4] = { 0 ,2, 2 , 0 };
	float valueArr[4]{ 5 , 6 ,1, 6 };
	
	//N x N
	int n = 2, m = 2;
	int numOfComponents = 4;
	MatrixXd G(n, n);
	//(row i ,column j)
	for (int i = 0; i < n; i++) { //n = index + 1

		G(i, i) = 0;

		for (int k = 0; k < numOfComponents; k++) { //search in the data
			if (typeArr[k] == 'R') {
				if (firstNodeArr[k] == i + 1 || secondNodeArr[k] == i + 1) { //i+1 because we don't consider 0 node
					G(i, i) += 1 / valueArr[k];  //diagonal
				}
			}
		}

		for (int j = i + 1; j < n; j++) {
			G(i, j) = 0;
			G(j, i) = 0;	//assigning 0 to an coeffcient which has a garbage does not change the value(garbage).
			for (int k = 0; k < numOfComponents; k++) {
				if (typeArr[k] == 'R') {
					if ((firstNodeArr[k] == i + 1 && secondNodeArr[k] == j + 1) || (firstNodeArr[k] == j + 1 && secondNodeArr[k] == i + 1)) {
						G(i, j) -= 1 / valueArr[k];
						G(j, i) = G(i, j);
					}
				}
			}
		}
	}
	//cout << G << endl;

	//N x M , M x N
	MatrixXd B = MatrixXd::Zero(n, m); //initialize all coefficient to 0
	MatrixXd C = MatrixXd::Zero(m, n);

	int lastIndex = 0;

	for (int i = 0; i < m; i++) {//coulmns //FIFO (first in first out) method in dealing with Vs
		for (int k = lastIndex; k < numOfComponents; k++) { //reads the next voltage source
			if (typeArr[k] == 'V') {
				lastIndex = k + 1;
				if (firstNodeArr[k] != 0) {
					B(firstNodeArr[k] - 1, i) = 1;
					C(i, firstNodeArr[k] - 1) = 1;		//-1 because it's index now
				}
				if (secondNodeArr[k] != 0) {
					B(secondNodeArr[k] - 1, i) = -1;
					C(i, secondNodeArr[k] - 1) = -1;
				}
				break;
			}
		}
	}
	//cout << B << endl;
	//cout << C << endl;

	//mxm	
	MatrixXd D = MatrixXd::Zero(m, m);
	
	//Block Operation	//integration matrix (N+M) x (N+M)
	MatrixXd A(m+n, m+n);
	A.block(0, 0, n, n) = G;
	A.block(0, n, n, m) = B;
	A.block(n, 0, m, n) = C;
	A.block(n, n, m, m) = D;
	cout << A << endl;

	//z matrix (n + m) x 1
	MatrixXd z = MatrixXd::Zero(n + m, 1);

	int rowCounter = n;
	for (int k = 0; k < numOfComponents; k++) { //reads the next voltage source
		if (typeArr[k] == 'V') {
			z(rowCounter, 0) = valueArr[k];
			rowCounter++;
		}
	}
	cout << z << endl;
}

