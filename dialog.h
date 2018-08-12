#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include "itemlist.h"
#include "checker_vis.h"


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    int angle=0.00;

    ~Dialog();
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Dialog *ui;
    BoardView * board;
    Position * Pos;

};

#endif // DIALOG_H
