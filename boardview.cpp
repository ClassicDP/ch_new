#include "checker_vis.h"
#include <QDebug>

void Animation::Pos(int i)
{
    Ch->setPos(xPos+i*dx,yPos+i*dy);
}
void Animation::Next()
{
    qDebug() << Ch->_X()<< Ch->_Y() << "finished!";
    delete timeLine;
    busy=false;
    _loop.exit();
}



void Animation::LineMoveAnimation(CVItem *Ch, int xPos, int yPos, int StepCount, int mSec)
{
    this->xPos=xPos;
    this->yPos=yPos;
    dx=Ch->pos().x();
    dy=Ch->pos().y();
    dx=-(xPos-dx)/StepCount;
    dy=-(yPos-dy)/StepCount;
    this->Ch=Ch;
    timeLine=new QTimeLine(mSec);
    timeLine->setFrameRange(0,StepCount);
    timeLine->setDirection(QTimeLine::Backward);
    connect(timeLine, SIGNAL( frameChanged(int)), this, SLOT(Pos(int)));
    connect(timeLine, SIGNAL(finished()),this,SLOT(Next()));
    busy=true;
    timeLine->start();
    _loop.exec();
}
void Animation::TrcNext()
{
    delete timeLine;
    _loop.exit();

}
void Animation::TrackMoveAnimation(CVItem *Ch, int xPos, int yPos, int StepCount, int mSec)
{

    this->xPos=xPos;
    this->yPos=yPos;
    dx=Ch->x();
    dy=Ch->y();
    dx=-(xPos-dx)/StepCount;
    dy=-(yPos-dy)/StepCount;
    this->Ch=Ch;
    timeLine=new QTimeLine(mSec);
    timeLine->setFrameRange(0,StepCount);
    timeLine->setDirection(QTimeLine::Backward);
    connect(timeLine, SIGNAL( frameChanged(int)), this, SLOT(Pos(int)));
    connect(timeLine, SIGNAL(finished()),this,SLOT(TrcNext()));
    busy=true;
    timeLine->start();
    _loop.exec();
}

void Animation::TrackAnimation(CVItem *Ch, ItemsList<MoveTreeItem> *movesTrack, int8_t from, int8_t cnt)
{
    if (cnt<0) cnt=int8_t(movesTrack->Count);
    if (from<0) from=0;
    movesTrack->SetToStart();
    int i=0;
    while (movesTrack->Curr && cnt)
    {
        if (i>=from)
        {
            cnt--;
            double X = Ch->Xpos(movesTrack->CurrentItem()->move->to);
            double Y = Ch->Ypos(movesTrack->CurrentItem()->move->to);
            TrackMoveAnimation(Ch, int(X),int(Y),100,3000);
        }
        i++;
        movesTrack->SetToNext();
    }
}

BoardView::BoardView(QWidget *parent, uint8_t size, Game *game, ItemsList<CVItem> *CheckersList)
{
    this->CheckersList=CheckersList;
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene=new ChGrapicsScene (this);
    this->game=game;
    this->setScene(scene);
    this->size=size;
}

BoardView::~BoardView()
{
}
#include <math.h>
void BoardView::Draw()

{
    foreach (auto it, scene->items()) {
        scene->removeItem(it);
        (reinterpret_cast<QObject *>(it))->deleteLater();
    }
    double height=this->height();
    double width=this->width();
    height=std::min(height,width);
    height=height-2*height/size/3;
    width=height;
    scene->SetParams(width,height,size);
    fW=uint8_t(scene->fsize/3);
    qDebug() << frameSize() << scene->fsize<< fW;
    scene->setSceneRect(-fW,-fW,width+2*fW,height+2*fW);
    QBrush brush(0xE7CBAB);
    scene->addRect(-fW, -fW, width+2*fW, height+2*fW, QPen(0xE7CBAB), brush);
    QPen pen(0x45413D);
    pen.setWidth(2);
    scene->addRect(-1, -1, width+2, height+2, pen);
    pen.setWidth(1);
    for (uint8_t i=0;i<size;i++)
    {
        QGraphicsTextItem *txt;
        txt=scene->addText(QString('0'+((white_down)?(size-i):(i+1))));
        txt->setPos(-fW,(i+0.3)*scene->fsize);
        txt =scene->addText(QString('0'+((white_down)?(size-i):(i+1))));
        txt->setPos(width,(i+0.3)*scene->fsize);
        txt =scene->addText(QString('A'+((white_down)?i:(size-i-1))));
        txt->setPos((i+0.3)*scene->fsize, -fW*1.3);
        txt =scene->addText(QString('A'+((white_down)?i:(size-i-1))));

        txt->setPos((i+0.3)*scene->fsize, height-fW*0.2);

        for (uint8_t j=0;j<size;j++)
        {
            if ((i+j)%2)
            {
                CVItem * field=new CVItem(i,size-j-1,scene);
                field->setPixmap(Pixmaps.FieldBlack());
                field->setScale(scene->fsize/field->pixmap().width());
                field->setPos(i*scene->fsize,j*scene->fsize);
                scene->addItem(field);
            }
            else
            {
                CVItem * field=new CVItem(i,size-j-1,scene);
                field->setPixmap(Pixmaps.FieldWhite());
                field->setScale(scene->fsize/field->pixmap().width());
                field->setPos(i*scene->fsize,j*scene->fsize);
                scene->addItem(field);
            }
        }

    }
    CheckersList->SetToStart();
    while (CheckersList->CurrentItem())
    {
        auto * ch=CheckersList->CurrentItem();
        ch->UpdatePixmap();
        scene->addItem(ch);
        CheckersList->SetToNext();
    }
}

