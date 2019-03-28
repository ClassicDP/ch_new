#include "dialog.h"
#include "ui_dialog.h"
#include "checker_vis.h"
#include "game.h"

#define _size 8

QGraphicsPixmapItem * px;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    board=new BoardView(parent, _size);
    ui->Layout->addWidget(board);
    board->game = new Game(_size);
    //board->game->board=board;
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    board->game->IsEdit^=1;
    if (board->game->IsEdit)
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


void Dialog::on_pushButton_2_clicked()
{
//    for (auto it=board->CheckersList->begin();it;it=board->CheckersList->next())
//        board->game->CheckersList->AddItem(new Ch(it->_pos,it->_type,it->_color));
    board->game->next_move_list(ui, board);
}



void Dialog::on_pushButton_3_clicked()
{
    if (board->game->IsEdit)
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
