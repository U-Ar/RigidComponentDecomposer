#include "pebble.h"
#include<iostream>
#include<vector>

int main()
{
    int n,m;
    std::cin >> n; 
    std::cin >> m;
    std::vector<std::vector<int>> G(n,std::vector<int>{});
    for (int i = 0; i < m; ++i) {
        int u, v;
        std::cin >> u >> v;
        G[u].push_back(v);
        G[v].push_back(u);
    }
    std::pair<int,int> gc_size = pebble_game(n,m,G,2,3);
    std::cout << gc_size.first << " " << gc_size.second; 
}