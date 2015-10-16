#ifndef _PUSH_RELABEL
#define _PUSH_RELABEL

#include "Network.hpp"

namespace NPushRelableAlgorithm
{
    using std::vector;
    using std::queue;
    
    class PushRelableAlgorithm: public IFlowSearcher
    {
        void push(NNetwork::NetworkEdge * edge, vector<ui64> &excess)
        {
            ui32 v = edge->reverse_edge->to;
            
            ui64 d = std::min(excess[v], (ui64)edge->capacity - edge->flow);
            
            edge->flow += d;
            edge->reverse_edge->flow -= d;
            
            excess[v] -= d;
            excess[edge->to] += d;
        }
        
        void relabel(ui32 vertex, vector<ui32> &height, const NNetwork::Network &net)
        {
            vector<NNetwork::NetworkEdge*> edges = net.getEdges(vertex);
            
            ui32 new_height = UINT_MAX;
            
            for (ui32 i = 0; i < edges.size(); ++i)
            {
                if (edges[i]->capacity - edges[i]->flow > 0)
                {
                    assert(height[edges[i]->to] >= height[vertex]);
                    new_height = std::min(new_height, height[edges[i]->to]);
                }
            }
            
            assert(new_height != UINT_MAX);
            
            new_height++;
            height[vertex] = new_height;
        }
        
        void discharge(ui32 vertex, NNetwork::Network &net, vector<ui64> &excess, vector<ui32> &height)
        {
            vector<NNetwork::NetworkEdge*>::iterator edge = net.getFirstEdge(vertex);
            vector<NNetwork::NetworkEdge*>::iterator end = net.getLastEdge(vertex);
            
            while (excess[vertex] > 0)
            {
                if (edge == end)
                {
                    relabel(vertex, height, net);
                    edge = net.getFirstEdge(vertex);
                }
                else if (height[vertex] == height[(*edge)->to] + 1 && (*edge)->capacity - (*edge)->flow > 0)
                {
                    push(*edge, excess);
                }
                else 
                {
                    edge++;
                }
            }
        }
        
        void simpleFor(NNetwork::Network &net, vector<ui64> &excess, vector<ui32> &height)
        {
            while (1)
            {
                for (ui32 v = 0; v < net.size(); ++v)
                    if (v != net.getS() && v != net.getT())
                        discharge(v, net, excess, height);
                
                bool is_end = 1;
                
                for (ui32 i = 0; i < net.size(); ++i)
                    is_end &= (excess[i] == 0 || i == net.getS() || i == net.getT());
                
                if (is_end)
                    break;
            }
        }
    
    public:
        void searchMaxFlow(NNetwork::Network &net)
        {
            vector<ui64> excess(net.size(), 0);
            vector<ui32> height(net.size(), 0);
            
            height[net.getS()] = net.size();
            vector<NNetwork::NetworkEdge*> edges = net.getEdges(net.getS());
            
            for (ui32 i = 0; i < edges.size(); ++i)
            {
                if (edges[i]->to == net.getS())
                    continue;
            
                edges[i]->flow += edges[i]->capacity;
                excess[edges[i]->to] += edges[i]->capacity;
                edges[i]->reverse_edge->flow -= edges[i]->capacity;
            }
            
            simpleFor(net, excess, height);
        }
    };
}
#endif
