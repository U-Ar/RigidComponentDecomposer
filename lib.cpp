#include"lib.h"

void print_graph(std::vector<std::vector<int>> const& G)
{
    std::cout << "called print_graph" << std::endl;
    std::cout << "N=" << G.size() << std::endl;
    for (size_t i = 0; i < G.size(); ++i)
    {
        std::cout << i << " : ";
        for (size_t j = 0; j < G[i].size(); ++j)
            std::cout << G[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool comp_edge(const std::pair<int,int>& e1,
               const std::pair<int,int>& e2)
{
    return (e1.first == e2.first && e1.second == e2.second)
        || (e1.first == e2.second && e1.second == e2.first);
}

Union_pair_find::Union_pair_find(int n)
    : LV(0), MV(n,0), VV(n), EL{}, marked(n,false), C{0}
{
    MV[0] = 1;
}

Union_pair_find::~Union_pair_find()
{
    
}

bool Union_pair_find::pair_find(int u, int v)
{
    if (LV != u)
    {
        LV = u;
        //enumerate components u belongs to and set entries of MV to 1
        std::fill(MV.begin(),MV.end(),0);
        VCell* p = VV.vec[u];
        while (p != nullptr)
        {
            EHead* eh = p->assoc;
            ECell* ec = eh->head;
            while (ec != nullptr)
            {
                MV[ec->e.first] = MV[ec->e.second] = 1;
                ec = ec->next;
            }
            p = p->next;
        }
    }
    
    if (MV[v]) return true;
    else 
    {
        VCell *vc1 = new VCell;
        VCell *vc2 = new VCell;
        ECell *e   = new ECell{u,v};
        EHead *eh  = new EHead;
        if (EL.bottom==nullptr) eh->cid = 0;
        else                    eh->cid = EL.bottom->cid + 1;
        eh->prev = EL.bottom;
        eh->next = nullptr;
        vc1->assoc = vc2->assoc = eh;
        eh->head = eh->tail = e;
        if (EL.bottom == nullptr)
        {
            EL.top = EL.bottom = eh;
        } else 
        {
            EL.bottom->next = eh;
            EL.bottom = eh;
        }
        VV.add(u,vc1);
        VV.add(v,vc2);
        MV[v] = 1;
        C++;
        return false;
    }
}

std::vector<int> Union_pair_find::find(int const u)
{
    return VV.find(u);
}

void Union_pair_find::pointer_union(EHead* eh1, EHead* eh2)
{
    if (eh1 == eh2) return;
    std::fill(marked.begin(),marked.end(),false);
    ECell* e = eh1->head;
    while (e != nullptr)
    {
        marked[e->e.first] = marked[e->e.second] = true;
        e = e->next;
    }

    for (int u = 0; u < VV.vec.size(); ++u)
    {
        VCell* vc = VV.vec[u];
        VCell* tmp = vc;
        while (vc != nullptr)
        {
            if (vc->assoc == eh2)
            {
                if (marked[u])
                {
                    tmp = vc->next;
                    VV.remove(u,vc);
                    vc = tmp;
                    continue;
                }
                else 
                {
                    vc->assoc = eh1;
                }
            }
            vc = vc->next;
        }
    }
    EL.merge(eh1,eh2);
    C--;
    e = eh1->head;
    while (e != nullptr)
    {
        if (LV == e->e.first || LV == e->e.second)
        {
            e = eh1->head;
            while (e != nullptr)
            {   
                MV[e->e.first] = MV[e->e.second] = 1;
                e = e->next;
            }
            return;
        }
        e = e->next;
    }
}

void Union_pair_find::cid_union(int const cid1, int const cid2)
{
    if (cid1 == cid2) return;
    EHead *eh = EL.top;
    EHead *eh1,*eh2;
    while (eh != nullptr)
    {
        if (eh->cid == cid1) eh1 = eh;
        if (eh->cid == cid2) eh2 = eh;
        eh = eh->next;
    }

    pointer_union(eh1,eh2);
}


void Union_pair_find::edge_union(std::pair<int,int> const& e1, std::pair<int,int> const& e2)
{
    EHead *eh1 = nullptr, *eh2 = nullptr;
    EHead *eh = EL.top;
    while (eh != nullptr)
    {
        ECell* ec = eh->head;
        while (ec != nullptr)
        {
            if (comp_edge(ec->e,e1)) eh1 = eh;
            if (comp_edge(ec->e,e2)) eh2 = eh;
            ec = ec->next;
        }
        eh = eh->next;
    }
    if (eh1 == nullptr || eh2 == nullptr) throw std::invalid_argument("such edge does not exist");
    if (eh1 == eh2) return;

    pointer_union(eh1,eh2);
}

void Union_pair_find::included_union(std::vector<bool> const& mk)
{
    // merge all components s.t. all of their vertices are marked
    std::vector<EHead*> included(0);
    EHead* eh = EL.top;
    while (eh != nullptr)
    {
        bool flag = true;
        ECell* ec = eh->head;
        while (ec != nullptr)
        {
            if ((!mk[ec->e.first])||(!mk[ec->e.second]))
            {
                flag = false;
                break;
            }
            ec = ec->next;
        }
        if (flag) included.push_back(eh);
        eh = eh->next;
    }
    if (!included.empty())
    {
        for (size_t i = 1; i < included.size(); ++i) pointer_union(included[0],included[i]);
    }
    renumber();
}

void Union_pair_find::renumber() { EL.renumber(); }

void Union_pair_find::dump()
{
    std::cout << "union-pair-find debug info" << std::endl;
    size_t n = VV.vec.size();
    std::cout << "VV:" << std::endl;
    for (size_t i = 0; i < n; ++i)
    {
        std::cout << std::setw(6) << std::setfill(' ') << std::left << i << ": ";
        VCell* vc = VV.vec[i];
        while (vc != nullptr)
        {
            std::cout << (vc->assoc)->cid << " ";
            vc = vc->next;
        }
        std::cout << std::endl;
    }

    std::cout << "EL:" << std::endl;
    EHead* eh = EL.top;
    while (eh != nullptr)
    {
        std::cout << std::setw(6) << std::setfill(' ') << std::left << eh->cid << ": ";
        ECell* ec = eh->head;
        while (ec != nullptr)
        {
            std::cout << "(" << ec->e.first << "," << ec->e.second << ") ";
            ec = ec->next;
        }
        std::cout << std::endl;
        eh = eh->next;
    }

    std::cout << "LV: " << LV << std::endl;
    std::cout << "MV: " ;
    for (size_t i = 0; i < MV.size(); ++i) 
    {
        if (MV[i]) std::cout << i << " ";
    }
    std::cout << std::endl << std::endl;
}

std::pair<int,int> Union_pair_find::component_size_top2()
{
    int max1 = -1;
    int max2 = -2;
    std::unordered_set<int> uset{};


    EHead* eh = EL.top;
    while (eh != nullptr)
    {
        ECell* ec = eh->head;
        while (ec != nullptr)
        {
            uset.insert(ec->e.first);
            uset.insert(ec->e.second);
            ec = ec->next;
        }
        int tmp = uset.size();
        if (tmp > max1)
        {
            max2 = max1;
            max1 = tmp;
        } else if (tmp > max2)
        {
            max2 = tmp;
        }

        uset.clear();
        eh = eh->next;
    }

    return std::make_pair(max1,max2);
}


//below: implemantation of UnionFind

Union_find::Union_find(int n)
    : par(n,-1), sz(n,1) 
{
    for (size_t i = 0; i < par.size(); ++i) par[i] = i; 
}

void Union_find::init()
{
    for (size_t i = 0; i < par.size(); ++i) 
    {
        par[i] = i; sz[i] = 0;
    }
}

bool Union_find::find(int u, int v)
{
    return parent(u) == parent(v);
}

void Union_find::unite(int u, int v)
{
    int pu = parent(u);
    int pv = parent(v);
    if (pu == pv) return;
    if (sz[pu] >= sz[pv]) 
    {
        par[pv] = pu;
        sz[pu] += sz[pv];
    } 
    else 
    {
        par[pu] = pv;
        sz[pv] += sz[pu];
    }
}

int Union_find::parent(int u)
{
    while (u != par[u]) u = par[u];
    return u;
}

int Union_find::size(int u)
{
    return sz[parent(u)];
}


std::pair<int,int> Union_find::component_size_top2()
{
    int max1 = -1, max2 = -2;
    std::vector<bool> visited(sz.size(),false);

    for (size_t i = 0; i < sz.size(); ++i)
    {
        int p = parent(i);
        if (visited[p]) continue;
        visited[p] = true;
        if (sz[p] > max1) 
        {
            max2 = max1;
            max1 = sz[p];
        }
        else if (sz[p] > max2)
        {
            max2 = sz[p];
        }
    }
    return std::make_pair(max1,max2);
}