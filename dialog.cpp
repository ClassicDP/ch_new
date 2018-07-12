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
    game = new Game(_size);
    board=new BoardView(parent, _size, game, game->CheckersList);
    game->board=board;
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


void Dialog::on_pushButton_2_clicked()
{
    game->next_move_list(ui);
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
