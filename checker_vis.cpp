#include "math.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimeLine>
#include "game.h"
#include "itemlist.h"
#include "boardview.h"
#include <QGraphicsView>
#include "checker_vis.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>


CVItem::CVItem(BoardItem * board_item, ChGrapicsScene *scene, QPixmap pix):BoardItem(*board_item)

{
    isField=false;
    this->board_item=board_item;
    this->scene=scene;
    setPixmap(pix);
    setScale(scene->fsize/pix.width());
    setPos(dX(),dY());
    selected=0;

}

CVItem::CVItem(BoardItem *board_item, ChGrapicsScene *scene):BoardItem(*board_item)
{
    isField=false;
    this->board_item=board_item;
    this->scene=scene;
    auto pix=scene->board->Pixmaps.pix(_color,_type, killed);
    setPixmap(pix);
    setScale(scene->fsize/pix.width());
    setPos(dX(),dY());
    selected=0;

}

CVItem::CVItem(uint8_t X, uint8_t Y, ChGrapicsScene *scene):BoardItem(X,Y, scene->size)
{
    isField=true;
    this->scene=scene;
}

void CVItem::UpdatePixmap()
{
    CheckersPixmaps Pix;
    setPixmap(Pix.pixmaps[_color][_type][killed]);
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
    if (scene->params->IsEdit)
    {
        if (!this->isField)
        {
            auto * i=scene->board->ChSearch(_pos);
            if (i)
            {
                int x=_color+2*_type;
                x++;
                if (x==4)
                {
                    auto xx=scene->board->CheckersList->searchIt(i);
                    scene->removeItem(xx->It);
                    scene->board->CheckersList->DeleteLate(xx);
                } else
                {
                    if (x&1) i->_color=_white;
                    else i->_color=_black;
                    if ((x/2)&1) i->_type=_king;
                    else i->_type=_simple;
                    i->UpdatePixmap();
                }
                scene->board->Draw();
            }
        } else if (!((_X()+_Y())%2))
        {
            auto chL = scene->board->CheckersList;
            bool res=false;
            for (int i=0;i<chL->Count;i++) res|=(chL->At(i)->It->_X()==_X()) && (chL->At(i)->It->_Y()==_Y());
            if (!res)
            {
                scene->board->CheckersList->AddItem(new CVItem(
                                                        new BoardItem(_black,_simple,_X(),_Y(), scene->size),scene));
                scene->addItem(scene->board->CheckersList->Last->It);
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
    if (scene->params->IsEdit) return;
    // in Board check
    double dx=mapToScene(event->pos()).x()-scene->board->ChAnimation->mxPos+dX();
    double dy=mapToScene(event->pos()).y()-scene->board->ChAnimation->myPos+dY();
    dx=dx<0?0:dx;dx=dx>scene->sq-scene->fsize?scene->sq-scene->fsize:dx;
    dy=dy<0?0:dy;dy=dy>scene->sq-scene->fsize?scene->sq-scene->fsize:dy;
    setPos(dx,dy);

    this->setZValue(1);

    //QGraphicsItem::mouseMoveEvent(event);
}
void CVItem::TrackLoop()
{
    if (scene->board->ChAnimation->busy) return;
    int trkcnt;
    if (scene->board->treemoves->movesTrack)
        trkcnt=scene->board->treemoves->movesTrack->Count;
    else trkcnt=0;
    ItemsList <MoveTreeItem> * treeTop;
    treeTop=scene->board->treemoves->
            MoveVariantsFromPos(_pos);
    if (treeTop)
    {
        treeTop->SetToStart();
        while (treeTop->Curr)
        {
            scene->board->last_points->
                    AddItem(scene->board->SetPoint(treeTop->CurrentItem()->move->to));
            treeTop->SetToNext();
        }
    }
    if (!(trkcnt<scene->board->treemoves->movesTrack->Count) &&
            (scene->board->last_points->Count==1)) treeTop=scene->board
            ->treemoves->MoveVariantsFromPos(scene->board->last_points->First->It->Pos());
    if (trkcnt<scene->board->treemoves->movesTrack->Count)
    {
        scene->board->last_points->SetToStart();
        while (scene->board->last_points->Curr)
        {
            scene->removeItem(scene->board->last_points->CurrentItem());
            scene->board->last_points->SetToNext();
        }
        scene->board->last_points->ClearLate();

        auto xx=scene->board->treemoves->movesTrack->Last->It->move;
        auto ChX= scene->board->ChSearch(xx->from);
        scene->board->ChAnimation->LineMoveAnimation
                (ChX,ChX->Xpos(xx->to), ChX->Ypos(xx->to), 100, 700);
        qDebug () << "animation start"<< ChX->_X() << ChX->_Y();
        ChX->_pos=xx->to;
        if (is_king(ChX->_color, scene->board, xx->to))
        {
            ChX->_type=_king;
            ChX->UpdatePixmap();
        }
        auto yy=(CVItem *) scene->board->ChSearch(xx->kill);
        yy->killed=_yes;
        yy->UpdatePixmap();
        foreach (QGraphicsItem * i, scene->items())
        {
            i->setZValue(0);
        }
        ChX->setZValue(1);
    }

}



ChGrapicsScene::ChGrapicsScene(BoardView * board):board(board){}

void ChGrapicsScene::SetParams(double w, double h, uint8_t size, double t, double l)
{
    sq=w<h?w:h;
    this->w=w;this->h=h;this->t=t;this->l-l;this->size=size;
    fsize=sq/size;
}


