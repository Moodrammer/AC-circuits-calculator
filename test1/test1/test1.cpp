// test1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <istream>
using namespace std;


int main()
{
	ifstream myFile("text.txt"); //opens the file
	
	char type; int misco; int firstNode, secondNode, value;
	char typeArr[100]; int firstNodeArr[100], secondNodeArr[100], valueArr[100];
	
int i = 0; //counter
if (myFile.is_open())
	{
		while (myFile >> type)			//reads from the file.
		{
			typeArr[i] = type;

			myFile >> misco;

			myFile >> firstNode; firstNodeArr[i] = firstNode;

			myFile >> secondNode; secondNodeArr[i] = secondNode;

			myFile >> value; valueArr[i] = value;

			i++;
		}
	myFile.close();
}
	else
	{
	cout<< "Unable to open file";
	}

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
