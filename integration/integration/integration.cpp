#include "pch.h"
#include <iostream>
#include <Eigen/Dense>
#include <complex>
#include <fstream>
#include <istream>
#include <string>
using Eigen::MatrixXcf;
using namespace std;
# define pi           3.14159265358979323846

int main()
{
	
	int action;
	do {
		do {
			cout << "[1] solve an AC circuit" << endl;
			cout << "[2] exit" << endl;
			cout << "please select a valid operation..." << endl;
			cin >> action;
		} while (!(action == 1 || action == 2));

		if (action == 1) {


	cout << "please enter the name of your text file . . .";
	string inputString;
	cin >> inputString;
	ifstream myFile(inputString + ".txt"); //opens the file

	char type; int ID; float firstNode, secondNode, phaseArrElement;
	string value;
	char typeArr[100]; int IDArr[100]; float firstNodeArr[100], secondNodeArr[100], valueArr[100], PhaseArr[100];
	int phaseArrCount = 0;

	int numOfComponents = 0;
	int i = 0; //counter
	if (myFile.is_open())
	{
		while (myFile >> type)			//reads from the file.
		{
			numOfComponents++;

			typeArr[i] = type;
			myFile >> ID; IDArr[i] = ID;

			myFile >> firstNode; firstNodeArr[i] = firstNode;

			myFile >> secondNode; secondNodeArr[i] = secondNode;

			string::size_type sz;     // alias of size_t
			myFile >> value; valueArr[i] = stof(value, &sz);

			if (type == 'V' || type == 'I') {
				myFile >> phaseArrElement; PhaseArr[phaseArrCount] = phaseArrElement;
				phaseArrCount++;
			}

			i++;
		}

		myFile.close();
	

	//integeration matrix :
	float W;
	do {
		cout << " please enter a valid angular frequency..." << endl;
		cin >> W;
	} while (W < 0);
	//omega
	
	int n = 1; //the minimum number of essential nodes
	int m = 0;
	for (int k = 1; k < numOfComponents; k++) {
		if (firstNodeArr[k] > n  ) n = firstNodeArr[k];
		else if (secondNodeArr[k] > n) n = secondNodeArr[k];

		if (typeArr[k] == 'V')m++;
	}


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
			else if (typeArr[k] == 'L') {
				if (firstNodeArr[k] == i + 1 || secondNodeArr[k] == i + 1) { //i+1 because we don't consider 0 node
					G(i, i) += complex<float>(0, -1 / (W * valueArr[k]));  //diagonal
				}
			}
			else if (typeArr[k] == 'C') {
				if (firstNodeArr[k] == i + 1 || secondNodeArr[k] == i + 1) { //i+1 because we don't consider 0 node
					G(i, i) += complex<float>(0, (W * valueArr[k]));  //diagonal
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
				else if (typeArr[k] == 'L') {
					if ((firstNodeArr[k] == i + 1 && secondNodeArr[k] == j + 1) || (firstNodeArr[k] == j + 1 && secondNodeArr[k] == i + 1)) {
						G(i, j) -= complex<float>(0, -1 / (W * valueArr[k]));
						G(j, i) = G(i, j);
					}
				}
				else if (typeArr[k] == 'C') {
					if ((firstNodeArr[k] == i + 1 && secondNodeArr[k] == j + 1) || (firstNodeArr[k] == j + 1 && secondNodeArr[k] == i + 1)) {
						G(i, j) -= complex<float>(0, (W * valueArr[k]));
						G(j, i) = G(i, j);
					}
				}
			}
		}
	}

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

	//mxm	
	MatrixXcf D = MatrixXcf::Zero(m, m);

	//Block Operation	//integration matrix (N+M) x (N+M)
	MatrixXcf A(m + n, m + n);
	A.block(0, 0, n, n) = G;
	A.block(0, n, n, m) = B;
	A.block(n, 0, m, n) = C;
	A.block(n, n, m, m) = D;

	//z matrix (n + m) x 1
	MatrixXcf z = MatrixXcf::Zero(n + m, 1);

	int rowVoltageCounter = n;
	int phaseCounter = 0;
	for (int k = 0; k < numOfComponents; k++) { //reads the next voltage source
		if (typeArr[k] == 'V') {
			z(rowVoltageCounter, 0) = complex<float>(valueArr[k] * cos((PhaseArr[phaseCounter] * pi) / 180), valueArr[k] * sinf((PhaseArr[phaseCounter] * pi) / 180));
			rowVoltageCounter++;
			phaseCounter++;
		}
		else if (typeArr[k] == 'I') {
			if (firstNodeArr[k] != 0)
				z(firstNodeArr[k] - 1, 0) += complex<float>(valueArr[k] * cos((PhaseArr[phaseCounter] / 180) * pi), valueArr[k] * sinf((PhaseArr[phaseCounter] * pi) / 180));
			if (secondNodeArr[k] != 0)
				z(secondNodeArr[k] - 1, 0) += complex<float>(valueArr[k] * cos((PhaseArr[phaseCounter] * pi) / 180), valueArr[k] * sinf((PhaseArr[phaseCounter] * pi) / 180));

			phaseCounter++;
		}
	}

	//calculate the unknown matrix
	MatrixXcf x = MatrixXcf::Zero(n + m, 1);
	x = A.inverse() * z;

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
				complex<float> value(0, 0);
				if (typeArr[i] == 'R')
				{
					value = valueArr[i];		//real = valueArr[i] and imag  = 0
					cout << 'R' << IDArr[i];
				}
				else if (typeArr[i] == 'C')
				{
					value = complex<float>(0, -1 / (W*valueArr[i]));
					cout << 'C' << IDArr[i];
				}
				else if (typeArr[i] == 'L') {
					value = complex<float>(0, W * valueArr[i]);
					cout << 'L' << IDArr[i];
				}
				cout << " I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") " << ((x(firstNodeArr[i] - 1, 0) - x(secondNodeArr[i] - 1, 0)) / value) << endl;

			}
			else
			{
				if (firstNodeArr[i] == 0)
				{
					complex<float> value(0, 0);
					if (typeArr[i] == 'R')
					{
						value = valueArr[i];
						cout << 'R' << IDArr[i];
					}
					else if (typeArr[i] == 'C')
					{
						value = complex<float>(0, -1 / (W * valueArr[i]));
						cout << 'C' << IDArr[i];
					}
					else if (typeArr[i] == 'L') {
						value = complex<float>(0, W * valueArr[i]);
						cout << 'L' << IDArr[i];
					}
					cout << " I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") " << ((complex<float>(0, 0) - x(secondNodeArr[i] - 1, 0)) / value) << endl;

				}
				else
				{
					complex<float> value(0, 0);
					if (typeArr[i] == 'R') {
						value = valueArr[i];
						cout << 'R' << IDArr[i];
					}
					else if (typeArr[i] == 'C') {
						value = complex<float>(0, -1 / (W * valueArr[i]));
						cout << 'C' << IDArr[i];
					}
					else if (typeArr[i] == 'L') {
						value = complex<float>(0, W * valueArr[i]);
						cout << 'L' << IDArr[i];
					}
					cout << " I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") " << ((x(firstNodeArr[i] - 1, 0) - complex<float>(0, 0))) / value << endl;

				}
			}
		}
		else if (typeArr[i] == 'V') {
			cout << IDArr[i] << " I(" << firstNodeArr[i] << "," << secondNodeArr[i] << ") " << x(n + VoltagesourceCounter, 0) << endl;
			VoltagesourceCounter++;
		}
	}

	cout << "\n<------------------------------------------------------------------------------------------->\n";
	}
	else
	{
	cout << "Unable to open file" <<endl;
	}

	//int x1;
	//cout <<  "please press any key to continue" << endl;
	//cin >> x1;
			}

	} while (action != 2);
	return 0;
}

