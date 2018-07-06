#include "boardview.h"
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
        dx=Ch->pos().x();
        dy=Ch->pos().y();
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
    if (cnt<0) cnt=movesTrack->Count;
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
            TrackMoveAnimation(Ch, X,Y,100,3000);
        }
        i++;
        movesTrack->SetToNext();
    }
}

BoardView::BoardView(QWidget *parent, uint8_t size, gameparams *params, ItemsList<CVItem> *CheckersList)
{
    this->CheckersList=CheckersList;
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene=new ChGrapicsScene (this);
    scene->params=params;
    this->setScene(scene);
    this->size=size;
}

BoardView::~BoardView()
{
}

void BoardView::Draw()

{
    foreach (auto it, scene->items()) {
        scene->removeItem(it);
        if (!((CVItem *) (it))->PtrCnt) ((CVItem *) (it))->deleteLater();
    }
    double height=this->height();
    double width=this->width();
    scene->SetParams(width,height,size);

    scene->setSceneRect(0,0,width,height);
    for (int i=0;i<size;i++)
    {
        for (int j=0;j<size;j++)
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
    item=NULL;
}
CVItem * BoardView::SetSelected(uint8_t pos)
{
    uint8_t i=pos%scene->size;u_int8_t j=pos/scene->size;
    CVItem * field=new CVItem(i,j,scene);
    field->setPixmap(*Pixmaps.PM_selectedFeild);
    field->setScale(scene->fsize/field->pixmap().width());
    field->setPos(scene->xy2pos(i,j));
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
    return NULL;
}

void BoardView::resizeEvent(QResizeEvent *event)
{
    Draw();
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
