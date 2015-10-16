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

using std::vector;

class Graph: public NGraph::IGraph<long long>
{
	vector<vector<NGraph::Edge<long long> > > g;
	
	class Weight: public NGraph::IWeight<long long>
	{
		long long const_weight;
	
	public:
		
		Weight()
		{
		}
		
		Weight(long long const_weight)
		:const_weight(const_weight)
		{
		}
		
		long long operator () (long long current)
		{
			return const_weight;
		}
	};
	
public:
	
	Graph (ui32 number_verticles)
	{
		g.resize(number_verticles);
	}
	
	void insertEdge(ui32 v, ui32 u, long long w)
	{
		
		g[v].push_back(NGraph::Edge<long long>(u, new Weight(w)));
		g[u].push_back(NGraph::Edge<long long>(v, new Weight(w)));
	}
	
	vector<NGraph::Edge<long long> > getEdges(ui32 vertex)
	{
		return g[vertex];
	}
	
	ui32 numberVertex()
	{
		return g.size();
	}
	
	long long infinity()
	{
		return 2009000999ll;
	}
	
	long long zero()
	{
		return 0ll;
	}
	
	~Graph()
	{
		for (auto v: g)
			for (auto edge: v)
				delete edge.weight;
	}
};

int main()
{
	ui32 t;
	std::cin >> t;
	
	for (ui32 z = 0; z < t; ++z)
	{
		ui32 n, m;
		std::cin >> n >> m;
		
		Graph g(n);
		
		for (ui32 i = 0; i < m; ++i)
		{
			int v, u, w;
			scanf("%d%d%d", &v, &u, &w);
			g.insertEdge(v, u, w);
		}
		
		ui32 v;
		std::cin >> v;
		
		NDijkstra::Dijkstra<long long> d_object(v, &g);
		
		vector<long long> d = d_object.getAllDistances();
		
		for (auto x: d)
			std::cout << x << ' ';
		std::cout << '\n';
	}
}
