#ifndef GAME_H
#define GAME_H
#include "itemlist.h"
#include "stddef.h"
#include <stdint.h>
#include <qstring.h>
#include "ui_dialog.h"




enum checker_type { _simple=0, _king=1};
enum checker_color {_black=0, _white=1};
enum checker_killed {_yes=1,_no=0};

struct BaseItem
{
    uint PtrCnt=0;//the number of pointers to this object
};
struct Ch:BaseItem
{
    uint8_t _pos;// pos - position on board
    checker_type _type;
    checker_color _color;
};

class BoardItem:public Ch
{
public:
    checker_killed killed=_no;
    uint8_t b_size;
    BoardItem(BoardItem &It)
    {
        *this=It;
    }
    BoardItem (checker_color color, checker_type type, uint8_t x, uint8_t y, uint8_t b_size):
        b_size(b_size)
    {
        _pos=uint8_t(y*b_size+x);
        _color=color;
        _type=type;
    }
    BoardItem (uint8_t x, uint8_t y, uint8_t b_size):
        b_size(b_size)
    {
        _pos=uint8_t(y*b_size+x);
    }
    inline bool operator ==(const BoardItem &ch)
    {
        return (_pos==ch._pos && _type==ch._type && _color==ch._color);
    }
    inline BoardItem & operator =(const BoardItem &ch)
    {
        _pos=ch._pos; _type=ch._type;_color=ch._color;b_size=ch.b_size;
        return *this;
    }
public:
    inline uint8_t _X() {return _pos%b_size;}
    inline uint8_t _Y() {return uint8_t(_pos/b_size);}

};

class MoveItem
{
public:
    uint PtrCnt=0;//the number of pointers to this object
    MoveItem * conn=nullptr;//chain connection
    uint8_t from, to;
    int8_t kill;
    MoveItem(MoveItem * conn, uint8_t from, uint8_t to, int8_t kill=-1):conn(conn),from(from),to(to),kill(kill)
    {
        PtrCnt=0;
        if (conn) conn->PtrCnt++;
    }
    ~MoveItem()
    {
        //        if (conn) {delete conn; conn=NULL;}
    }
    inline bool operator ==(const MoveItem &move)
    {
        return (from==move.from && to==move.to && kill==move.kill);
    }
};

struct MoveTreeItem:BaseItem
{
    MoveItem * move=nullptr;
    ItemsList <MoveTreeItem> * next;
    MoveTreeItem(MoveItem * move);
    QString toString(uint8_t Size=8);
    ~MoveTreeItem()
    {
        if (move && !move->PtrCnt) {delete move;move=nullptr;}
        delete next;
    }

};


template <class OutputIterator, class Size, class T>
OutputIterator fill (OutputIterator first, Size n, const T& val)
{
    while (n>0) {
        *first = val;
        ++first; --n;
    }
    return first;     // since C++11
}

class CVItem;
class Position
{
public:
    uint PtrCnt=0;//the number of pointers to this object
    ItemOfList <Ch> ** board;//checkers on board; pointers in checkers list
    uint8_t Size, len;//len of board array
    checker_color ClrOfMove;//color of next move
    ItemsList <Ch> *ChList[2];//checkers on board
    MoveItem * move=nullptr;//Move track (chain to back);
    Position (uint8_t Size, ItemsList <CVItem> * CheckersList, checker_color nextMoveColor);
    Position (Position * pos)//copy of object
    {
        len=pos->len;
        Size=pos->Size;
        ClrOfMove=pos->ClrOfMove;
        board = new ItemOfList <Ch>* [len];
        ChList[0]=new ItemsList <Ch>;
        ChList[1]=new ItemsList <Ch>;
        fill(&board[0],len,nullptr);
        auto CurrItem0=pos->ChList[0]->Curr;
        auto CurrItem1=pos->ChList[1]->Curr;
        for (int i=0;i<2;i++)
        {
            if (!pos->ChList[i]->Count) continue;
            pos->ChList[i]->SetToStart();
            auto tmp=pos->ChList[i]->CurrentItem();
            while (tmp)
            {
                board[tmp->_pos]=ChList[i]->AddItem(tmp);
                pos->ChList[i]->SetToNext();
                tmp=pos->ChList[i]->CurrentItem();
            }
        }
        pos->ChList[0]->Curr=CurrItem0;
        pos->ChList[1]->Curr=CurrItem1;
    }
    QString PosToStr(int8_t pos)
    {
        QString res;
        int8_t x[2];
        x[0]='A'+int8_t(pos%Size);
        x[1]='1'+int8_t(pos/Size);
        res=x[0];
        res+=x[1];
        return res;
    }
    QString MoveAsStr()
    {
        QString res;
        MoveItem *x=move;
        while (x)
        {
            int8_t x0=int8_t(x->from);
            int8_t x1=x->kill;
            int8_t x2=int8_t(x->to);
            if (x1>=0) res=PosToStr(x0)+":"+PosToStr(x1)+":"+PosToStr(x2)+res;
            else res=PosToStr(x0)+"-"+PosToStr(x2)+res;
            x=x->conn;
            if (x) res=">"+res;
        }
        return res;
    }

