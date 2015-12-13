#ifndef _GRAPH
#define _GRAPH

#include <iostream>
#include <climits>
#include <vector>
#include <queue>
#include <algorithm>
#include <cassert>

typedef unsigned int ui32;
typedef unsigned long long ui64;

namespace NGraph
{
    using std::vector;
    
    template<class TEdge>
    class IGraph
    {
    public:
        virtual const vector<TEdge*>& getEdges(ui32 vertex) const = 0;
        virtual ui32 numberEdges(ui32 vertex) const = 0;
        virtual void insertEdge(ui32 from, TEdge &edge) = 0;
        virtual size_t size() const = 0;
        virtual ~IGraph()
        {
        }
    };
    
    class Edge
    {
    protected:
        ui32 vertex;
    public:
        Edge()
        {
        }
        
        Edge(ui32 vertex)
        : vertex(vertex)
        {
        }
        
        ui32 to() const
        {
            return vertex;
        }
        
        virtual ~Edge()
        {
        }
    };
    
    template<class TEdge>
    class Graph: public IGraph<TEdge>
    {
    protected:
        vector<vector<TEdge*> > graph;
        
        Graph()
        {
        }
        
    public:
        
        Graph(ui32 graph_size)
        {
            graph.resize(graph_size);
        }
        
        const vector<TEdge*>& getEdges(ui32 vertex) const
        {
            return graph[vertex];
        }
        
        ui32 numberEdges(ui32 vertex) const
        {
            return graph[vertex].size();
        }
        
        virtual void insertEdge(ui32 from, TEdge &edge)
        {
            graph[from].push_back(new TEdge(edge));
        }
        
        size_t size() const
        {
            return graph.size();
        }
        
        ~Graph()
        {
        }
    };
}
#endif
