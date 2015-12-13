#ifndef _NETWORK
#define _NETWORK

#include "Graph.hpp"

namespace NNetwork
{
    using std::vector;
    
    class NetworkEdge;
    class Network;
    
    class NetworkEdge: public NGraph::Edge
    {
        ui32 capacity;
        NetworkEdge * reverse_edge;
        int flow;
        
    public:
        NetworkEdge(ui32 _vertex, ui32 capacity)
        : capacity(capacity), flow(0)
        {
            vertex = _vertex;
        }
        
        ui32 getCapacity() const
        {
            return capacity;
        }
        
        int getFlow() const
        {
            return flow;
        }
        
        void push(int difference)
        {
            flow += difference;
            reverse_edge->flow -= difference;
        }
        
        NetworkEdge * getReverseEdge() const
        {
            return reverse_edge;
        }
        
        friend Network;
    };
    
    class Network: public NGraph::Graph<NetworkEdge>
    {
    protected:
        ui32 source, target;
        bool is_cleaning_necessary;
        
        Network()
        : is_cleaning_necessary(1)
        {
        }
        
        void clean()
        {
            if (!is_cleaning_necessary)
                return;
            for (ui32 v = 0; v < size(); ++v)
                for (ui32 i = 0; i < graph[v].size(); ++i)
                    if (!graph[v][i]->getCapacity())
                        delete graph[v][i];
        }
    public:
        
        Network(ui32 number, ui32 source, ui32 target)
        : source(source), target(target), is_cleaning_necessary(1)
        {
            graph.resize(number);
        }
        
        void insertEdge(ui32 v, NetworkEdge &edge)
        {
            NetworkEdge *to, *reversed;
            
            to = &edge;
            reversed = new NetworkEdge(v, 0);
            
            to->reverse_edge = reversed;
            reversed->reverse_edge = to;
            reversed->flow = -to->flow;
            
            graph[v].push_back(to);
            graph[edge.to()].push_back(reversed);
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
            vector<NetworkEdge*> edges = graph[source];
            
            long long flow = 0;
            
            for (ui32 i = 0; i < edges.size(); ++i)
                flow += (long long)std::max(0, edges[i]->flow);
            
            return flow;
        }
        
        ui32 getTarget() const
        {
            return target;
        }
        
        ui32 getSource() const
        {
            return source;
        }
        
        ~Network()
        {
            clean();
        }
    };
}

class IFlowSearcher
{
public:
    virtual void searchMaxFlow(NNetwork::Network &net) = 0;
};
#endif
