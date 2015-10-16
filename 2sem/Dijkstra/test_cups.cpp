//~ #include "Dijkstra.hpp"

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

typedef size_t ui32;

namespace NGraph
{
	using std::vector;
	
	template<typename TDistance>
	class IWeight
	{
	public:
		virtual TDistance operator () (TDistance current) = 0;
		virtual ~IWeight ()
		{
		}
	};
	
	template<typename TDistance>
	struct Edge
	{
		ui32 to;
		IWeight<TDistance> *weight;
		
		Edge(ui32 to, IWeight<TDistance> *weight)
		: to(to), weight(weight)
		{
		}
		
		TDistance getWeight(TDistance current)
		{
			return (*weight)(current);
		}
	}; 
	
	template<typename TDistance>
	class IGraph
	{
	public:
		virtual vector<Edge<TDistance> > getEdges(ui32 vertex) = 0;
		virtual ui32 numberVertex() = 0;
		virtual TDistance infinity() = 0;
		virtual TDistance zero() = 0;
		virtual ~IGraph ()
		{
		}
	};
};

namespace NDijkstra
{
	using std::vector;
	using std::set;
	using std::pair;
	using std::make_pair;
	
	template<typename TDistance> 
	class Dijkstra
	{
		ui32 start;
		NGraph::IGraph<TDistance> *graph;
		
		vector<int> parents;
		vector<TDistance> distance;
		
		struct Node
		{
			ui32 vertex;
			TDistance distance;
			
			Node (TDistance distance, ui32 vertex)
			: vertex(vertex), distance(distance)
			{
			}
			
			bool operator < (const Node &node) const
			{
				return make_pair(distance, vertex) < make_pair(node.distance, node.vertex);
			}
		};
		
		void doDijkstra()
		{
			distance = vector<TDistance>(graph->numberVertex(), graph->infinity());
			distance[start] = graph->zero();
			
			set<Node> q;
			q.insert(Node(graph->zero(), start));
			
			parents = vector<int>(graph->numberVertex(), -1);
			
			while (!q.empty())
			{
				ui32 x = q.begin()->vertex;
				q.erase(q.begin());
				
				vector<NGraph::Edge<TDistance> > edges = graph->getEdges(x);
				
				for (auto edge: edges)
				{
					if (distance[x] + edge.getWeight(distance[x]) < distance[edge.to])
					{
						parents[edge.to] = x;
						q.erase(Node(distance[edge.to], edge.to));
						distance[edge.to] = distance[x] + edge.getWeight(distance[x]);
						q.insert(Node(distance[edge.to], edge.to));
					}
				}
			}
		}
		
	public:
		Dijkstra(ui32 start, NGraph::IGraph<TDistance> *graph)
		: start(start), graph(graph)
		{
			doDijkstra();
		}
		
		TDistance getDistance(ui32 finish)
		{
			return distance[finish];
		}
		
		vector<TDistance> getAllDistances()
		{
			return distance;
		}
		
		vector<int> getPath(ui32 finish) const
		{
			if (distance[finish] == graph->infinity())
				return vector<int>(1, -1);
			
			ui32 current = finish;
			
			vector<int> path;
			
			while (current != start)
			{
				path.push_back(current);
				current = parents[current];
				
				assert(current >= 0);
			}
			
			path.push_back(start);
			
			std::reverse(path.begin(), path.end());
			return path;
		}
	};
};

struct Distance
{
	long long car_weight;
	int all_time;
	
	Distance (long long weight, long long all_time)
	: car_weight(weight), all_time(all_time)
	{
	}
	
	bool operator < (const Distance &distance) const
	{
		return all_time < distance.all_time;
	}
	
	Distance operator + (const Distance &distance) const
	{
		return Distance(car_weight, all_time + distance.all_time);
	}
};

using std::vector;

class Graph: public NGraph::IGraph<Distance>
{
	vector<vector<NGraph::Edge<Distance> > > g;
	
	class Weight: public NGraph::IWeight<Distance>
	{
		long long max_weight;
		long long edge_time;
	
	public:
		
		Weight()
		{
		}
		
		Weight(long long max_weight, long long edge_time)
		:max_weight(max_weight), edge_time(edge_time)
		{
		}
		
		Distance operator () (Distance current)
		{
			if (current.car_weight > max_weight)
				return Distance(current.car_weight, 2009000999ll);
			return Distance(current.car_weight, edge_time);
		}
	};
	
	long long current_weight;
	
public:
	
	Graph (ui32 number_vertex)
	{
		g.resize(number_vertex);
		current_weight = 3000000;
	}
	
	void insertEdge(ui32 v, ui32 u, long long w, long long t)
	{
		g[v].push_back(NGraph::Edge<Distance>(u, new Weight(w, t)));
		g[u].push_back(NGraph::Edge<Distance>(v, new Weight(w, t)));
	}
	
	vector<NGraph::Edge<Distance> > getEdges(ui32 vertex)
	{
		return g[vertex];
	}
	
	ui32 numberVertex()
	{
		return g.size();
	}
	
	Distance infinity()
	{
		return Distance(current_weight, 2009000999ll);
	}
	
	Distance zero()
	{
		return Distance(current_weight, 0);
	}
	
	void changeCurrentWeight(long long weight)
	{
		current_weight = weight;
	}
	
	~Graph()
	{
		for (auto v: g)
			for (auto edge: v)
				delete edge.weight;
	}
};

bool good(Graph * g, long long count)
{
	if (!count)
		return 1;
	
	long long weight = 3000000 + count * 100;
	
	g->changeCurrentWeight(weight);
	
	NDijkstra::Dijkstra<Distance> d_object(0, g);
	
	return d_object.getDistance(g->numberVertex() - 1).all_time <= 24 * 60;
}

long long binarySearch(Graph * g)
{
	long long left = 0, right = 10000001;
	
	while (right - left > 1)
	{
		int middle = (left + right) / 2;
		
		if (good(g, middle))
			left = middle;
		else
			right = middle;
	}
	
	return left;
}

int main()
{
	int n, m;
	std::cin >> n >> m;
	
	Graph g(n);
	
	for (int i = 0; i < m; ++i)
	{
		int v, u, t, w;
		scanf("%d%d%d%d", &v, &u, &t, &w);
		
		v--;
		u--;
		
		g.insertEdge(v, u, w, t);
	}
	
	std::cout << binarySearch(&g) << '\n';
}
