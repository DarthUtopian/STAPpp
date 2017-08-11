/***************************************************************/
/*  FEM++ : A C++ finite element method code for teaching      */
/*     Computational Dynamics Laboratory                       */
/*     School of Aerospace Engineering, Tsinghua University    */
/*                                                             */
/*     http://www.comdyn.cn/                                   */
/***************************************************************/

#include <algorithm>

#include "Element.h"

//  Calculate the column height, used with the skyline storage scheme
void Element::CalculateColumnHeight(unsigned int* ColumnHeight)
{
//	Obtain the location matrix: the global equation number that corresponding to each DOF of the element
//	Caution:  Equation number is numbered from 1 !
	vector<int> LocationMatrix;

	for (int N = 0; N < NEN; N++)
		for (int D = 0; D < Node::NDF; D++)
			if (nodes[N]->bcode[D])
				LocationMatrix.push_back(nodes[N]->bcode[D]);

#ifdef _DEBUG_
//	List location matrix
	for (int i = 0; i < LocationMatrix.size(); i++)
		cout <<	LocationMatrix[i] << " ";
	cout << endl;
#endif

	int nfirstrow = *std::min_element(LocationMatrix.begin(), LocationMatrix.end());

//	Calculate the column height contributed by this element
	for (int i = 0; i < LocationMatrix.size(); i++)
	{
		int column = LocationMatrix[i];
		if (!column)
			continue;

		int Height = column - nfirstrow;
		if (ColumnHeight[column-1] < Height) ColumnHeight[column-1] = Height;
	}
}

//	Assemble the banded global stiffness matrix (skyline storage scheme)
void Element::assembly(double* Matrix, double* StiffnessMatrix, unsigned int* DiagonalAddress)
{
//	Calculate element stiffness matrix
	ElementStiffness(Matrix);

//	Obtain the location matrix
	vector<int> LocationMatrix;
	for (int i=0; i<NEN; i++)
		for (int j=0; j<Node::NDF; j++)
			LocationMatrix.push_back(nodes[i]->bcode[j]);
	
//	Assemble global stiffness matrix
	for (int j = 0; j < LocationMatrix.size(); j++)
	{
		int Lj = LocationMatrix[j];	// Global equation number corresponding to jth DOF of the element
		if (!Lj) 
			continue;

//		Address of diagonal element of column j in the one dimensional element stiffness matrix
		int DiagjElement = (j+1)*j/2 + 1;

//		Address of diagonal element of column j in the banded global stiffness matrix
		int DiagjGlobal = DiagonalAddress[Lj - 1];

		for (int i = 0; i <= j; i++)
		{
			int Li = LocationMatrix[i];	// Global equation number corresponding to ith DOF of the element
			if (!Li) 
				continue;

			StiffnessMatrix[DiagjGlobal + Lj - Li - 1] += Matrix[DiagjElement + j - i - 1];
		}
	}

	return;
}