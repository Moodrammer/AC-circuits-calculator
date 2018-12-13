//to Do:checkout for short Circuit components to remove from the list e.g. (1,1) nodes

#include "pch.h"
#include <iostream>
#include <Eigen/Dense>
using Eigen::MatrixXd;
using namespace std;

int main()
{
	char typeArr[3] = { 'V' , 'R' , 'R'};
	float firstNodeArr[3] = {1 , 1 , 2 };
	float secondNodeArr[3] = {0 , 2 , 0};
	float valueArr[3]{ 5 , 1, 6 };
	
	//inside while loop that reads the file make, after you read a node see if it's bigger
	//than the max which is the maximum number of nodes
	//at first,make max =0
	//number of nodes n = max + 1;
	
	//in volatage case, increment m counter (the number of volatage sources);


	int n = 3 , m = 1;
	MatrixXd A(n , n); 
	//(row i ,column j)
	for (int i = 0; i < n; i++) {
		
		A(i, i) = 0;

		for (int k = 0; k < n; k++) {
			if (typeArr[k] == 'R') {
				if (firstNodeArr[k] == i || secondNodeArr[k] == i) {
					A(i, i) += 1 / valueArr[k];
				}
			}
		}

		for (int j = i + 1; j < n;j++) {
			A(i, j) = 0;
			A(j, i) = 0;	//assigning 0 to an coeffcient which has a garbage does not change the value(garbage).
			for (int k = 0; k < n; k++) {
				if (typeArr[k] == 'R') {
				if ((firstNodeArr[k] == i && secondNodeArr[k] == j) || (firstNodeArr[k] == j && secondNodeArr[k] == i)) {
					A(i, j) -= 1 / valueArr[k];
					A(j, i) = A(i, j);
				}
				}
			}
			}
	}
	cout << A << endl;
}

