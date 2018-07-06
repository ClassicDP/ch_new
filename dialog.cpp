#include "dialog.h"
#include "ui_dialog.h"
#include "checker_vis.h"
#include "game.h"
#include <QGraphicsPixmapItem>
#include <QTreeWidgetItem>

QGraphicsPixmapItem * px;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->game = new Game();
    board=new BoardView(parent, 8, &this->gameparam,game->CheckersList);
    ui->Layout->addWidget(board);
    board->Draw();
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    gameparam.IsEdit^=1;
    if (gameparam.IsEdit) ui->pushButton->setText("Редактор"); else ui->pushButton->setText("Игра");
}

void AddToTree (QList<QTreeWidgetItem*> *vList, ItemsList <MoveTreeItem> * tree)
{
    tree->SetToStart();
    while (tree->CurrentItem()) {
        QTreeWidgetItem *it=new QTreeWidgetItem;
        it->setText(0,tree->CurrentItem()->toString(8));
        vList->append(it);
        QList<QTreeWidgetItem*> *yy=new QList<QTreeWidgetItem*>;
        AddToTree(yy,tree->CurrentItem()->next);
        it->addChildren(*yy);
        tree->SetToNext();
    }
}

void Dialog::on_pushButton_2_clicked()
{
    pos = new Position (board->size,board->CheckersList,_white);
    GameFunctions gF(board->size);
    ItemsList <Position> * pList=new ItemsList <Position>;
    gF.MakeMovesList(pos, pList);
    pList->SetToStart();
    while (pList->CurrentItem())
    {
        ui->listWidget->addItem(pList->CurrentItem()->MoveAsStr());
        pList->SetToNext();
    }
    QList<QTreeWidgetItem*> *vList=new QList<QTreeWidgetItem*>;
    auto tree=PTreeMoves(pList);
    AddToTree(vList, tree.top);
    ui->treeWidget->addTopLevelItems(*vList);
    board->PTree= new PTreeMoves(pList);
}


