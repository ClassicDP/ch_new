#ifndef FASTLIST_H
#define FASTLIST_H

template <typename T>
class FastList
{
public:
    T ** arr;
    int count, len;
    FastList (int len)
    {
        arr = new T* [len];
        this->len=len;
        count=0;
    }
    FastList (FastList * list, int len=0)
    {
        if (len) this->len=len; else this->len=list->count;
        count=list->count;
        arr = new T* [len];
        std::copy(&list->arr[0], &list->arr[count], &arr[0]);
    }
    void DeleteAt(int i)
    {
       // delete arr[i];
        if ((i+1)<count) std::copy(&arr[i+1], &arr[count], &arr[i]);
        count--;
    }
    void DeleteFrom(int i)
    {
        //for (auto j=i;j<count;j++) delete arr[j];
        count=i;
    }
    ~FastList()
    {
        delete arr;
    }
    int InsertAt(int i, T* item)
    {
        if (count<=len)
        {
            copy_backward(&arr[i], &arr[i+1], &arr[count+1]);
            arr[i]=item;
            count++;
            return 1;
        }
        return 0;
    }
    int AddItem (T * item)
    {
        if (count<=len)
        {
            arr[count]=item;
            count++;
            return 1;
        }
        return 0;
    }
    inline T * Last()
    {
        return arr[count?count-1:0];
    }
};
#endif // FASTLIST_H
