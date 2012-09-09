#include "fibheap.h"
#include <iostream>
#include <vector>
#include <limits>
using namespace std;

struct Edge {
	int dest;
	int weight;

	Edge() {}
	Edge(int dest, int weight) : dest(dest), weight(weight) {}
};

struct VertextDistance {
	int vertex;
	int distance;

	VertextDistance(int vertex, int distance)
		: vertex(vertex), distance(distance) {
	}

	bool operator<(const VertextDistance& other) const {
		return this->distance < other.distance;
	}
};

template <>
class NegativeInfinity<VertextDistance> {
public:
	static VertextDistance get() {
		return VertextDistance(0, std::numeric_limits<int>::min());
	}
};

void backtrack(int currentVertex, int currentDistance, int targetVertex, int& currentMinDistance,
		 vector<bool>& visited, vector<vector<Edge> >& neighbors) {
	 visited[currentVertex] = true;
	 if (currentVertex == targetVertex) {
		 if (currentDistance < currentMinDistance) {
			 currentMinDistance = currentDistance;
		 }
	 } else {
		 for (size_t i = 0; i < neighbors[currentVertex].size(); i++) {
			 if (!visited[neighbors[currentVertex][i].dest]) {
				 backtrack(neighbors[currentVertex][i].dest, currentDistance + neighbors[currentVertex][i].weight,
					 targetVertex, currentMinDistance, visited, neighbors);
			 }
		 }
	 }
	 visited[currentVertex] = false;
}

int main() {
	int numberOfVertices, numberOfEdges, target;

	cout << "Enter number of vertices: ";
	cin >> numberOfVertices;
	cout << "Enter number of edges: ";
	cin >> numberOfEdges;
	cout << "Enter target: ";
	cin >> target;

	vector<vector<Edge> > neighbors(numberOfVertices);

	cout << "Enter edges:" << endl;
	for (int i = 0; i < numberOfEdges; i++) {
		int source, dest, weight;

		cin >> source >> dest >> weight;

		neighbors[source - 1].push_back(Edge(dest - 1, weight));
		neighbors[dest - 1].push_back(Edge(source - 1, weight));
	}

	vector<int> distance(numberOfVertices, -1);
	vector<int> previous(numberOfVertices, -1);
	FibonacciHeap<VertextDistance> q;

	distance[0] = 0;
	q.insert(VertextDistance(0, 0));

	while (!q.isEmpty()) {
		int next = q.getMin().vertex;
		q.extractMin();

		for (size_t i = 0; i < neighbors[next].size(); i++) {
			int alt = distance[next] + neighbors[next][i].weight;

			int currentNeighbor = neighbors[next][i].dest;
			if (distance[currentNeighbor] == -1 || alt < distance[currentNeighbor]) {
				distance[currentNeighbor] = alt;
				previous[currentNeighbor] = next;
				q.insert(VertextDistance(currentNeighbor, alt));
			}
		}
	}

	cout << "Distances: ";
	for (int i = 0; i < numberOfVertices; i++) {
		cout << distance[i] << " ";
	}
	cout << endl;

	cout << "Parents: ";
	for (int i = 0; i < numberOfVertices; i++) {
		cout << previous[i] + 1 << " ";
	}
	cout << endl;

	cout << "Dijkstra distance: " << distance[target - 1] << endl;
	
  // Comparing against simple backtracking implementation  
	int backtrackResult;
	vector<bool> visited(numberOfVertices, false);
	backtrack(0, 0, target - 1, backtrackResult, visited, neighbors);

	cout << "Backtracking distance: " << dfsResult << endl;

	return 0;
}
