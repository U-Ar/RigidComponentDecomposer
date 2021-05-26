#ifndef INCLUDED_PEBBLE
#define INCLUDED_PEBBLE

#include "lib.h"

std::pair<int,int> pebble_game(int n, int m, std::vector<std::vector<int>> const& G, int k, int l);

std::pair<int,int> pebble_game_upper(int n, int m, std::vector<std::vector<int>> const& G, int k, int l);

std::pair<int,int> pebble_game_lower(int n, int m, std::vector<std::vector<int>> const& G, int k, int l);

void pebble_search(int n, int k, int l, 
                   std::vector<std::vector<int>>& D, 
                   std::vector<std::vector<int>>& DI,
                   std::vector<int>& pebbles,
                   int u, int v,
                   std::vector<bool>& visited);

void edge_insert(std::vector<std::vector<int>>& D, 
                 std::vector<std::vector<int>>& DI,
                 std::vector<int>& pebbles,
                 int u, int v);

bool component_detection(int n, int k, int l, 
                         std::vector<std::vector<int>>& D, 
                         std::vector<std::vector<int>>& DI,
                         std::vector<int>& pebbles,
                         int u, int v,
                         std::vector<bool>& visited,
                         std::vector<bool>& marked);

void component_maintenance_upper(int n, 
                           std::vector<bool> const& marked,
                           Union_pair_find& upf);
                           
void component_maintenance_lower(int n, 
                           std::vector<bool> const& marked,
                           Union_find& uf);

#endif