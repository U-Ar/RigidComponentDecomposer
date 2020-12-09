#include<vector>
#include<iostream>
#include<utility>
#include<fstream>
#include<string>
#include "pebble.h"

void test_file(std::string const& filename)
{
    std::ifstream ifs(filename);
    int n, ei, tmp;
    ifs >> n;
    std::vector<std::vector<int>> G(n,std::vector<int>{});
    for (int i = 0; i < n; i++)
    {
        ifs >> ei;
        for (int j = 0; j < ei; ++j)
        {
            ifs >> tmp;
            G[i].push_back(tmp);
        }
    }
    print_graph(G);
    pebble_game(n,0,G,2,3);
}


int main()
{
    test_file("graph2.txt");
}