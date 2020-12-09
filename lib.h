#ifndef INCLUDED_UPFLIB
#define INCLUDED_UPFLIB

#include<vector>
#include<iomanip>
#include<iostream>
#include<utility>
#include<algorithm>
#include<exception>

void print_graph(std::vector<std::vector<int>> const& G);

struct ECell
{
    ECell* next;
    std::pair<int,int> e;
    ECell(int u, int v) : e{std::make_pair(u,v)}, next(nullptr) {}
};

class EHead
{
public:
    int cid;
    ECell* head;
    ECell* tail;
    EHead* next;
    EHead* prev;
    ~EHead()
    {
        ECell *p = head, *q = head;
        while (p != nullptr)
        {
            q = p->next; delete p; p = q;
        }
    }
};

class EList
{
public:
    EHead* top;
    EHead* bottom;
    EList() : top(nullptr), bottom(nullptr) {}
    ~EList()
    {
        EHead *p = top, *q = top;
        while (p != nullptr)
        {
            q = p->next; delete p; p = q;
        }
    }
    void remove(EHead* eh)
    {
        if (eh == nullptr) throw std::invalid_argument("cannot remove nullptr");
        if (eh == top) top = eh->next;
        if (eh == bottom) bottom = eh->prev;
        if (eh->prev != nullptr) (eh->prev)->next = eh->next;
        if (eh->next != nullptr) (eh->next)->prev = eh->prev;
    }
    //merge edgeset2 to edgeset1 and remove edgeset2
    void merge(EHead* eh1, EHead* eh2)
    {
        if (eh1->tail != nullptr)
        {
            (eh1->tail)->next = eh2->head;
        }
        else 
        {
            eh1->head = eh2->head;
        }
        eh1->tail = eh2->tail;
        (eh1->tail)->next = nullptr;

        remove(eh2);
    }
    void renumber()
    {
        int i = 0;
        EHead* eh = top;
        while (eh != nullptr)
        {
            eh->cid = i++;
            eh = eh->next;
        }
    }
};


struct VCell
{
    VCell* prev;
    EHead* assoc;
    VCell* next;
    VCell() : prev(nullptr),assoc(nullptr),next(nullptr) {}
};


class VList
{
public:
    std::vector<VCell*> vec;
    VList(int n) : vec(n,nullptr) {}
    ~VList()
    {
        for (size_t i = 0; i < vec.size(); i++)
        {
            VCell* head = vec[i];
            VCell *p = head, *q = head;
            while (p != nullptr) 
            {
                q = p->next;
                delete p;
                p = q;
            }
        }
    }
    VCell* head(int n)
    {
        return vec[n];
    }
    void add(int u, VCell* vc)
    {
        if (vec[u] != nullptr) vec[u]->prev = vc;
        vc->next = vec[u];
        vec[u] = vc;
    }
    void remove(int u, VCell* vc)
    {
        if (vc == nullptr) throw std::invalid_argument("cannot remove nullptr in VV");
        if (vec[u] == vc) vec[u] = vc->next;
        if (vc->prev != nullptr) (vc->prev)->next = vc->next;
        if (vc->next != nullptr) (vc->next)->prev = vc->prev;
        delete vc;
    }
    std::vector<int> find(int u)
    {
        std::vector<int> res;
        VCell *p = vec[u];
        while (p != nullptr)
        {
            res.push_back((p->assoc)->cid);
            p = p->next;
        }
        return res;
    }
};

class Union_pair_find
{
    int LV;
    std::vector<int> MV;
    VList VV;
    EList EL;
    std::vector<bool> marked;
    int C;
public:
    const int component_count() const { return C; }
    explicit Union_pair_find(int n);
    ~Union_pair_find();
    bool pair_find(int u, int v);
    std::vector<int> find(int u);
    void pointer_union(EHead* eh1, EHead* eh2);
    void cid_union(int cid1, int cid2);
    void edge_union(std::pair<int,int> const& e1, std::pair<int,int> const& e2);
    void included_union(std::vector<bool> const& mk);
    void renumber();
    void dump();
};


#endif