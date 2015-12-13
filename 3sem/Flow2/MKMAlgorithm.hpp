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
            BACKWARD,
            FORWARD
        };
        
        LayredNetwork()
        {
        }
        
        void bfs(const NNetwork::Network * net)
        {
            queue<ui32> bfs_queue;
            distance[net->getSource()] = 0;
            bfs_queue.push(net->getSource());
            
            while (!bfs_queue.empty())
            {
                ui32 vertex = bfs_queue.front();
                bfs_queue.pop();
                
                vector<NNetwork::NetworkEdge*> edges = net->getEdges(vertex);
                
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (distance[vertex] + 1 < distance[edges[i]->to()] && 
                    edges[i]->getCapacity() - edges[i]->getFlow() > 0)
                    {
                        distance[edges[i]->to()] = distance[vertex] + 1;
                        bfs_queue.push(edges[i]->to());
                    }
                }
            }
        }
        
        bool isEdgeNotReversed(const NNetwork::NetworkEdge * edge)
        {
            ui32 v = edge->getReverseEdge()->to();
            
            return distance[v] + 1 == distance[edge->to()];
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
                        potential_to[edges[i]->to()] += edges[i]->getCapacity() - edges[i]->getFlow();
                        potential_from[v] += edges[i]->getCapacity() - edges[i]->getFlow();
                    }
                }
            }
        }
        
        void getLayredNetwork(const NNetwork::Network * net)
        {
            bfs(net);
            
            source = net->getSource();
            target = net->getTarget();
            
            is_cleaning_necessary = 0;
            
            for (ui32 v = 0; v < net->size(); ++v)
            {
                vector<NNetwork::NetworkEdge*> edges = net->getEdges(v);
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (distance[v] + 1 == distance[edges[i]->to()])
                        insertEdge(v, *edges[i]);
                }
            }
        }
        
        ui64 calcPotencial(ui32 vertex)
        {
            if (vertex == getSource())
                return potential_from[vertex];
                
            if (vertex == getTarget())
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
                if (!is_exist[edges[i]->to()])
                    continue;
                
                if (isEdgeNotReversed(edges[i]))
                    potential_to[edges[i]->to()] -= edges[i]->getCapacity() - edges[i]->getFlow();
                else
                    potential_from[edges[i]->to()] -= edges[i]->getReverseEdge()->getCapacity() - 
                    edges[i]->getReverseEdge()->getFlow();
            }
            
            potential_from[vertex] = 0;
            potential_to[vertex] = 0;
            
            is_exist[vertex] = 0;
        }
        
        long long push(NNetwork::NetworkEdge * edge, const vector<ui64> &excess, FlowDirection direction)
        {
            ui32 v = edge->getReverseEdge()->to();
            
            long long difference;
            if (direction == FORWARD)
                difference = std::min((long long)excess[v], (long long)edge->getCapacity() - edge->getFlow());
            else
                difference = -std::min((long long)excess[v], 
                (long long)edge->getReverseEdge()->getCapacity() - edge->getReverseEdge()->getFlow());
            
            edge->push(difference);
            
            if (direction == BACKWARD)
                difference = -difference;
            
            if (direction == FORWARD)
            {
                potential_to[edge->to()] -= difference;
                potential_from[v] -= difference;
            }
            else
            {
                potential_to[v] -= difference;
                potential_from[edge->to()] -= difference;
            }
            
            return difference;
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
                    
                    if (!is_exist[edges[i]->to()])
                        continue;
                    
                    if ((isEdgeNotReversed(edges[i]) && direction == FORWARD) ||
                    (!isEdgeNotReversed(edges[i]) && direction == BACKWARD))
                    {
                        long long d = push(edges[i], excess, direction);
                        if (d)
                            q.push(edges[i]->to());
                        
                        excess[v] -= d;
                        excess[edges[i]->to()] += d;
                    }
                }
            }
        }
        
        void clear()
        {
            std::cerr << "LayredNetwork\n";
            return;
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
                
                pushFlow(vertex, FORWARD, potential);
                pushFlow(vertex, BACKWARD, potential);
                
                assert(calcPotencial(vertex) == 0);
                
                deleteVertex(vertex);
            }
        }
        
        void insertEdge(ui32 from, NNetwork::NetworkEdge &edge)
        {
            graph[from].push_back(&edge);
            graph[edge.to()].push_back(edge.getReverseEdge());
        }
        
        bool isPathExists() const
        {
            return distance[getTarget()] != UINT_MAX;
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
