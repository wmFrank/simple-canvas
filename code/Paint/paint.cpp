#include "paint.h"
#include "ui_paint.h"
#include <QBoxLayout>

Paint::Paint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Paint)
{
    ui->setupUi(this);
    setWindowTitle("Paint Demo");
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::lightGray);  //改变控件背景色
    this->setPalette(palette);

    toolbar1 = new QToolBar(tr("图元"),this);
    QActionGroup *toolbar1_actiongroup1 = new QActionGroup(toolbar1);
    QAction *toolbar1_action1 = new QAction(QIcon(tr(":/icons/resource/Fill.png")),tr("填充"),toolbar1_actiongroup1);
    QAction *toolbar1_action2 = new QAction(QIcon(tr(":/icons/resource/Line.png")),tr("直线"),toolbar1_actiongroup1);
    QAction *toolbar1_action3 = new QAction(QIcon(tr(":/icons/resource/Circle.png")),tr("圆"),toolbar1_actiongroup1);
    QAction *toolbar1_action4 = new QAction(QIcon(tr(":/icons/resource/Oval.png")),tr("椭圆"),toolbar1_actiongroup1);
    QAction *toolbar1_action5 = new QAction(QIcon(tr(":/icons/resource/Polygon.png")),tr("多边形"),toolbar1_actiongroup1);
    QAction *toolbar1_action6 = new QAction(QIcon(tr(":/icons/resource/Curve.png")),tr("曲线"),toolbar1_actiongroup1);
    toolbar1_action1->setData(1);
    toolbar1_action2->setData(2);
    toolbar1_action3->setData(3);
    toolbar1_action4->setData(4);
    toolbar1_action5->setData(5);
    toolbar1_action6->setData(6);
    toolbar1->addActions(toolbar1_actiongroup1->actions());

    toolbar2 = new QToolBar(tr("辅助功能"),this);
    QActionGroup *toolbar2_actiongroup1 = new QActionGroup(toolbar2);
    QAction *toolbar2_action1 = new QAction(QIcon(tr(":/icons/resource/ClearScreen.png")),tr("清屏"),toolbar2_actiongroup1);
    QAction *toolbar2_action2 = new QAction(QIcon(tr(":/icons/resource/Save.png")),tr("保存图片"),toolbar2_actiongroup1);
    toolbar2_action1->setData(-1);
    toolbar2_action2->setData(0);
    toolbar2->addActions(toolbar2_actiongroup1->actions());

    toolbar3 = new QToolBar(tr("颜色选择器"),this);
    QActionGroup *toolbar3_actiongroup1 = new QActionGroup(toolbar3);
    QAction *black = new QAction(QIcon(tr(":/icons/resource/Black.png")),tr("黑"),toolbar3_actiongroup1);
    black->setData(QColor(0,0,0));
    QAction *white = new QAction(QIcon(tr(":/icons/resource/White.png")),tr("白"),toolbar3_actiongroup1);
    white->setData(QColor(255,255,255));
    QAction *blue = new QAction(QIcon(tr(":/icons/resource/Blue.png")),tr("蓝"),toolbar3_actiongroup1);
    blue->setData(QColor(0,0,255));
    QAction *darkblue = new QAction(QIcon(tr(":/icons/resource/DarkBlue.png")),tr("深蓝"),toolbar3_actiongroup1);
    darkblue->setData(QColor(0,0,139));
    QAction *lightsteelblue = new QAction(QIcon(tr(":/icons/resource/LightSteelBlue.png")),tr("浅钢蓝"),toolbar3_actiongroup1);
    lightsteelblue->setData(QColor(176,196,222));
    QAction *deepskyblue = new QAction(QIcon(tr(":/icons/resource/DeepSkyBlue.png")),tr("深天空蓝"),toolbar3_actiongroup1);
    deepskyblue->setData(QColor(0,191,255));
    QAction *darkcyan = new QAction(QIcon(tr(":/icons/resource/DarkCyan.png")),tr("深青"),toolbar3_actiongroup1);
    darkcyan->setData(QColor(0,139,139));
    QAction *springgreen = new QAction(QIcon(tr(":/icons/resource/SpringGreen.png")),tr("春绿"),toolbar3_actiongroup1);
    springgreen->setData(QColor(60,179,113));
    QAction *lawngreen = new QAction(QIcon(tr(":/icons/resource/LawnGreen.png")),tr("草绿"),toolbar3_actiongroup1);
    lawngreen->setData(QColor(124,252,0));
    QAction *yellow = new QAction(QIcon(tr(":/icons/resource/Yellow.png")),tr("黄"),toolbar3_actiongroup1);
    yellow->setData(QColor(255,255,0));
    QAction *gold = new QAction(QIcon(tr(":/icons/resource/Gold.png")),tr("金"),toolbar3_actiongroup1);
    gold->setData(QColor(255,215,0));
    QAction *peachpuff = new QAction(QIcon(tr(":/icons/resource/PeachPuff.png")),tr("桃红"),toolbar3_actiongroup1);
    peachpuff->setData(QColor(255,218,185));
    QAction *orangered = new QAction(QIcon(tr(":/icons/resource/OrangeRed.png")),tr("橘红"),toolbar3_actiongroup1);
    orangered->setData(QColor(255,69,0));
    QAction *maroon= new QAction(QIcon(tr(":/icons/resource/Maroon.png")),tr("栗红"),toolbar3_actiongroup1);
    maroon->setData(QColor(128,0,0));
    toolbar3->addActions(toolbar3_actiongroup1->actions());

    toolbar4 = new QToolBar(tr("变换选项"),this);
    QActionGroup *toolbar4_actiongroup1 = new QActionGroup(toolbar4);
    QAction *toolbar4_action1 = new QAction(QIcon(tr(":/icons/resource/Translate.png")),tr("平移"),toolbar4_actiongroup1);
    QAction *toolbar4_action2 = new QAction(QIcon(tr(":/icons/resource/Rotate.png")),tr("旋转"),toolbar4_actiongroup1);
    QAction *toolbar4_action3 = new QAction(QIcon(tr(":/icons/resource/Edit.png")),tr("编辑"),toolbar4_actiongroup1);
    QAction *toolbar4_action4 = new QAction(QIcon(tr(":/icons/resource/Cut.png")),tr("裁剪"),toolbar4_actiongroup1);
    toolbar4_action1->setData(2);
    toolbar4_action2->setData(3);
    toolbar4_action3->setData(4);
    toolbar4_action4->setData(5);
    toolbar4->addActions(toolbar4_actiongroup1->actions());

    drawwidget1 = new DrawWidget(this);

    QVBoxLayout *vboxlayout1 = new QVBoxLayout(this);
    vboxlayout1->addWidget(drawwidget1);
    vboxlayout1->addWidget(toolbar3);
    vboxlayout1->addWidget(toolbar1);
    vboxlayout1->addWidget(toolbar4);
    vboxlayout1->addWidget(toolbar2);
    this->setLayout(vboxlayout1);

    connect(toolbar1_actiongroup1,SIGNAL(triggered(QAction *)),this->drawwidget1,SLOT(draw(QAction *)));
    connect(toolbar2_actiongroup1,SIGNAL(triggered(QAction *)),this->drawwidget1,SLOT(draw(QAction *)));
    connect(this->drawwidget1,SIGNAL(draw_shape(int)),this->drawwidget1,SLOT(setChoose(int)));
    connect(toolbar3_actiongroup1,SIGNAL(triggered(QAction *)),this->drawwidget1,SLOT(pick(QAction *)));
    connect(this->drawwidget1,SIGNAL(pick_color(QColor)),this->drawwidget1,SLOT(setColor(QColor)));
    connect(toolbar4_actiongroup1,SIGNAL(triggered(QAction *)),this->drawwidget1,SLOT(transform(QAction *)));
    connect(this->drawwidget1,SIGNAL(transform_shape(int)),this->drawwidget1,SLOT(setMode(int)));
}

Paint::~Paint()
{
    delete ui;
    delete toolbar1;
    delete toolbar2;
    delete toolbar3;
    delete toolbar4;
    delete drawwidget1;
}

