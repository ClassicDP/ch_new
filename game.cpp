#include "checker_vis.h"
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QTreeWidgetItem>


//not optimised in king steps after killing
bool GameFunctions::TryToKill(Position *ps, ItemsList<Position> *PosList, MoveItem *move, int8_t skipDir, bool WasMoreDeep)
{
    bool res=false;
    auto chIt=ps->ChList[ps->ClrOfMove]->CurrentItem();

    for (int8_t i=0; i<4; i++)
    {
        if (i==skipDir) continue;
        int8_t type=chIt->_type;
        auto x0=chIt->_pos; int8_t x1=x0+ve[i];
        //search first not empty field by ve[] direction if _king type
        if (type==_king) while (isOnBoard(x1) && !ps->board[x1]) x1+=ve[i];
        if (!isOnBoard(x1) || !ps->board[x1]) continue;
        if (ps->board[x0]->It->_color==ps->board[x1]->It->_color) continue;
        int8_t x2=x1;
        //loop if _king type for each empty field after kill-move by ve[] direction
        int8_t SkipDirection=-1;
        auto saveCaseClean_s=PosList->Last;//if false position will be added
        bool MoreDeepWasPossible=false;
        do {
            x2+=ve[i];
            if (!isOnBoard(x2) || ps->board[x2]) break;
            if (!ps->board[x2]) {
                //kill posible!
                qDebug() << x2 << ps->board[x2];
                res=true;
                auto type= chIt->_type;//save status before check for revers
                //check for simple revers to king by move
                if (type!=_king && is_king(ps,x2)) ps->board[x0]->It->_type=_king;
                MoveItem * newMove = new MoveItem(move,x0,uint8_t(x2),x1);
                //reverse color of killed checker before recursion//swap for jamp-move
                ps->board[x1]->It->_color=reverse(ps->board[x1]->It->_color);
                swap_(ps->board[x0],ps->board[x2]);
                chIt->_pos=uint8_t(x2);
                auto SaveMoreDeepStatus=MoreDeepWasPossible;auto saveCaseClean_e=PosList->Last;
                MoreDeepWasPossible|=TryToKill(ps, PosList, newMove, SkipDirection, MoreDeepWasPossible);
                //clean false positions from list if more deep move was faund
                if (MoreDeepWasPossible!=SaveMoreDeepStatus)
                {
                    while (saveCaseClean_s!=saveCaseClean_e) {
                        auto tmp=saveCaseClean_e->Prev;
                        PosList->Delete(saveCaseClean_e);
                        saveCaseClean_e=tmp;
                    }
                }
                SkipDirection=i;
                //jamp-move back after return form recursion and set old  checkers type statuses
                ps->board[x1]->It->_color=reverse(ps->board[x1]->It->_color);
                swap_(ps->board[x0],ps->board[x2]);
                chIt->_type=type;
                chIt->_pos=x0;
            }
        } while (type==_king);
    }
    if (!res && move && !WasMoreDeep)
    {
        auto newPos=new Position(ps);
        //deleting killed checkers from board and from chList
        newPos->move=move;
        auto xx=move;
        while (xx)
        {
            newPos->ChList[!newPos->ClrOfMove]->Delete(newPos->board[xx->kill]);
            newPos->board[xx->kill]=nullptr;
            xx=xx->conn;
        }
        //Add to list
        PosList->AddItem(newPos);
    }
    return res;
}

bool GameFunctions::TryToMove(Position *ps, ItemsList<Position> *PosList, MoveItem *move)
{
    bool res=false;
    auto ChIt=ps->ChList[ps->ClrOfMove]->CurrentItem();
    int8_t a,b;
    //select possible directions
    if (ChIt->_type==_king) {a=0;b=4;}
    else if (ChIt->_color==_white) {a=0;b=2;} else {a=2;b=4;}
    auto x0=ChIt->_pos;
    int8_t x1;
    auto type= ChIt->_type;//save status for posible revers
    for (int8_t i=a; i<b; i++)
    {
        x1=int8_t(x0);
        do
        {
            x1+=ve[i];
            if (!isOnBoard(x1) || ps->board[x1]) break;
            //move possible!
            res=true;
            //check for simple revers to king by move
            if (type!=_king && is_king(ps,x1)) ChIt->_type=_king;
            ChIt->_pos=uint8_t(x1);
            auto newPos=new Position(ps);
            newPos->move=new MoveItem(move,uint8_t(x0),uint8_t(x1));
            swap_(newPos->board[x0],newPos->board[x1]);
            //Add to list
            PosList->AddItem(newPos);
        } while (type==_king);
        //restore incoming position
        ChIt->_pos=x0;
        ChIt->_type=type;
    }
    return res;
}

