#include <iostream>
#include <climits>
#include <vector>
#include <queue>
#include <algorithm>

typedef unsigned int ui32;

namespace NMaxFlow
{
    using std::vector;
    using std::queue;
    
    class Graph
    {
    public:
        struct Edge
        {
            ui32 to;
            ui32 capacity;
            
            Edge()
            {
            }
            
            Edge(ui32 to, ui32 capacity)
            : to(to), capacity(capacity)
            {
            }
            
            virtual ~Edge()
            {
            }
        };
        
    protected:
        vector<vector<Edge*> > graph;
        
        Graph()
        {
        }
        
    public:
        
        Graph(ui32 graph_size)
        {
            graph.resize(graph_size);
        }
        
        virtual vector<Edge*> getEdges(ui32 vertex) const
        {
            return graph[vertex];
        }
        
        virtual ui32 numberEdges(ui32 vertex) const 
        {
            return graph[vertex].size();
        }
        
        virtual void insertEdge(ui32 from, ui32 to, ui32 capacity)
        {
            graph[from].push_back(new Edge(to, capacity));
        }
        
        virtual size_t size() const
        {
            return graph.size();
        }
        
        virtual void clean()
        {
            for (auto edges: graph)
                for (auto edge: edges)
                    delete edge;
                    
            graph.resize(0);
        }
        
        virtual ~Graph()
        {
        }
    };
    
    class Network: public Graph
    {
        ui32 s, t;
        
        Network()
        {
        }
    public:
        struct NetworkEdge: public Graph::Edge
        {
            int flow;
            NetworkEdge * reverse_edge;
            bool is_real;
            
            NetworkEdge(ui32 vertex, ui32 _capacity, bool real)
            {
                flow = 0;
                to = vertex;
                capacity = _capacity;
                is_real = real;
            }
        };
    
    private:
        vector<ui32> bfs() const
        {
            vector<ui32> distance(graph.size(), UINT_MAX);
            vector<ui32> parents(graph.size(), UINT_MAX);
            queue<ui32> bfs_queue;
            
            distance[s] = 0;
            bfs_queue.push(s);
            
            while (!bfs_queue.empty())
            {
                ui32 vertex = bfs_queue.front();
                bfs_queue.pop();
                
                if (vertex == t)
                    break;
                
                vector<Graph::Edge*> edges = graph[vertex];
                
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (distance[vertex] + 1 < distance[edges[i]->to] && (int)edges[i]->capacity > dynamic_cast<NetworkEdge*>(edges[i])->flow)
                    {
                        distance[edges[i]->to] = distance[vertex] + 1;
                        parents[edges[i]->to] = vertex;
                        bfs_queue.push(edges[i]->to);
                    }
                }
            }
            
            return parents;
        }
        
    public:
        
        Network(ui32 n, ui32 s, ui32 t)
        : s(s), t(t)
        {
            graph.resize(n);
        }
        
        void insertEdge(ui32 v, ui32 u, ui32 capacity)
        {
            NetworkEdge *to, *reversed;
            
            to = new NetworkEdge(u, capacity, 1);
            reversed = new NetworkEdge(v, 0, 0);
            
            to->reverse_edge = reversed;
            reversed->reverse_edge = to;
            
            graph[v].push_back(to);
            graph[u].push_back(reversed);
        }
        
        int pushFlow()
        {
            vector<ui32> parents = bfs();
            
            if (parents[t] == UINT_MAX)
                return 0;
            
            int pushed_flow = INT_MAX;
            
            vector<ui32> path(1, t);
            
            ui32 vertex = t;
            
            while (vertex != s)
            {
                vertex = parents[vertex];
                path.push_back(vertex);
            }
            
            std::reverse(path.begin(), path.end());
            
            for (ui32 i = 0; i + 1 < path.size(); ++i)
            {
                vector<Graph::Edge*> edges = graph[path[i]];
                
                for (auto edge: edges)
                    if (edge->to == path[i + 1] && dynamic_cast<NetworkEdge*>(edge)->is_real)
                        pushed_flow = std::min(pushed_flow, (int)edge->capacity - dynamic_cast<NetworkEdge*>(edge)->flow);
            }
            
            for (ui32 i = 0; i + 1 < path.size(); ++i)
            {
                vector<Graph::Edge*> edges = graph[path[i]];
                
                for (auto edge: edges)
                    if (edge->to == path[i + 1] && dynamic_cast<NetworkEdge*>(edge)->is_real)
                    {
                        dynamic_cast<NetworkEdge*>(edge)->flow += pushed_flow;
                        dynamic_cast<NetworkEdge*>(edge)->reverse_edge->flow -= pushed_flow;
                    }
            }
            
            return pushed_flow;
        }
        
        unsigned long long getFlow() const
        {
            vector<Graph::Edge*> edges = graph[s];
            
            unsigned long long flow = 0;
            
            for (auto edge: edges)
                flow += dynamic_cast<NetworkEdge*>(edge)->flow;
                
            return flow;
        }
        
        ~Network()
        {
        }
    };
    
    void searchMaxFlow(Network &net)
    {
        while (net.pushFlow())
            continue;
    }
}

int main()
{
    int n, m;
    
    std::cin >> n >> m;
    
    NMaxFlow::Network net(n, 0, n - 1);
    
    for (int i = 0; i < m; ++i)
    {
        int v, u, c;
        scanf("%d%d%d", &v, &u, &c);
        net.insertEdge(v - 1, u - 1, c);
    }
    
    NMaxFlow::searchMaxFlow(net);
    std::cout << net.getFlow() << '\n';
    
    net.clean();
    
    return 0;
}

