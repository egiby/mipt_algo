#ifndef _MKMAlgorithm
#define _MKMAlgorithm

#include "Network.hpp"

namespace NSearchingBlockingFlowAlgorithm
{
    using std::vector;
    using std::queue;
    
    class LayredNetwork: public NNetwork::Network
    {
        vector<ui32> distance;
        vector<bool> is_exist;
        vector<ui64> potential_to;
        vector<ui64> potential_from;
        
        enum FlowDirection
        {
            TO_LEFT,
            TO_RIGHT
        };
        
        LayredNetwork()
        {
        }
        
        void bfs(const NNetwork::Network * net)
        {
            queue<ui32> bfs_queue;
            distance[net->getS()] = 0;
            bfs_queue.push(net->getS());
            
            while (!bfs_queue.empty())
            {
                ui32 vertex = bfs_queue.front();
                bfs_queue.pop();
                
                vector<NNetwork::NetworkEdge*> edges = net->getEdges(vertex);
                
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (distance[vertex] + 1 < distance[edges[i]->to] && edges[i]->capacity - edges[i]->flow > 0)
                    {
                        distance[edges[i]->to] = distance[vertex] + 1;
                        bfs_queue.push(edges[i]->to);
                    }
                }
            }
        }
        
        bool isEdgeNotReversed(const NNetwork::NetworkEdge * edge)
        {
            ui32 v = edge->reverse_edge->to;
            
            return distance[v] + 1 == distance[edge->to];
        }
        
        void getPotentials()
        {
            for (ui32 v = 0; v < size(); ++v)
            {
                vector<NNetwork::NetworkEdge*> edges = getEdges(v);
                
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (isEdgeNotReversed(edges[i]))
                    {
                        potential_to[edges[i]->to] += edges[i]->capacity - edges[i]->flow;
                        potential_from[v] += edges[i]->capacity - edges[i]->flow;
                    }
                }
            }
        }
        
        void getLayredNetwork(const NNetwork::Network * net)
        {
            bfs(net);
            
            s = net->getS();
            t = net->getT();
            
            for (ui32 v = 0; v < net->size(); ++v)
            {
                vector<NNetwork::NetworkEdge*> edges = net->getEdges(v);
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (distance[v] + 1 == distance[edges[i]->to])
                        insertEdge(v, *edges[i]);
                }
            }
        }
        
        ui64 calcPotencial(ui32 vertex)
        {
            if (vertex == getS())
                return potential_from[vertex];
                
            if (vertex == getT())
                return potential_to[vertex];
                
            return std::min(potential_to[vertex], potential_from[vertex]);
        }
        
        ui32 vertexWithMinimalPotential()
        {
            ui32 v = UINT_MAX;
            ui64 p = ULONG_MAX;
            
            for (ui32 i = 0; i < size(); ++i)
                if (p > calcPotencial(i) && is_exist[i])
                {
                    p = calcPotencial(i);
                    v =  i;
                }
            
            return v;
        }
        
        void deleteVertex(ui32 vertex)
        {
            vector<NNetwork::NetworkEdge*> edges = getEdges(vertex);
            
            for (ui32 i = 0; i < edges.size(); ++i)
            {
                if (!is_exist[edges[i]->to])
                    continue;
                
                if (isEdgeNotReversed(edges[i]))
                    potential_to[edges[i]->to] -= edges[i]->capacity - edges[i]->flow;
                else
                    potential_from[edges[i]->to] -= edges[i]->reverse_edge->capacity - edges[i]->reverse_edge->flow;
            }
            
            potential_from[vertex] = 0;
            potential_to[vertex] = 0;
            
            is_exist[vertex] = 0;
        }
        
        long long push(NNetwork::NetworkEdge * edge, const vector<ui64> &excess, FlowDirection direction)
        {
            ui32 v = edge->reverse_edge->to;
            
            long long d;
            if (direction == TO_RIGHT)
                d = std::min((long long)excess[v], (long long)edge->capacity - edge->flow);
            else
                d = -std::min((long long)excess[v], (long long)edge->reverse_edge->capacity - edge->reverse_edge->flow);
            
            edge->flow += d;
            edge->reverse_edge->flow -= d;
            
            if (direction == TO_LEFT)
                d = -d;
            
            if (direction == TO_RIGHT)
            {
                potential_to[edge->to] -= d;
                potential_from[v] -= d;
            }
            else
            {
                potential_to[v] -= d;
                potential_from[edge->to] -= d;
            }
            
            return d;
        }
        
        void pushFlow(ui32 vertex, FlowDirection direction, const ui64 potential)
        {
            queue<ui32> q;
            q.push(vertex);
            
            vector<ui64> excess(size(), 0);
            
            excess[vertex] = potential;
            
            while (!q.empty())
            {
                ui32 v = q.front();
                q.pop();
                
                vector<NNetwork::NetworkEdge*> edges = getEdges(v);
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (excess[v] == 0)
                        break;
                    
                    if (!is_exist[edges[i]->to])
                        continue;
                    
                    if ((isEdgeNotReversed(edges[i]) && direction == TO_RIGHT) ||
                    (!isEdgeNotReversed(edges[i]) && direction == TO_LEFT))
                    {
                        long long d = push(edges[i], excess, direction);
                        if (d)
                            q.push(edges[i]->to);
                        
                        excess[v] -= d;
                        excess[edges[i]->to] += d;
                    }
                }
            }
        }
        
    public:
        LayredNetwork(const NNetwork::Network * net)
        : distance(vector<ui32>(net->size(), UINT_MAX)), is_exist(vector<bool>(net->size(), 1)), 
        potential_to(vector<ui64>(net->size(), 0)), potential_from(vector<ui64>(net->size(), 0))
        {
            graph.resize(net->size());
            getLayredNetwork(net);
            getPotentials();
        }
        
        void findBlokingFlow()
        {
            for (ui32 i = 0; i < size(); ++i)
            {
                ui32 vertex = vertexWithMinimalPotential();
                assert(vertex != UINT_MAX);
                
                ui64 potential = calcPotencial(vertex);
                
                pushFlow(vertex, TO_RIGHT, potential);
                pushFlow(vertex, TO_LEFT, potential);
                
                assert(calcPotencial(vertex) == 0);
                
                deleteVertex(vertex);
            }
        }
        
        void insertEdge(ui32 from, NNetwork::NetworkEdge &edge)
        {
            graph[from].push_back(&edge);
            graph[edge.to].push_back(edge.reverse_edge);
        }
        
        bool isPathExists() const
        {
            return distance[getT()] != UINT_MAX;
        }
    };
    
    class SearchingBlockingFlowAlgorithm: public IFlowSearcher
    {
    public:
        void searchMaxFlow(NNetwork::Network &net)
        {
            while (1)
            {
                LayredNetwork l_net(&net);
                if (!l_net.isPathExists())
                    break;
                    
                l_net.findBlokingFlow();
            }
        }
    };
}
#endif
