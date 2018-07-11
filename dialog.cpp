#include "dialog.h"
#include "ui_dialog.h"
#include "checker_vis.h"
#include "game.h"
#include <QGraphicsPixmapItem>
#include <QTreeWidgetItem>
 #define _size 8
QGraphicsPixmapItem * px;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    game = new Game(_size);
    board=new BoardView(parent, _size, game, game->CheckersList);
    ui->Layout->addWidget(board);
    board->Draw();
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    game->IsEdit^=1;
    if (game->IsEdit)
    {
        ui->pushButton->setText("Редактор");
        ui->pushButton_3->setText("Начальная");
    }
    else
    {
        ui->pushButton->setText("Игра");
        ui->pushButton_3->setText("Повернуть доску");
    }
}

void AddToTree (QList<QTreeWidgetItem*> *vList, ItemsList <MoveTreeItem> * tree)
{
    tree->SetToStart();
    while (tree->CurrentItem()) {
        QTreeWidgetItem *it=new QTreeWidgetItem;
        it->setText(0,tree->CurrentItem()->toString(_size));
        vList->append(it);
        QList<QTreeWidgetItem*> *yy=new QList<QTreeWidgetItem*>;
        AddToTree(yy,tree->CurrentItem()->next);
        it->addChildren(*yy);
        tree->SetToNext();
    }
}

void Dialog::on_pushButton_2_clicked()
{
    ui->listWidget->clear();
    game->next_move_list();
    game->pList->SetToStart();
    while (game->pList->CurrentItem())
    {
        ui->listWidget->addItem(game->pList->CurrentItem()->MoveAsStr());
        game->pList->SetToNext();
    }
    QList<QTreeWidgetItem*> *vList=new QList<QTreeWidgetItem*>;
    auto tree=PTreeMoves(game->pList);
    AddToTree(vList, tree.top);
    ui->treeWidget->clear();
    ui->treeWidget->addTopLevelItems(*vList);
    board->PTree= new PTreeMoves(game->pList);
}



void Dialog::on_pushButton_3_clicked()
{
    if (game->IsEdit)
    {
        board->start_pos();
        board->Draw();
    }
    else
    {
        board->white_down^=1;
        board->Draw();
    }
}
