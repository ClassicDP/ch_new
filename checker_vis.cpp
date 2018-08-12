#include "math.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimeLine>
#include "game.h"
#include "itemlist.h"
#include <QGraphicsView>
#include "checker_vis.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>


CVItem::CVItem(BoardItem * board_item, ChGrapicsScene *scene, QPixmap pix):BoardItem(*board_item)

{
    isField=false;
    this->brd_it=board_item;
    this->scene=scene;
    board=scene->board;
    setPixmap(pix);
    setScale(scene->fsize/pix.width());
    setPos(dX(),dY());
    selected=0;

}

CVItem::CVItem(BoardItem *board_item, ChGrapicsScene *scene):BoardItem(*board_item)
{
    isField=false;
    this->brd_it=board_item;
    this->scene=scene;
    board=scene->board;
    auto pix=board->Pixmaps.pix(_color,_type, killed);
    setPixmap(pix);
    setScale(scene->fsize/pix.width());
    setPos(dX(),dY());
    selected=0;

}

CVItem::CVItem(uint8_t X, uint8_t Y, ChGrapicsScene *scene):BoardItem(X,Y, scene->size)
{
    isField=true;
    this->scene=scene;
    board=scene->board;
}

void CVItem::setPos(qreal x, qreal y)
{
    if (!board->white_down)
    {
        x=scene->w-x-scene->fsize;
        y=scene->h-y-scene->fsize;
    }
    QGraphicsPixmapItem::setPos(x,y);
}

QPointF CVItem::pos()
{
    auto tmp=QGraphicsPixmapItem::pos();
    if (!board->white_down)
    {
        tmp.setX(-tmp.x()+scene->w-scene->fsize);
        tmp.setY(-tmp.y()+scene->h-scene->fsize);
    }
    return tmp;
}

void CVItem::UpdatePixmap()
{
    auto pix=board->Pixmaps.pix(_color,_type, killed);
    setPixmap(pix);
    setScale(scene->fsize/pix.width());
    setPos(dX(),dY());
}


double CVItem::dX()
{
    return (scene->fsize*_X());
}

double CVItem::dY()
{
    return scene->fsize*(scene->size-1-_Y());
}

double CVItem::Xpos(uint8_t pos)
{
    uint8_t i=pos%scene->size;
    return i*scene->fsize;

}
double CVItem::Ypos(uint8_t pos)
{
    u_int8_t j=pos/scene->size;
    return (scene->size-j-1)*scene->fsize;
}

uint8_t CVItem::Pos()
{
    return _pos;
}
inline bool is_king(checker_color clr, BoardView *brd,uint8_t x)
{
    return  ( ( (clr==_black) && (x<brd->size) ) ||
              ( (clr==_white) && (x>=(brd->size*(brd->size-1))) ) );
}


void CVItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (board->game->IsEdit)
    {
        if (!this->isField)
        {
            auto * i=board->ChSearch(_pos);
            if (i)
            {
                int x=_color+2*_type;
                x++;
                if (x==4)
                {
                    auto xx=board->CheckersList->searchIt(i);
                    scene->removeItem(xx->It);
                    board->CheckersList->DeleteLate(xx);
                } else
                {
                    if (x&1) i->_color=_white;
                    else i->_color=_black;
                    if ((x/2)&1) i->_type=_king;
                    else i->_type=_simple;
                    i->UpdatePixmap();
                }
            }
        } else if (!((_X()+_Y())%2))
        {
            auto chL = board->CheckersList;
            bool res=false;
            for (int i=0;i<chL->Count;i++) res|=(chL->At(i)->It->_X()==_X()) && (chL->At(i)->It->_Y()==_Y());
            if (!res)
            {
                board->CheckersList->
                        AddItem(new CVItem(new BoardItem(_black,_simple,_X(),_Y(), scene->size),scene));
                scene->addItem(board->CheckersList->Last->It);
            }
        }
        return;
    } else TrackLoop();

}

void CVItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //    if (hideItem) hideItem->show();
    //    if (!scene->params->IsEdit) ChAnimation.LineMoveAnimation(this, X(), Y(), 100, 500);
    //    QGraphicsItem::mouseReleaseEvent(event);
    //    selected=0;
}

void CVItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    return;
    if (board->game->IsEdit) return;
    // in Board check
    double dx=mapToScene(event->pos()).x()-board->ChAnimation->mxPos+dX();
    double dy=mapToScene(event->pos()).y()-board->ChAnimation->myPos+dY();
    dx=dx<0?0:dx;dx=dx>scene->sq-scene->fsize?scene->sq-scene->fsize:dx;
    dy=dy<0?0:dy;dy=dy>scene->sq-scene->fsize?scene->sq-scene->fsize:dy;
    setPos(dx,dy);

    this->setZValue(1);

    //QGraphicsItem::mouseMoveEvent(event);
}

void CVItem::RefreshPoints(ItemsList <MoveTreeItem> * top, bool clear_only)
{

    for (auto it=board->last_points->begin();it;it=board->last_points->next())
        scene->removeItem(it);
    board->last_points->ClearLate();
    if (!clear_only) for (auto it=top->begin();it;it=top->next())
        board->last_points->AddItem(board->SetPoint(it->move->to));
}
void CVItem::TrackLoop()
{

    if (board->ChAnimation->busy) return;
    int trkcnt;
    ItemsList <MoveTreeItem> * top; auto XX=board->PTree;
    do
    {
        trkcnt=XX->track->Count;
        if (board->last_points->Count==1)
            top=XX->MoveVariants(board->last_points->Last->It->_pos); else
            top=XX->MoveVariants(_pos);
        if (top && top->Count) RefreshPoints(top);
        if (trkcnt<XX->track->Count)
        {
            auto xx=XX->track->Last->It->move;
            auto ChX=board->ChSearch(xx->from);
            foreach (QGraphicsItem * i, scene->items()) i->setZValue(0);
            ChX->setZValue(1);
            board->ChAnimation->LineMoveAnimation
                    (ChX,ChX->Xpos(xx->to), ChX->Ypos(xx->to), 100, 700);
            //            qDebug () << "animation start"<< ChX->_X() << ChX->_Y();
            ChX->_pos=xx->to;
            if (is_king(ChX->_color, scene->board, xx->to))
            {
                ChX->_type=_king;
                ChX->UpdatePixmap();
            }
            if (xx->kill>=0)
            {
                auto yy=(CVItem *) board->ChSearch(xx->kill);
                XX->to_delete->AddItem(yy);
                yy->killed=_yes;
                yy->UpdatePixmap();
            }
            if (XX->Wait_for_First) {
                RefreshPoints(top, true);
                XX->to_delete->SetToStart();
                while (XX->to_delete->Curr)
                {
                    board->CheckersList->
                            Delete(board->CheckersList->searchIt(XX->to_delete->CurrentItem()));
                    XX->to_delete->SetToNext();
                    board->Draw();
                }
                board->game->next_move_list();
                return;
            }
        }

    } while (board->last_points->Count==1);
}



ChGrapicsScene::ChGrapicsScene(BoardView * board):board(board){}

void ChGrapicsScene::SetParams(double w, double h, uint8_t size, double t, double l)
{
    sq=w<h?w:h;
    this->w=w;this->h=h;this->t=t;this->l=l;this->size=size;
    fsize=sq/size;
}


