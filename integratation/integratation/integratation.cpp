#include "pch.h"
#include <iostream>
#include <Eigen/Dense>
#include <complex>
using Eigen::MatrixXcf;
using namespace std;

int main()
{
	//integeration matrix :
	cout <<"G(NxN) C(N x M)"<<endl; 
	cout << "C(MxN) D(NxN)"<<endl;
	
	float W = 4; //omega
	const int numOfComponents = 5;
	char typeArr[numOfComponents] = { 'V' ,'R', 'C','R','L' };
	float firstNodeArr[numOfComponents] = { 1 , 1 , 2 , 1 ,3 };
	float secondNodeArr[numOfComponents] = { 0 , 2 , 0 , 3 ,0 };
	float valueArr[numOfComponents]{ 50 ,4 , 1 / 12.0 , 8 ,3 };
	//N x N
	int n = 3, m = 1;  ///we don't count node 0
	//float W = 4000; //omega
	//const int numOfComponents = 4;
	//char typeArr[numOfComponents] = { 'V' ,'C', 'R' ,'L'};
	//float firstNodeArr[numOfComponents] = { 1 ,3,2 ,3};
	//float secondNodeArr[numOfComponents] = { 0 ,2,1 ,0};
	//float valueArr[numOfComponents]{ 10 ,0.1 * pow(10,-6), 5 * pow (10,3) ,1};
	////N x N
	//int n = 3, m = 1;
	MatrixXcf G(n, n);
	//(row i ,column j)
	for (int i = 0; i < n; i++) { //n = index + 1

		G(i, i) = 0;

		for (int k = 0; k < numOfComponents; k++) { //search in the data
			if (typeArr[k] == 'R') {
				if (firstNodeArr[k] == i + 1 || secondNodeArr[k] == i + 1) { //i+1 because we don't consider 0 node
					G(i, i) += 1 / valueArr[k];  //diagonal
				}
			}
			else if (typeArr[k] == 'C') {
				if (firstNodeArr[k] == i + 1 || secondNodeArr[k] == i + 1) { //i+1 because we don't consider 0 node
					G(i, i) += complex<float>(0 , W * valueArr[k]);  //diagonal
				}
			}
			else if (typeArr[k] == 'L') {
				if (firstNodeArr[k] == i + 1 || secondNodeArr[k] == i + 1) { //i+1 because we don't consider 0 node
					G(i, i) += complex<float>(0, -1 / (W * valueArr[k]));  //diagonal
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
				else if (typeArr[k] == 'C') {
					if ((firstNodeArr[k] == i + 1 && secondNodeArr[k] == j + 1) || (firstNodeArr[k] == j + 1 && secondNodeArr[k] == i + 1)) {
						G(i, j) -= complex<float>(0, W * valueArr[k]);
						G(j, i) = G(i, j);
					}
				}
				else if (typeArr[k] == 'L') {
					if ((firstNodeArr[k] == i + 1 && secondNodeArr[k] == j + 1) || (firstNodeArr[k] == j + 1 && secondNodeArr[k] == i + 1)) {
						G(i, i) += complex<float>(0, -1 / (W * valueArr[k]));  //diagonal
						G(j, i) = G(i, j);
					}
				}
			}
		}
	}
	cout << "G "<<G << endl;

	//N x M , M x N
	MatrixXcf B = MatrixXcf::Zero(n, m); //initialize all coefficient to 0
	MatrixXcf C = MatrixXcf::Zero(m, n);

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
	cout << "B "<< B << endl;
	cout <<"C "<< C << endl;

	//mxm	
	MatrixXcf D = MatrixXcf::Zero(m, m);
	
	//Block Operation	//integration matrix (N+M) x (N+M)
	MatrixXcf A(m+n, m+n);
	A.block(0, 0, n, n) = G;
	A.block(0, n, n, m) = B;
	A.block(n, 0, m, n) = C;
	A.block(n, n, m, m) = D;
	cout << "A " <<A << endl;

	//z matrix (n + m) x 1
	MatrixXcf z = MatrixXcf::Zero(n + m, 1);

	int rowCounter = n;
	for (int k = 0; k < numOfComponents; k++) { //reads the next voltage source
		if (typeArr[k] == 'V') {
			z(rowCounter, 0) = valueArr[k];
			rowCounter++;
		}
	}
	cout << "z "<<z << endl;
	
	//calculate the unknown matrix
	MatrixXcf x = MatrixXcf::Zero(n + m, 1);
	x = A.inverse() * z;
	cout << "x" << x << endl;

	cout << "\n<----------------------------------------OUTPUT--------------------------------------------->\n";
	//Displaying all node voltages
	for (int i = 0; i < n; i++)
	{
		cout << "V(" << i + 1 << ") " << x(i, 0) << endl;
	}

	//Displaying currents in all branches
	int VoltagesourceCounter = 0;
	for (int i = 0; i < numOfComponents; i++)
	{
		if (typeArr[i] != 'V' && typeArr[i] != 'I')
		{
			if (firstNodeArr[i] != 0 && secondNodeArr[i] != 0)
			{
				cout << "I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") ";
				complex<float> value(0,0);
				if (typeArr[i] == 'R')
					value = valueArr[i];		//real = valueArr[i] and imag  = 0
				else if(typeArr[i] == 'C')
					value = complex<float> (0,-1/(W*valueArr[i]));
				else if (typeArr[i] == 'L')
					value = complex<float>(0, W * valueArr[i]);

				cout << ((x(firstNodeArr[i] - 1, 0) - x(secondNodeArr[i] - 1, 0)) / value) <<endl;
			}
			else
			{
				if (firstNodeArr[i] == 0)
				{
					cout << "I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") ";
					complex<float> value(0, 0);
					if (typeArr[i] == 'R')
						value= valueArr[i];
					else if (typeArr[i] == 'C')
						value = complex<float>(0, -1 / (W * valueArr[i]));
					else if (typeArr[i] == 'L')
						value = complex<float>(0, W * valueArr[i]);

					cout <<	((complex<float>(0,0) - x(secondNodeArr[i] - 1, 0)) / value) << endl;
				}
				else
				{
					cout << "I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") ";
					complex<float> value(0, 0);
					if (typeArr[i] == 'R')
						value = valueArr[i];
					else if (typeArr[i] == 'C')
						value = complex<float>(0, -1 / (W * valueArr[i]));
					else if (typeArr[i] == 'L')
						value = complex<float>(0, W * valueArr[i]);

					cout << ((x(firstNodeArr[i] - 1, 0) - complex<float>(0, 0))) / value << endl;
				}
			}
		}
		else
		{
			cout << "I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") " << x(n + VoltagesourceCounter, 0) << endl;
			VoltagesourceCounter++;
		}
	}

	cout << "\n<------------------------------------------------------------------------------------------->\n";

	return 0;
}

