#ifndef ITEMLIST_H
#define ITEMLIST_H
#include <algorithm>
#include <iterator>
//#define NULL nullptr

template <typename T>
inline void swap_(T & x, T & y )
{
    T  tmp=x;
    x=y;
    y=tmp;
}

template <typename T>
struct ItemOfList
{
    T * It;
    ItemOfList *Next = nullptr;
    ItemOfList *Prev = nullptr;
};

template <typename T>
class ItemsList
{

public:    

    ItemOfList <T> *First;
    ItemOfList <T> *Last;
    ItemOfList <T> *Curr;
    int Count;
    ItemsList()
    {
        First = NULL;
        Last = NULL;
        Curr=Last;
        Count = 0;
    }
    inline void SetToStart()
    {
        Curr=First;
    }
    inline void SetToEnd()
    {
        Curr=Last;
    }
    inline void SetToNext()
    {
        if (Curr) Curr=Curr->Next;
    }
    inline void SetToPrev()
    {
        if (Curr) Curr=Curr->Prev;
    }
    inline T* begin()
    {
        SetToStart();
        return CurrentItem();
    }
    inline T* end()
    {
        Curr=Last;
        return CurrentItem();
    }
    inline T * CurrentItem()
    {
        if (Curr) return Curr->It;
        else return nullptr;
    }
    inline T * next()
    {
        if (Curr) Curr=Curr->Next;
        if (Curr) return Curr->It;
        else return nullptr;
    }
    inline T * prev()
    {
        if (Curr->Prev) Curr=Curr->Prev;
        if (Curr) return Curr->It;
        else return nullptr;
    }
    ItemOfList <T> * AddItem(T * it)
    {
        ItemOfList <T> *X = new ItemOfList <T>;
        X->It=it;
        if(!First)
        {
            First = X;
            Last = X;
        }
        else
        {
            X->Prev = Last;
            Last->Next = X;
            Last = X;
        }
        Count ++;
        Curr=Last;
        it->PtrCnt++;
        return X;
    }
    ItemOfList <T> * At(int i)
    {
        ItemOfList <T> *X=First;
        while (i>0)
        {
            if (!X) return X;
            X=X->Next;
            i--;
        }
        return X;
    }

    ItemOfList <T> * searchIt (T* Itm)
    {
        ItemOfList <T>  *X=First;
        while (X)
        {
            if (X->It==Itm) return X;
            X=X->Next;
        }
        return NULL;
    }
    ItemOfList <T> * InsertBeforeCurrent(T * it)
    {
        ItemOfList <T> *X = new ItemOfList <T>;
        X->It=it;
        if(!First)
        {
            First = X;
            Last = X;
        } else
        {
            if (!Curr) Curr=First;
            X->Prev = Curr->Prev;
            X->Next=Curr;
        }
        Count ++;
        Curr=X;
        it->PtrCnt++;
        return X;
    }
    ItemOfList <T> * InsertAfterCurrent(T * it)
    {
        ItemOfList <T> *X = new ItemOfList <T>;
        X->It=it;
        if(!First)
        {
            First = X;
            Last = X;
        } else
        {
            if (!Curr) Curr=First;
            X->Next = Curr->Next;
            X->Prev=Curr;
            Curr->Next=X;
        }
        Count ++;
        Curr=X;
        it->PtrCnt++;
        return X;
    }

    void Delete(ItemOfList <T> *X)
    {
        if (!X) return;
        if (X->It)
        {
            if (!(--(X->It->PtrCnt)))
            {
                delete X->It;
                X->It=nullptr;
            }
        }
        if(X->Prev)
            X->Prev->Next = X->Next;
        else
            First = X->Next;
        if(X->Next)
            X->Next->Prev=X->Prev;
        else
            Last = X->Prev;
        delete X;
        Count --;
        Curr=Last;
    }

    void DeleteLate(ItemOfList <T> *X)
    {
        if (!X) return;
        if (X->It)
        {
            if (!(--(X->It->PtrCnt)))
            {
                X->It->deleteLater();
                X->It=NULL;
            }
        }
        if(X->Prev)
            X->Prev->Next = X->Next;
        else
            First = X->Next;
        if(X->Next)
            X->Next->Prev=X->Prev;
        else
            Last = X->Prev;
        delete X;
        Count --;
        Curr=Last;
    }

    void ClearList()
    {
        while(Count)
        {
            Delete(First);
        }
        Curr=Last;
    }
    void ClearLate()
    {
        while(Count)
        {
            DeleteLate(First);
        }
        Curr=Last;
    }

    ~ItemsList()
    {
        ClearList();
    }
};




#endif // ITEMLIST_H
