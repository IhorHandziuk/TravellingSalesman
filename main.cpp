#include "graph.h"

int main()
{
	int verNum, popSize;
	cout << "Enter number of vertices and population size: ";
	cin >> verNum >> popSize;
	Graph myGraph(verNum);
	myGraph.display_start();
	myGraph.make_first_gen(popSize);
	myGraph.display_loop();
}