bool GameFunctions::MakeMovesList(Position *ps, ItemsList<Position> *PosList)
{
    bool res=false;
    auto tmp=ps->ChList[ps->ClrOfMove];
    tmp->SetToStart();
    while (tmp->CurrentItem())
    {
        res|=TryToKill(ps,PosList);
        tmp->SetToNext();
    }
    if (res) return res;// exit if Kill possible
    tmp->SetToStart();
    while (tmp->CurrentItem())
    {
        res|=TryToMove(ps,PosList);
        tmp->SetToNext();
    }
    return res;
}




QString Pos2Str(int pos, int Size)
{
    QString res;
    int8_t x[2];
    x[0]='A'+int8_t(pos%Size);
    x[1]='1'+int8_t(pos/Size);
    res=x[0];
    res+=x[1];
    return res;
}

MoveTreeItem::MoveTreeItem(MoveItem *move)
{
    this->move=move;
    next=new ItemsList <MoveTreeItem>;
}

QString MoveTreeItem::toString(uint8_t Size)
{
    return (move->kill<0)?Pos2Str(move->from, Size)+"-"+Pos2Str(move->to, Size):
                          Pos2Str(move->from, Size)+":"+Pos2Str(move->to, Size);
}

Position::Position(uint8_t Size, ItemsList<CVItem> *CheckersList, checker_color nextMoveColor)
{
    len=uint8_t(Size*Size);
    this->Size=Size;
    board = new ItemOfList <Ch>* [len];
    ChList[0]=new ItemsList <Ch>;
    ChList[1]=new ItemsList <Ch>;
    fill(&board[0],len, nullptr);
    this->ClrOfMove=nextMoveColor;
    for (Ch * ch=CheckersList->begin();ch;ch=CheckersList->next())
        board[ch->_pos]=ChList[ch->_color]->AddItem(ch);
}

Game::Game(uint8_t size)
{
    funct= new GameFunctions(size);
    CheckersList=new ItemsList <CVItem>;
    next_move_clr=_white;
    _size=size;
    Pos = new Position (_size, CheckersList, next_move_clr);
    pList=new ItemsList <Position>;
}

void Game::next_move_list(Ui_Dialog *_ui)
{
    if (_ui) {this->ui=_ui; next_move_clr=reverse(next_move_clr);}
    delete Pos;
    next_move_clr=reverse(next_move_clr);
    Pos= new Position(_size, CheckersList, next_move_clr);
    pList->ClearList();
    funct->MakeMovesList(Pos, pList);
    ui->listWidget->clear();
    for (auto it=pList->begin();it;it=pList->next())
        ui->listWidget->addItem(it->MoveAsStr());
    QList<QTreeWidgetItem*> *vList=new QList<QTreeWidgetItem*>;
    auto tree=PTreeMoves(pList);
    AddToTree(vList, tree.top, board->size);
    ui->treeWidget->clear();
    ui->treeWidget->addTopLevelItems(*vList);
    board->PTree= new PTreeMoves(pList);
}

Game::~Game()
{
    delete CheckersList;
    delete Pos;
    delete pList;
    delete funct;
}

void AddToTree(QList<QTreeWidgetItem *> *vList, ItemsList<MoveTreeItem> *tree, uint8_t _size)
{
    for (auto it=tree->begin();it;it=tree->next())
    {
        QTreeWidgetItem *it_=new QTreeWidgetItem;
        it_->setText(0,it->toString(_size));
        vList->append(it_);
        QList<QTreeWidgetItem*> *yy=new QList<QTreeWidgetItem*>;
        AddToTree(yy,it->next, _size);
        it_->addChildren(*yy);
    }
}
