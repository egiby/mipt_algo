#include "MKMAlgorithm.hpp"
//~ #include "PushRelabel.hpp"

#include <cstdio>

int main()
{
    ui32 n, m;
    
    std::cin >> n >> m;
    std::vector<std::pair<ui32, NNetwork::NetworkEdge> > edges;
    
    NNetwork::Network net(n, 0, n - 1);
    
    for (ui32 i = 0; i < m; ++i)
    {
        ui32 v, u, c;
        scanf("%u%u%u", &v, &u, &c);
        v--;
        u--;
        
        edges.push_back(std::make_pair(v, NNetwork::NetworkEdge(u, c)));
    }
    
    for (ui32 i = 0; i < m; ++i)
        net.insertEdge(edges[i].first, edges[i].second);
    
    NSearchingBlockingFlowAlgorithm::SearchingBlockingFlowAlgorithm searcher;
    //~ NPushRelableAlgorithm::PushRelableAlgorithm searcher;
    searcher.searchMaxFlow(net);
    
    std::cout << net.getFlow() << '\n';
    
    for (ui32 i = 0; i < m; ++i)
    {
        std::cout << edges[i].second.getFlow()<< '\n';
    }
    
    return 0;
}
