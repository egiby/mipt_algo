#ifndef _NETWORK
#define _NETWORK

#include "Graph.hpp"

namespace NNetwork
{
    using std::vector;
    
    struct NetworkEdge: public NGraph::Edge
    {
        ui32 capacity;
        int flow;
        NetworkEdge * reverse_edge;
        bool is_real;
        
        NetworkEdge(ui32 vertex, ui32 _capacity, bool real = 1)
        {
            flow = 0;
            to = vertex;
            capacity = _capacity;
            is_real = real;
        }
    };
    
    class Network: public NGraph::Graph<NetworkEdge>
    {
    protected:
        ui32 s, t;
        
        Network()
        {
        }
        
    public:
        
        Network(ui32 n, ui32 s, ui32 t)
        : s(s), t(t)
        {
            graph.resize(n);
        }
        
        void insertEdge(ui32 v, NetworkEdge &edge)
        {
            NetworkEdge *to, *reversed;
            
            to = &edge;
            reversed = new NetworkEdge(v, 0, 0);
            
            to->reverse_edge = reversed;
            reversed->reverse_edge = to;
            reversed->flow = -to->flow;
            
            graph[v].push_back(to);
            graph[edge.to].push_back(reversed);
        }
        
        vector<NetworkEdge*>::iterator getFirstEdge(ui32 v)
        {
            return graph[v].begin();
        }
        
        vector<NetworkEdge*>::iterator getLastEdge(ui32 v)
        {
            return graph[v].end();
        }
        
        long long getFlow() const
        {
            vector<NetworkEdge*> edges = graph[s];
            
            long long flow = 0;
            
            for (ui32 i = 0; i < edges.size(); ++i)
                flow += (long long)std::max(0, edges[i]->flow);
            
            return flow;
        }
        
        ui32 getT() const
        {
            return t;
        }
        
        ui32 getS() const
        {
            return s;
        }
        
        void clear()
        {
            for (ui32 v = 0; v < size(); ++v)
                for (ui32 i = 0; i < graph[v].size(); ++i)
                    if (!graph[v][i]->is_real)
                        delete graph[v][i];
        }
        
        ~Network()
        {
        }
    };
}

class IFlowSearcher
{
public:
    virtual void searchMaxFlow(NNetwork::Network &net) = 0;
};
#endif