    ~Position()
    {
        delete board;
        if (ChList[0]) delete ChList[0];
        if (ChList[1]) delete ChList[1];
        if (move)
        {
            delete move;
        }
        // !!!        while (track) {auto tmp=track;track=track->prev;delete tmp;}
    }
};

inline checker_color reverse(checker_color x)
{
    return (x==_white)?_black:_white;
}
inline bool is_king(Position *ps, int8_t x)
{
    return  ( ( (ps->ClrOfMove==_black) && (x<ps->Size) ) ||
              ( (ps->ClrOfMove==_white) && (x>=(ps->len-ps->Size)) ));
}


ItemsList <MoveTreeItem> *PositionListToMoveTree(ItemsList <Position> * pList);
class GameFunctions
{
    int8_t ve[4];//vector of directoins
    uint8_t len, Size;
    inline bool isOnBoard(int8_t pos)
    {
        len=Size*Size;
        if ((pos<0) || (pos>=Size*Size)) return false;
        return !((pos/Size+pos%Size)%2);
    }
public:
    GameFunctions(uint8_t Size):Size(Size)
    {
        ve[0]=int8_t(Size)-1;
        ve[1]=int8_t(Size)+1;
        ve[2]=-ve[0];
        ve[3]=-ve[1];
    }

    bool TryToKill(Position *ps, ItemsList<Position> *PosList,
                   MoveItem *move=nullptr, int8_t skipDir=-1, bool WasMoreDeep=false);
    bool TryToMove (Position * ps, ItemsList <Position> * PosList, MoveItem * move=nullptr);
    bool MakeMovesList(Position * ps, ItemsList <Position> * PosList);

};


class PTreeMoves
{
public:
    ItemsList <MoveTreeItem> * top=new ItemsList <MoveTreeItem>, *saveTop=nullptr;
    ItemsList <MoveTreeItem> * track=new ItemsList <MoveTreeItem>;
    ItemsList <MoveTreeItem> * Variants=new ItemsList <MoveTreeItem>;
    ItemsList <MoveItem> * MoveList=new ItemsList <MoveItem>;
    ItemsList <CVItem> * to_delete= new ItemsList <CVItem>;
    bool Wait_for_First=true;
    ItemsList <MoveTreeItem> * MoveVariants(uint8_t pos)
    {
        if (!track->Count)
        {
            for (auto it=saveTop->begin();it;it=saveTop->next())
                if (it->move->from==pos)
                {
                    top=saveTop;
                    Wait_for_First=true;
                    break;
                }
        }
        if (Wait_for_First)
        {
            to_delete->ClearList();
            Variants->ClearList();
            for (auto it=top->begin();it;it=top->next())
                if (it->move->from==pos) Variants->AddItem(it);
            if (Variants->Count) {
                top=Variants;
                Wait_for_First=false;
            } else return nullptr;
            return top;
        } else
        {
            for (auto it=top->begin();it;it=top->next())
                if (it->move->to==pos)
                {
                    track->AddItem(it);
                    top=it->next;
                    if (!top->Count) {
                        Wait_for_First=true;
                        return nullptr;
                    }
                    return top;
                }
        }
        return nullptr;
    }

    void DeleteTrack()
    {
        track->ClearList();
        top=saveTop;
        Variants->ClearList();
    }
    ~PTreeMoves()
    {
        delete track;
        delete top;
        delete Variants;
        delete MoveList;
        delete to_delete;
    }

    PTreeMoves(ItemsList<Position> *pList)
    {
        for (auto it=pList->begin();it;it=pList->next())
        {
            auto x=it->move;
            MoveItem * moveit=nullptr;
            MoveItem * nextIt=nullptr;
            while (x) {
                moveit=new MoveItem(nextIt,x->from,x->to,x->kill);
                x=x->conn;
                nextIt=moveit;
            }
            if (moveit) MoveList->AddItem(moveit);
        }
        for (auto it=MoveList->begin();it;it=MoveList->next())
        {
            auto x=it;
            auto tree=top;
            while (x)
            {
                tree->SetToStart();
                //search for MoveItem entering
                while (tree->Curr && !(*(tree->CurrentItem()->move)==*x)) tree->SetToNext();
                if (!tree->Curr) tree->AddItem(new MoveTreeItem(x));//Add new if not found entering
                x=x->conn;//set to next move-step of MoveList
                tree=tree->CurrentItem()->next;//jamp to next level of tree
            }
        }
        saveTop=top;
        track = new ItemsList <MoveTreeItem>;
    }
};

void AddToTree (QList<QTreeWidgetItem*> *vList, ItemsList <MoveTreeItem> * tree, uint8_t _size);

class BoardView;
class Game
{
public:
    ItemsList <CVItem> * CheckersList;
    bool IsEdit=true;
    ItemsList <Position> * pList;
    Position * Pos;
    checker_color next_move_clr;
    uint8_t _size;
    BoardView * board;
    GameFunctions * funct;
    Ui_Dialog * ui;
    Game(uint8_t size);
    void next_move_list(Ui_Dialog * _ui=nullptr);
    ~Game();
};


#endif // GAME_H