CVItem * BoardView::SetPoint(uint8_t pos)
{
    uint8_t i=pos%scene->size;u_int8_t j=pos/scene->size;
    CVItem * field=new CVItem(i,j,scene);
    field->setPixmap(*Pixmaps.PM_pointMove);
    field->setScale(scene->fsize/field->pixmap().width());
    field->setPos(i*scene->fsize,(size-j-1)*scene->fsize);
    scene->addItem(field);
    return field;
}

void BoardView::DeletePoint(CVItem  *item)
{
    delete item;
    item=nullptr;
}
CVItem * BoardView::SetSelected(uint8_t pos)
{
    uint8_t i=pos%scene->size;u_int8_t j=pos/scene->size;
    CVItem * field=new CVItem(i,j,scene);
    field->setPixmap(*Pixmaps.PM_selectedFeild);
    field->setScale(scene->fsize/field->pixmap().width());
    field->setPos(scene->xy2pos(i,j).x(), scene->xy2pos(i,j).y());
    scene->addItem(field);
    return field;
}

void BoardView::DeleteSelected(CVItem *item)
{
    scene->removeItem(item);
}

CVItem *BoardView::ChSearch(uint8_t pos)
{
    CheckersList->SetToStart();
    while (CheckersList->Curr) {
        if (CheckersList->CurrentItem()->_pos==pos)
            return CheckersList->CurrentItem();
        CheckersList->SetToNext();
    }
    return nullptr;
}

void BoardView::resizeEvent(QResizeEvent *event)
{
    Draw();
}

void BoardView::start_pos()
{
    CheckersList->ClearLate();
    int rows=size/2;
    for (uint8_t i=0;i<size;i++)
        for (uint8_t j=0;j<size;j++)
        {
            if ((i+j)%2) continue;
            if (i<(rows-1)) CheckersList->AddItem(new CVItem(new BoardItem (_white,_simple,j,i,size),scene));
            if (i>rows) CheckersList->AddItem(new CVItem(new BoardItem (_black,_simple,j,i,size),scene));
        }
}



CheckersPixmaps::CheckersPixmaps()
{
    pixmaps[_black][_simple][_on]=
            QPixmap(":/new/prefix1/res/Шашка 1.svg");
    pixmaps[_white][_simple][_on]=
            QPixmap(":/new/prefix1/res/Шашка 2.svg");
    pixmaps[_black][_king][_on]=
            QPixmap(":/new/prefix1/res/Черная дамка.svg");
    pixmaps[_white][_king][_on]=
            QPixmap(":/new/prefix1/res/Белая дамка.svg");
    pixmaps[_black][_simple][_off]=
            QPixmap(":/new/prefix1/res/Шашка 1 битая.svg");
    pixmaps[_white][_simple][_off]=
            QPixmap(":/new/prefix1/res/Шашка 2 битая.svg");
    pixmaps[_black][_king][_off]=
            QPixmap(":/new/prefix1/res/Черная дамка битая.svg");
    pixmaps[_white][_king][_off]=
            QPixmap(":/new/prefix1/res/Белая дамка битая.svg");
    PM_fieldBlack=new QPixmap(":/new/prefix1/res/Клетка черная.svg");
    PM_fieldWhite=new QPixmap(":/new/prefix1/res/Клетка белая.svg");
    PM_pointMove=new QPixmap(":/new/prefix1/res/Отметка 2.svg");
    PM_selectedFeild=new QPixmap(":/new/prefix1/res/select.svg");
}

QPixmap CheckersPixmaps::pix(uint8_t color, uint8_t type, uint8_t state)
{
    return pixmaps[color][type][state];
}

QPixmap CheckersPixmaps::FieldBlack()
{
    return *this->PM_fieldBlack;
}

QPixmap CheckersPixmaps::FieldWhite()
{
    return *this->PM_fieldWhite;
}

CheckersPixmaps::~CheckersPixmaps()
{
    delete PM_fieldBlack;delete PM_fieldWhite;
    delete PM_pointMove; delete PM_selectedFeild;
}
