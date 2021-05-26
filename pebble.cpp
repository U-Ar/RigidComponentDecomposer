#include "pebble.h"

std::pair<int,int> pebble_game(int n, int m, std::vector<std::vector<int>> const& G, int k, int l)
{
    if (l <= 0 || l >= 2*k) throw std::invalid_argument("pebble game with such (k,l) is not implemented.");
    else if (l >= k+1)
    {
        //edge-disjoint
        return pebble_game_upper(n,m,G,k,l);
    }
    else 
    {
        //vertex-disjoint
        return pebble_game_lower(n,m,G,k,l);
    }
}

std::pair<int,int> pebble_game_upper(int n, int m, std::vector<std::vector<int>> const& G, int k, int l)
{
    std::vector<std::vector<int>> D(n,std::vector<int>{});
    std::vector<std::vector<int>> DI(n,std::vector<int>{});
    std::vector<int> pebbles(n,k);
    Union_pair_find upf{n};

    std::vector<bool> bfs_visited(n,false), visited(n,false), marked(n,false);
    for (int u = 0; u < n; ++u)
    {
        bfs_visited[u] = true;
        for (int i = 0; i < G[u].size(); ++i)
        {
            if (bfs_visited[G[u][i]]) continue;
            if (upf.pair_find(u,G[u][i])) continue;
            else
            {
                //pebble search,insert
                //component detection, maintenance
                pebble_search(n,k,l,D,DI,pebbles,u,G[u][i],visited);
                edge_insert(D,DI,pebbles,u,G[u][i]);
                bool flag = component_detection(n,k,l,D,DI,pebbles,u,G[u][i],visited,marked);
                
                if (flag) component_maintenance_upper(n,marked,upf);
                
            }
        }
    }
    std::pair<int,int> gc_size = upf.component_size_top2();
    return gc_size;
}

std::pair<int,int> pebble_game_lower(int n, int m, std::vector<std::vector<int>> const& G, int k, int l)
{
    std::vector<std::vector<int>> D(n,std::vector<int>{});
    std::vector<std::vector<int>> DI(n,std::vector<int>{});
    std::vector<int> pebbles(n,k);
    Union_find uf{n};

    std::vector<bool> bfs_visited(n,false), visited(n,false), marked(n,false);
    for (int u = 0; u < n; ++u)
    {
        bfs_visited[u] = true;
        for (int i = 0; i < G[u].size(); ++i)
        {
            if (bfs_visited[G[u][i]]) continue;
            if (uf.find(u,G[u][i])) continue;
            else
            {
                //pebble search,insert
                //component detection, maintenance
                pebble_search(n,k,l,D,DI,pebbles,u,G[u][i],visited);
                edge_insert(D,DI,pebbles,u,G[u][i]);
                bool flag = component_detection(n,k,l,D,DI,pebbles,u,G[u][i],visited,marked);
                if (flag) component_maintenance_lower(n,marked,uf);
                
            }
        }
    }
    std::pair<int,int> gc_size = uf.component_size_top2();
    return gc_size;
}

//remove an element in vector
//pop back after swapping the element with back
void edge_deletion(int u, int v,
                   std::vector<std::vector<int>>& D)
{
    for (size_t i = 0; i < D[u].size(); ++i)
    {
        if (D[u][i] == v)
        {
            std::swap(D[u][i],D[u].back());
            D[u].pop_back();
            return;
        }
    }
}

void edge_inversion(std::vector<int>& path,
                    std::vector<std::vector<int>>& D,
                    std::vector<std::vector<int>>& DI)
{
    for (int i = 0; i < path.size()-1; ++i)
    {
        edge_deletion(path[i],path[i+1],D);
        edge_deletion(path[i+1],path[i],DI);
        D[path[i+1]].push_back(path[i]);
        DI[path[i]].push_back(path[i+1]);
    }
}

