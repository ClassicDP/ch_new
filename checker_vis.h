#ifndef CHECKER_VIS_H
#define CHECKER_VIS_H
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QEventLoop>
#include <stdint.h>
#include <QTimeLine>
#include "game.h"
#include "itemlist.h"

enum checker_onboard {_on=0, _off=1};

class Animation:public QObject
{
    Q_OBJECT
    double xPos, yPos, dx,dy, sq;
    CVItem *Ch;

    QTimeLine *timeLine;
public:
    bool busy=false;
    QEventLoop _loop;
    double mxPos, myPos; //mouse position on event
    void LineMoveAnimation (CVItem * Ch, int xPos, int yPos, int StepCount, int mSec);
    void TrackAnimation (CVItem *Ch, ItemsList<MoveTreeItem> *movesTrack,
                         int8_t from=-1, int8_t cnt=-1);
    void TrackMoveAnimation(CVItem *Ch, int xPos, int yPos, int StepCount, int mSec);
public slots:
    void Pos(int i);
    void Next();
    void TrcNext();
};

struct gameparams
{
    bool IsEdit=true;
};

class CheckersPixmaps
{
public:
    QPixmap pixmaps[2][2][2];// цвет, тип, статус на доске (обычная, битая, клетка)
    QPixmap *PM_fieldBlack, *PM_fieldWhite, *PM_pointMove, *PM_selectedFeild;
    CheckersPixmaps();
    QPixmap pix(uint8_t color, uint8_t type, uint8_t state);
    QPixmap FieldBlack();
    QPixmap FieldWhite();
    ~CheckersPixmaps();

};

class ChGrapicsScene;
class BoardView : public QGraphicsView
{
    Q_OBJECT

public:
    ChGrapicsScene *scene;
    ItemsList <CVItem>  *CheckersList;
    CheckersPixmaps Pixmaps;
    PTreeMoves *PTree;
    ItemsList <CVItem> * last_points=new ItemsList <CVItem>;
    Animation * ChAnimation= new Animation;
    uint8_t size;


    explicit BoardView(QWidget *parent = 0,  uint8_t size=8, gameparams * params=NULL, ItemsList<CVItem> *CheckersList=NULL);
    ~BoardView();
    void Draw();
    CVItem *SetPoint(uint8_t pos);
    void DeletePoint(CVItem * item);
    CVItem *SetSelected(uint8_t pos);
    void DeleteSelected(CVItem *item);
    CVItem *ChSearch(uint8_t pos);

    void resizeEvent(QResizeEvent *event);

};


class ChGrapicsScene:public QGraphicsScene
{
public:
    gameparams * params;
    BoardView * board;
    uint8_t size;
    double w,h,t,l;
    double fsize, sq;
    ChGrapicsScene (BoardView * board);
    inline QPointF  xy2pos(uint8_t x, uint8_t y)
    {
        return *new QPointF( x*fsize,(size-y-1)*fsize);

    }
    void SetParams (double w, double h, uint8_t size, double t=0, double l=0);
    ~ ChGrapicsScene()
    {

    }
};




class CVItem: public QObject, public QGraphicsPixmapItem, public BoardItem
{
    Q_OBJECT
    ChGrapicsScene * scene;
public:
    BoardView * board;
    ItemsList <MoveTreeItem> * tree=NULL;
    BoardItem *brd_it;

    bool isField;
    bool selected;
    CVItem (BoardItem * brd_it, ChGrapicsScene *scene, QPixmap pix);//checker Item
    CVItem (BoardItem * brd_it, ChGrapicsScene *scene);//checker Item
    CVItem (uint8_t dX, uint8_t dY, ChGrapicsScene *scene);// Field Item
    ~CVItem()
    {
        if (tree) delete tree;
    }
    void UpdatePixmap();
    double dX();
    double dY();
    double Xpos(uint8_t _pos);
    double Ypos(uint8_t _pos);
    uint8_t Pos();
public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void TrackLoop();
    void RefreshPoints(ItemsList <MoveTreeItem> * top, bool clear_only=false);
};


#endif


