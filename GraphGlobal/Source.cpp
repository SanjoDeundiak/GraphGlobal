#include "Graph.h"
#include <iostream>
#include <sstream>
#include <fstream>

int main()
{
	std::ifstream inFile("debug.txt");
	size_t n;
	inFile >> n;	
	inFile.close();
	for (size_t i = 0; i < n; i++)
	{
		std::stringstream inFileName, outFileName;
		inFileName << "input" << i << ".txt";		
		outFileName << "output" << i << ".txt";		
		inFile.open(inFileName.str(), std::ios_base::in);
		std::ofstream outFile(outFileName.str());
		size_t k;
		inFile >> k;
		std::vector<double> X(k), Y(k);
		for (size_t j = 0; j < k; j++)
		{
			inFile >> X[j] >> Y[j];
			outFile << "( " << X[j] << " , " << Y[j] << " );\n";
		}
		inFile.close();		
		outFile << "\n\nANSWER:\n";
		Graph g(X, Y);
		std::vector<Figure> result = g.process();
		for (size_t j = 0; j < result.size(); j++)
			outFile << result[j] << std::endl << std::endl;
	}

	std::cin.get();	
}