void _pebble_DFS(int n,
                std::vector<std::vector<int>>& D,
                std::vector<std::vector<int>>& DI,
                std::vector<int>& pebbles,
                int u,
                std::vector<bool>& visited,
                std::vector<int>& path,
                bool & finished)
{
    for (size_t i = 0; i < D[u].size(); ++i)
    {
        if (!visited[D[u][i]])
        {
            if (pebbles[D[u][i]] > 0)
            {
                path.push_back(D[u][i]);
                pebbles[D[u][i]]--;
                pebbles[path[0]]++;
                edge_inversion(path,D,DI);
                finished = true;
            } 
            else 
            {
                visited[D[u][i]] = true;
                path.push_back(D[u][i]);
                _pebble_DFS(n,D,DI,pebbles,D[u][i],visited,path,finished);
                path.pop_back();
            }
        }
        if (finished) return;
    }
}

bool pebble_DFS(int n,
                std::vector<std::vector<int>>& D,
                std::vector<std::vector<int>>& DI,
                std::vector<int>& pebbles,
                int u,
                std::vector<bool>& visited)
{
    std::vector<int> path;
    path.push_back(u);
    bool finished = false;
    _pebble_DFS(n,D,DI,pebbles,u,visited,path,finished);
    return finished;
}

void pebble_search(int n, int k, int l, 
                   std::vector<std::vector<int>>& D, 
                   std::vector<std::vector<int>>& DI,
                   std::vector<int>& pebbles,
                   int u, int v,
                   std::vector<bool>& visited)
{
    bool changed;
    while (pebbles[u]+pebbles[v] < l+1)
    {
        std::fill(visited.begin(),visited.end(),false);
        visited[u] = visited[v] = true;
        changed = pebble_DFS(n,D,DI,pebbles,u,visited);
        if (!changed) break;
    }
    while (pebbles[u]+pebbles[v] < l+1)
    {
        std::fill(visited.begin(),visited.end(),false);
        visited[u] = visited[v] = true;
        pebble_DFS(n,D,DI,pebbles,v,visited);
    }
}

void edge_insert(std::vector<std::vector<int>>& D, 
                 std::vector<std::vector<int>>& DI,
                 std::vector<int>& pebbles,
                 int u, int v)
{
    if (pebbles[u] == 0)
    {
        pebbles[v]--;
        D[v].push_back(u);
        DI[u].push_back(v);
    }
    else
    {
        pebbles[u]--;
        D[u].push_back(v);
        DI[v].push_back(u);
    }
}

void reach(std::vector<std::vector<int>> const& D, int u, std::vector<bool>& visited)
{
    visited[u] = true;
    for (size_t i = 0; i < D[u].size(); ++i)
    {
        if (!visited[D[u][i]]) reach(D,D[u][i],visited);
    }
}

void unreach(std::vector<std::vector<int>> const& DI, int u, std::vector<bool>& marked)
{
    marked[u] = false;
    for (size_t i = 0; i < DI[u].size(); ++i)
    {
        if (marked[DI[u][i]]) unreach(DI,DI[u][i],marked);
    }
}

bool component_detection(int n, int k, int l, 
                        std::vector<std::vector<int>>& D, 
                        std::vector<std::vector<int>>& DI,
                        std::vector<int>& pebbles,
                        int u, int v,
                        std::vector<bool>& visited,
                        std::vector<bool>& marked)
{
    std::fill(visited.begin(),visited.end(),false);
    std::fill(marked.begin(),marked.end(),true);
    
    //when num of pebbles > l, no component is generated
    if (pebbles[u]+pebbles[v] > l) return false;

    reach(D,u,visited);
    reach(D,v,visited);

    //if some w in reach(u,v) have a pebble, return nothing
    for (int i = 0; i < n; ++i)
    {
        if (visited[i] && (pebbles[i] > 0) && i!=u && i!=v) return false;
    }

    //DFS on DI from w in V-reach(u,v) s.t. pebbles[w]>0  and mark all visited vertices
    for (int i = 0; i < n; ++i)
    {
        if ((!visited[i]) && (pebbles[i] > 0)) unreach(DI,i,marked);
    }

    return true;
}

void component_maintenance_upper(int n, 
                           std::vector<bool> const& marked,
                           Union_pair_find& upf)
{
    upf.included_union(marked);
}

void component_maintenance_lower(int n,
                           std::vector<bool> const& marked,
                           Union_find& uf)
{
    int tmp = -1;
    for (int i = 0; i < n; ++i)
    {
        if (marked[i])
        {
            if (tmp != -1) uf.unite(tmp,i);
            tmp = i;
        }
    }
}