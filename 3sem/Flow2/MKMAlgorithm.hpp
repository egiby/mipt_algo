#ifndef _MKMAlgorithm
#define _MKMAlgorithm

#include "Network.hpp"

namespace NSearchingBlockingFlowAlgorithm
{
    using std::vector;
    using std::queue;
    
    class LayredNetwork
    {
        NNetwork::Network * net;
        NNetwork::Network * layred_net;
        
        vector<ui32> distance;
        vector<bool> is_exist;
        vector<ui64> potential;
        
        ui32 number_of_exists;
        
        LayredNetwork()
        {
        }
        
        void bfs()
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
        
        void getPotentials()
        {
            vector<ui64> potential_to(net->size(), 0);
            vector<ui64> potential_from(net->size(), 0);
            
            for (ui32 v = 0; v < net->size(); ++v)
            {
                vector<NNetwork::NetworkEdge*> edges = net->getEdges(v);
                
                for (ui32 i = 0; i < edges.size(); ++i)
                    if (distance[v] + 1 == distance[edges[i]->to])
                    {
                        potential_to[edges[i]->to] += edges[i]->capacity - edges[i]->flow;
                        potential_from[v] += edges[i]->capacity - edges[i]->flow;
                    }
            }
            
            for (ui32 v = 0; v < net->size(); ++v)
                potential[v] = std::min(potential_to[v], potential_from[v]);
        }
        
        void getLayredNetwork()
        {
            layred_net = new NNetwork::Network(net->size(), net->getS(), net->getT());
            for (ui32 v = 0; v < net->size(); ++v)
            {
                vector<NNetwork::NetworkEdge*> edges = net->getEdges(v);
                for (ui32 i = 0; i < edges.size(); ++i)
                {
                    if (distance[v] + 1 == distance[edges[i]->to])
                        layred_net->insertEdge(v, *edges[i]);
                }
            }
        }
        
        ui32 vertexWithMinimalPotential()
        {
            ui32 v = -1;
            ui64 p = ULONG_MAX;
            
            for (ui32 i = 0; i < net->size(); ++i)
                if (p > potential[i])
                {
                    p = potential[i];
                    v =  i;
                }
            
            return v;
        }
        
    public:
        LayredNetwork(NNetwork::Network * net)
        : net(net), layred_net(0), distance(vector<ui32>(net->size(), UINT_MAX)), is_exist(vector<bool>(net->size(), 1)),
        potential(vector<ui64>(net->size(), 0)), number_of_exists(net->size())
        {
            bfs();
        }
        
        void findBlokingFlow()
        {
            getPotentials();
            getLayredNetwork();
            
            while (number_of_exists)
            {
                ui32 vertex = vertexWithMinimalPotential();
                
                //~ pushFlowLeft(vertex);
                //~ pushFlowRight(vertex);
                
                is_exist[vertex] = 0;
                number_of_exists--;
            }
        }
        
        bool isPathExists()
        {
            return distance[net->getT()] != UINT_MAX;
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
