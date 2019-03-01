#ifndef SORTLIST_H
#define SORTLIST_H
#include <algorithm>
template <typename T>
class SortList
{
    int MaxSize;
    bool SortBack;
    T* node;
public:
    int Size;
    inline bool cmp(T &a, T &b)
    {
        return (SortBack)?a>b:a<b;
    }

    SortList (int MaxSize, bool SortBack=false):MaxSize(MaxSize),SortBack(SortBack)
    {
        node=new T [MaxSize];
        Size=0;
    }
    ~SortList()
    {
        delete node;
    }
    int SearchInsPoint(T key)
    {
        if (!Size) return 0;
        int x=0;
        int y=Size-1;
        int i;
        while ((y-x)>1)
        {
            i=(x+y)/2;
            if (cmp(key,node[i])) y=i; else x=i;
        }
        if (cmp(key,node[x])) return x;
        if (cmp(key,node[y])) return y;
        return y+1;
    }
    bool Insert(T  it)
    {
        int i=SearchInsPoint(it);
        if (i==MaxSize) return false;
        if (i<Size) std::copy_backward(node+i,node+Size,node+Size+1);
        node[i]=it;
        if (Size<MaxSize) Size++;
        return true;
    }
    bool DeleteAt(int i)
    {
        if ((i<0) || (i>= Size)) return false;
        std::copy(node+i+1,node+Size,node+i);
        Size--;
    }
    T& operator[](int i)
    {
        return node[i];
    }


};

#endif // SORTLIST_H
