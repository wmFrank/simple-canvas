#ifndef PAINT_H
#define PAINT_H

#include <QWidget>
#include <QPainter>
#include <QToolBar>
#include <QMenuBar>
#include <QTabWidget>
#include <QLabel>
#include <QToolButton>
#include "drawwidget.h"


namespace Ui {
class Paint;
}

class Paint : public QWidget
{
    Q_OBJECT

public:
    explicit Paint(QWidget *parent = nullptr);
    ~Paint();

signals:

public slots:

private:
    Ui::Paint *ui;         //主UI
    QToolBar *toolbar1;    //图元工具栏
    QToolBar *toolbar2;    //辅助功能工具栏
    QToolBar *toolbar3;    //色盘工具栏
    QToolBar *toolbar4;    //图元变换工具栏
    DrawWidget *drawwidget1;   //画板类
};



#endif // PAINT_H
