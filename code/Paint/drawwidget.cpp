#include "drawwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStack>
#include <QDebug>
#include <QInputDialog>
#include <algorithm>

//构造和析构函数
DrawWidget::DrawWidget(QWidget *parent) : QWidget(parent)
{
    int fixed_width = 1100;
    int fixed_height = 650;
    this->setMaximumSize(fixed_width,fixed_height);
    this->setMinimumSize(fixed_width,fixed_height);
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);  //改变控件背景色
    this->setPalette(palette);

    qimage = new QImage(fixed_width,fixed_height,QImage::Format_ARGB32);
    tmp_qimage = new QImage(fixed_width,fixed_height,QImage::Format_ARGB32);
    *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
    editframe = new QImage(fixed_width,fixed_height,QImage::Format_ARGB32);
    cutframe = new QImage(fixed_width,fixed_height,QImage::Format_ARGB32);
    p1 = QPoint(0,0);
    p2 = QPoint(0,0);
    a1 = QPoint(0,0);
    a2 = QPoint(0,0);
    b1 = QPoint(0,0);
    b2 = QPoint(0,0);
    c1 = QPoint(0,0);
    c2 = QPoint(0,0);
    d1 = QPoint(0,0);
    d2 = QPoint(0,0);
    mode = -1;
    choose = -1;
    mouse_state = -1;
    valid_dot = -1;
    angle = 0.0;
    current_color = QColor(0,0,0);
    tension = 0.6;
    list.clear();
    m.clear();
    MatrixGet();
    finish = false;
    dot_index = -1;
}

DrawWidget::~DrawWidget()
{
    delete qimage;
    delete tmp_qimage;
    delete editframe;
    delete cutframe;
    list.clear();
    m.clear();
}

//绘图事件
void DrawWidget::paintEvent(QPaintEvent *event)
{
    if(mode == 1)
    {
        if(choose == -1)
            tmp_qimage->fill(QColor(255,255,255));
        else if(choose == 1)
            SeadStuff(tmp_qimage,current_color,tmp_qimage->pixelColor(p2.x(),p2.y()),p2);
        else if(choose == 2)
            ddaLine(tmp_qimage,current_color,p1,p2);
        else if(choose == 3)
            BresenhamCircle(tmp_qimage,current_color,p1,p2);
        else if(choose == 4)
            DirectOval(tmp_qimage,current_color,p1,p2,0.0);
        else if(choose == 5)
        {
            for(int i = 0; i < list.size(); i++)
                FlagPoint(tmp_qimage,QColor(255,140,0),list[i]);
            if(list.size() >= 2)
                Polygon(tmp_qimage,current_color,list);
        }
        else if(choose == 6)
        {
            for(int i = 0; i < list.size(); i++)
                FlagPoint(tmp_qimage,QColor(255,0,0),list[i]);
            if(list.size() >= 2)
                CardinalCurve(tmp_qimage,current_color,list);
        }

        QPainter painter;
        painter.begin(this);
        painter.drawImage(QPoint(0,0),*tmp_qimage);
        painter.end();

        if(mouse_state == 3)
        {
            if(choose == 3)
            {
                int x1 = p1.x();
                int y1 = p1.y();
                int x2 = p2.x();
                int y2 = p2.y();
                if(x1 < x2 && y1 < y2)
                {
                    ;
                }
                else if(x1 >= x2 && y1 < y2)
                {
                    p1 = QPoint(x2,y1);
                    p2 = QPoint(x1,y2);
                }
                else if(x1 >= x2 && y1 >= y2)
                {
                    int delta = x1 - x2;
                    p1 = QPoint(x2,y1 - delta);
                    p2 = QPoint(x1,y1);
                }
                else
                {
                    int delta = x2 - x1;
                    p1 = QPoint(x1,y1 - delta);
                    p2 = QPoint(x2,y1);
                }
            }
            else if(choose == 4)
            {
                int x1 = p1.x();
                int y1 = p1.y();
                int x2 = p2.x();
                int y2 = p2.y();
                if(x1 < x2 && y1 < y2)
                {
                    ;
                }
                else if(x1 >= x2 && y1 < y2)
                {
                    p1 = QPoint(x2,y1);
                    p2 = QPoint(x1,y2);
                }
                else if(x1 >= x2 && y1 >= y2)
                {
                    p1 = QPoint(x2,y2);
                    p2 = QPoint(x1,y1);
                }
                else
                {
                    p1 = QPoint(x1,y2);
                    p2 = QPoint(x2,y1);
                }
            }
        }
    }
    else if(mode == 2)
    {
        Translation(tmp_qimage,a1,a2);
        QPainter painter;
        painter.begin(this);
        painter.drawImage(QPoint(0,0),*tmp_qimage);
        painter.end();
    }
    else if(mode == 3)
    {
        Rotation(tmp_qimage,b1,b2);
        QPainter painter;
        painter.begin(this);
        painter.drawImage(QPoint(0,0),*tmp_qimage);
        painter.end();
    }
    else if(mode == 4)
    {
        Editing(tmp_qimage,c1,c2);
        QPainter painter;
        painter.begin(this);
        painter.drawImage(QPoint(0,0),*tmp_qimage);
        if(choose == 3)
        {
            int d = p2.x() - p1.x();
            editframe->fill(QColor(255,255,255,0));
            if(valid_dot == -1)
            {
                QPoint pp2 = QPoint(p1.x()+d,p1.y()+d);
                EditFrame(editframe,p1,pp2);
            }
            else if(valid_dot == 1)
            {
                QPoint pp2 = QPoint(p1.x()+d,p1.y()+d);
                int delta = pp2.x() - c2.x();
                QPoint pp1 = QPoint(pp2.x()-delta,pp2.y()-delta);
                EditFrame(editframe,pp1,pp2);
            }
            else if(valid_dot == 2)
            {
                QPoint t = QPoint(p1.x(),p1.y()+d);
                int delta = c2.x() - t.x();
                QPoint pp1 = QPoint(t.x(),t.y()-delta);
                QPoint pp2 = QPoint(t.x()+delta,t.y());
                EditFrame(editframe,pp1,pp2);
            }
            else if(valid_dot == 3)
            {
                QPoint t = QPoint(p1.x()+d,p1.y());
                int delta = t.x() - c2.x();
                QPoint pp1 = QPoint(t.x()-delta,t.y());
                QPoint pp2 = QPoint(t.x(),t.y()+delta);
                EditFrame(editframe,pp1,pp2);
            }
            else if(valid_dot == 4)
            {
                QPoint pp1 = p1;
                int delta = c2.x() - pp1.x();
                QPoint pp2 = QPoint(pp1.x()+delta,pp1.y()+delta);
                EditFrame(editframe,pp1,pp2);
            }
            painter.drawImage(QPoint(0,0),*editframe);
        }
        else if(choose == 4)
        {
            editframe->fill(QColor(255,255,255,0));
            if(valid_dot == -1)
                EditFrame(editframe,p1,p2);
            else if(valid_dot == 1)
                EditFrame(editframe,c2,p2);
            else if(valid_dot == 2)
            {
                QPoint pp1 = QPoint(p1.x(),c2.y());
                QPoint pp2 = QPoint(c2.x(),p2.y());
                EditFrame(editframe,pp1,pp2);
            }
            else if(valid_dot == 3)
            {
                QPoint pp1 = QPoint(c2.x(),p1.y());
                QPoint pp2 = QPoint(p2.x(),c2.y());
                EditFrame(editframe,pp1,pp2);
            }
            else if(valid_dot == 4)
                EditFrame(editframe,p1,c2);
            painter.drawImage(QPoint(0,0),*editframe);
        }
        painter.end();
    }
    else if(mode == 5)
    {
        if(choose == 2)
        {
            if(mouse_state != 3)
            {
                ddaLine(tmp_qimage,current_color,p1,p2);
                cutframe->fill(QColor(255,255,255,0));
                Rectangle(cutframe,QColor(0,255,0),d1,d2);
                QPainter painter;
                painter.begin(this);
                painter.drawImage(QPoint(0,0),*tmp_qimage);
                painter.drawImage(QPoint(0,0),*cutframe);
                painter.end();
            }
            else
            {
                int x1 = d1.x();
                int y1 = d1.y();
                int x2 = d2.x();
                int y2 = d2.y();
                if(x1 < x2 && y1 < y2)
                {
                    ;
                }
                else if(x1 >= x2 && y1 < y2)
                {
                    d1 = QPoint(x2,y1);
                    d2 = QPoint(x1,y2);
                }
                else if(x1 >= x2 && y1 >= y2)
                {
                    d1 = QPoint(x2,y2);
                    d2 = QPoint(x1,y1);
                }
                else
                {
                    d1 = QPoint(x1,y2);
                    d2 = QPoint(x2,y1);
                }
                LiangBarskyLineCut(tmp_qimage,d1,d2);
                cutframe->fill(QColor(255,255,255,0));
                Rectangle(cutframe,QColor(0,255,0),d1,d2);
                QPainter painter;
                painter.begin(this);
                painter.drawImage(QPoint(0,0),*tmp_qimage);
                painter.drawImage(QPoint(0,0),*cutframe);
                painter.end();
            }
        }
        else if(choose == 3)
        {
            BresenhamCircle(tmp_qimage,current_color,p1,p2);
            QPainter painter;
            painter.begin(this);
            painter.drawImage(QPoint(0,0),*tmp_qimage);
            painter.end();
        }
        else if(choose == 4)
        {
            DirectOval(tmp_qimage,current_color,p1,p2,-angle);
            QPainter painter;
            painter.begin(this);
            painter.drawImage(QPoint(0,0),*tmp_qimage);
            painter.end();
        }
        else if(choose == 5)
        {
            for(int i = 0; i < list.size(); i++)
                FlagPoint(tmp_qimage,QColor(255,140,0),list[i]);
            if(list.size() >= 2)
                Polygon(tmp_qimage,current_color,list);
            QPainter painter;
            painter.begin(this);
            painter.drawImage(QPoint(0,0),*tmp_qimage);
            painter.end();
        }
        else if(choose == 6)
        {
            for(int i = 0; i < list.size(); i++)
                FlagPoint(tmp_qimage,QColor(255,0,0),list[i]);
            if(list.size() >= 2)
                CardinalCurve(tmp_qimage,current_color,list);
            QPainter painter;
            painter.begin(this);
            painter.drawImage(QPoint(0,0),*tmp_qimage);
            painter.end();
        }
    }
}

//鼠标事件
void DrawWidget::mousePressEvent(QMouseEvent *event)
{
    mouse_state = 1;
    if(event->button() == Qt::LeftButton)
    {
        if(mode == 1)
        {
            if(choose != 6 && choose != 5)
                *qimage = tmp_qimage->copy(0,0,tmp_qimage->width(),tmp_qimage->height());
            if(finish == false && choose == 5)
            {
                *qimage = tmp_qimage->copy(0,0,tmp_qimage->width(),tmp_qimage->height());
                finish = true;
            }
            if(finish == false && choose == 6)
            {
                *qimage = tmp_qimage->copy(0,0,tmp_qimage->width(),tmp_qimage->height());
                finish = true;
            }
            if(choose != -1)
                p1 = event->pos();
        }
        else if(mode == 2)
        {
            if(choose != -1)
                a1 = event->pos();
        }
        else if(mode == 3)
        {
            if(choose != -1)
                b1 = event->pos();
        }
        else if(mode == 4)
        {
            if(choose == 2)
            {
                c1 = event->pos();
                if((c1.x()-p1.x())*(c1.x() - p1.x())+(c1.y()-p1.y())*(c1.y()-p1.y()) <= 16)
                    valid_dot = 1;
                if((c1.x()-p2.x())*(c1.x() - p2.x())+(c1.y()-p2.y())*(c1.y()-p2.y()) <= 16)
                    valid_dot = 2;
            }
            else if(choose == 3)
            {
                c1 = event->pos();
                int d;
                QPoint pp1,pp2,pp3,pp4;
                int x1 = p1.x();
                int x2 = p2.x();
                d = x2 - x1;
                pp1 = p1;
                pp2 = QPoint(p1.x()+d,p1.y());
                pp3 = QPoint(p1.x(),p1.y()+d);
                pp4 = QPoint(p1.x()+d,p1.y()+d);
                if((c1.x()-pp1.x())*(c1.x() - pp1.x())+(c1.y()-pp1.y())*(c1.y()-pp1.y()) <= 16)
                {
                    valid_dot = 1;
                    c1 = pp1;
                }
                if((c1.x()-pp2.x())*(c1.x() - pp2.x())+(c1.y()-pp2.y())*(c1.y()-pp2.y()) <= 16)
                {
                    valid_dot = 2;
                    c1 = pp2;
                }
                if((c1.x()-pp3.x())*(c1.x() - pp3.x())+(c1.y()-pp3.y())*(c1.y()-pp3.y()) <= 16)
                {
                    valid_dot = 3;
                    c1 = pp3;
                }
                if((c1.x()-pp4.x())*(c1.x() - pp4.x())+(c1.y()-pp4.y())*(c1.y()-pp4.y()) <= 16)
                {
                    valid_dot = 4;
                    c1 = pp4;
                }
            }
            else if(choose == 4)
            {
                c1 = event->pos();
                QPoint pp1,pp2,pp3,pp4;
                pp1 = p1;
                pp2 = QPoint(p2.x(),p1.y());
                pp3 = QPoint(p1.x(),p2.y());
                pp4 = p2;
                if((c1.x()-pp1.x())*(c1.x() - pp1.x())+(c1.y()-pp1.y())*(c1.y()-pp1.y()) <= 16)
                {
                    valid_dot = 1;
                    c1 = pp1;
                }
                if((c1.x()-pp2.x())*(c1.x() - pp2.x())+(c1.y()-pp2.y())*(c1.y()-pp2.y()) <= 16)
                {
                    valid_dot = 2;
                    c1 = pp2;
                }
                if((c1.x()-pp3.x())*(c1.x() - pp3.x())+(c1.y()-pp3.y())*(c1.y()-pp3.y()) <= 16)
                {
                    valid_dot = 3;
                    c1 = pp3;
                }
                if((c1.x()-pp4.x())*(c1.x() - pp4.x())+(c1.y()-pp4.y())*(c1.y()-pp4.y()) <= 16)
                {
                    valid_dot = 4;
                    c1 = pp4;
                }
            }
            else if(choose == 5)
            {
                c1 = event->pos();
                for(int i = 0 ; i < list.size(); i++)
                {
                    if((c1.x()-list[i].x())*(c1.x()-list[i].x())+(c1.y()-list[i].y())*(c1.y()-list[i].y()) <= 16)
                    {
                        dot_index = i;
                        c1 = list[i];
                        break;
                    }
                }
            }
            else if(choose == 6)
            {
                c1 = event->pos();
                for(int i = 0 ; i < list.size(); i++)
                {
                    if((c1.x()-list[i].x())*(c1.x()-list[i].x())+(c1.y()-list[i].y())*(c1.y()-list[i].y()) <= 16)
                    {
                        dot_index = i;
                        c1 = list[i];
                        break;
                    }
                }
            }
        }
        else if(mode == 5)
        {
            if(choose != -1)
                d1 = event->pos();
        }
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouse_state = 2;
    if(event->buttons() & Qt::LeftButton)
    {
        if(mode == 1)
        {
            *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                p2 = event->pos();
            if(choose != 6 && choose != 5)
                update();
        }
        else if(mode == 2)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                a2 = event->pos();
            update();
        }
        else if(mode == 3)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                b2 = event->pos();
            update();
        }
        else if(mode == 4)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                c2 = event->pos();
            update();
        }
        else if(mode == 5)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                d2 = event->pos();
            update();
        }
    }
}

void DrawWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_state = 3;
    if(event->button() == Qt::LeftButton)
    {
        if(mode == 1)
        {
            *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                p2 = event->pos();
            if(choose == 6 || choose == 5)
                list.push_back(p2);
            update();
        }
        else if(mode == 2)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                a2 = event->pos();
            update();
        }
        else if(mode == 3)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                b2 = event->pos();
            update();
        }
        else if(mode == 4)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                c2 = event->pos();
            update();
        }
        else if(mode == 5)
        {
             *tmp_qimage = qimage->copy(0,0,qimage->width(),qimage->height());
            if(choose != -1)
                d2 = event->pos();
            update();
        }
    }
}

//各种算法
double DrawWidget::coordinate_x(double x, double y, int a, int b, double angle)
{
    x = x - a;
    y = y - b;
    x = x*cos(angle)-y*sin(angle) + a;
    return x;
}

double DrawWidget::coordinate_y(double x, double y, int a, int b, double angle)
{
    x = x - a;
    y = y - b;
    y = x*sin(angle)+y*cos(angle) + b;
    return y;
}

void DrawWidget::ddaLine(QImage *qimage, QColor pen, QPoint p1, QPoint p2)
{
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    double dx,dy,e,x,y;
    dx=x2-x1;
    dy=y2-y1;
    e=(fabs(dx)>fabs(dy))?fabs(dx):fabs(dy);
    dx/=e;
    dy/=e;
    x=x1;
    y=y1;
    for(int i=1;i<=e;i++)
    {
        qimage->setPixelColor((int)(x+0.5),(int)(y+0.5),pen);
        x+=dx;
        y+=dy;
    }
}

void DrawWidget::BresenhamCircle(QImage *qimage, QColor pen, QPoint p1, QPoint p2)
{
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    int r = 0,center_x = 0,center_y = 0;
    if(x1 < x2 && y1 < y2)
    {
        r = (x2 - x1) / 2;
        center_x = x1 + r;
        center_y = y1 + r;
    }
    else if(x1 >= x2 && y1 < y2)
    {
        r = (x1 - x2) / 2;
        center_x = x1 - r;
        center_y = y1 + r;
    }
    else if(x1 >= x2 && y1 >= y2)
    {
        r = (x1 - x2) / 2;
        center_x = x1 - r;
        center_y = y1 - r;
    }
    else
    {
        r = (x2 - x1) / 2;
        center_x = x1 + r;
        center_y = y1 - r;
    }
    int x, y, p;
    x = 0;
    y = r;
    p = 3 - 2 * r;
    for(; x <= y; x++)
    {
        qimage->setPixelColor(x + center_x, y + center_y,pen);
        qimage->setPixelColor(x + center_x, - y + center_y,pen);
        qimage->setPixelColor(- x + center_x, y + center_y,pen);
        qimage->setPixelColor(- x + center_x, - y + center_y,pen);
        qimage->setPixelColor(y + center_x, x + center_y,pen);
        qimage->setPixelColor(y + center_x, - x + center_y,pen);
        qimage->setPixelColor(- y + center_x, x + center_y,pen);
        qimage->setPixelColor(- y + center_x, - x + center_y,pen);
        if(p >= 0)
        {
            p += 4 * (x - y) + 10;
            y--;
        }
        else
        {
            p += 4 * x + 6;
        }
    }
}

void DrawWidget::BresenhamOval(QImage *qimage, QColor pen, QPoint p1, QPoint p2)
{
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    int a = 0,b = 0,center_x = 0,center_y = 0;
    if(x1 < x2 && y1 < y2)
    {
        a = (x2 - x1) / 2;
        b = (y2 - y1) / 2;
        center_x = x1 + a;
        center_y = y1 + b;
    }
    else if(x1 >= x2 && y1 < y2)
    {
        a = (x1 - x2) / 2;
        b = (y2 - y1) / 2;
        center_x = x1 - a;
        center_y = y1 + b;
    }
    else if(x1 >= x2 && y1 >= y2)
    {
        a = (x1 - x2) / 2;
        b = (y1 - y2) / 2;
        center_x = x1 - a;
        center_y = y1 - b;
    }
    else
    {
        a = (x2 - x1) / 2;
        b = (y1 - y2) / 2;
        center_x = x1 + a;
        center_y = y1 - b;
    }
    int x, y, p;
    x = 0;
    y = b;
    p = 2 * b * b - 2 * b * a * a + a * a;
    int key_p = round((double)(a * a) / sqrt((double)(a * a + b * b)));
    for(; x <= key_p; x++)
    {
        qimage->setPixelColor(x + center_x, y + center_y,pen);
        qimage->setPixelColor(x + center_x, - y + center_y,pen);
        qimage->setPixelColor(- x + center_x, y + center_y,pen);
        qimage->setPixelColor(- x + center_x, - y + center_y,pen);
        if(p >= 0)
        {
            p += 2 * b * b * (2 * x + 3) - 4 * a * a * (y - 1);
            y--;
        }
        else
        {
            p += 2 * b * b * (2 * x + 3);
        }
    }
    p = b * b * (x * x + x) + a * a * (y * y - y) - a * a * b * b;
    for(; y >= 0; y--)
    {
        qimage->setPixelColor(x + center_x, y + center_y,pen);
        qimage->setPixelColor(x + center_x, - y + center_y,pen);
        qimage->setPixelColor(- x + center_x, y + center_y,pen);
        qimage->setPixelColor(- x + center_x, - y + center_y,pen);
        if(p >= 0)
        {
            p = p - 2 * a * a * y - a * a;
        }
        else
        {
            x++;
            p = p - 2 * a * a * y - a * a + 2 * b * b * x + 2 * b * b;
        }
    }
}

void DrawWidget::DirectOval(QImage *qimage, QColor pen, QPoint p1, QPoint p2, double angle)
{
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    int a = 0,b = 0,center_x = 0,center_y = 0;
    if(x1 < x2 && y1 < y2)
    {
        a = (x2 - x1) / 2;
        b = (y2 - y1) / 2;
        center_x = x1 + a;
        center_y = y1 + b;
    }
    else if(x1 >= x2 && y1 < y2)
    {
        a = (x1 - x2) / 2;
        b = (y2 - y1) / 2;
        center_x = x1 - a;
        center_y = y1 + b;
    }
    else if(x1 >= x2 && y1 >= y2)
    {
        a = (x1 - x2) / 2;
        b = (y1 - y2) / 2;
        center_x = x1 - a;
        center_y = y1 - b;
    }
    else
    {
        a = (x2 - x1) / 2;
        b = (y1 - y2) / 2;
        center_x = x1 + a;
        center_y = y1 - b;
    }
    double th = acos(-1) / 180;
    double i;
    double x,y,tx,ty;
    for(i=-180;i<=180;i=i+0.5)
    {
        x=a*cos(i*th);
        y=b*sin(i*th);
        tx=x;
        ty=y;
        x=tx*cos(angle)-ty*sin(angle)+center_x;  /*坐标旋转*/
        y=center_y-(ty*cos(angle)+tx*sin(angle));
        qimage->setPixelColor((int)x, (int)y,pen);
        qimage->setPixelColor((int)(x+1), (int)y,pen);
        qimage->setPixelColor((int)x, (int)(y+1),pen);
        qimage->setPixelColor((int)(x-1), (int)y,pen);
        qimage->setPixelColor((int)x, (int)(y-1),pen);
    }
}

void DrawWidget::Rectangle(QImage *qimage, QColor pen, QPoint p1, QPoint p2)
{
    QPoint p3 = QPoint(p1.x(),p2.y());
    QPoint p4 = QPoint(p2.x(),p1.y());
    ddaLine(qimage,pen,p1,p3);
    ddaLine(qimage,pen,p1,p4);
    ddaLine(qimage,pen,p3,p2);
    ddaLine(qimage,pen,p4,p2);
}

void DrawWidget::Polygon(QImage *qimage, QColor pen, vector<QPoint> list)
{
    for(int i = 1; i < list.size(); i++)
    {
        ddaLine(qimage,pen,list[i-1],list[i]);
    }
    ddaLine(qimage,pen,list[list.size()-1],list[0]);
}

void DrawWidget::SeadStuff(QImage *qimage, QColor pen, QColor stuffcolor, QPoint p2)
{
    int x1 = p2.x();
    int y1 = p2.y();
    if(pen == stuffcolor)
        return;
    QStack<QPoint> *qstack = new QStack<QPoint>();
    int i = x1;
    for(; i >= 0; i--)
    {
        QColor t = qimage->pixelColor(i,y1);
        if(t != stuffcolor)
            break;
    }
    i = i + 1;
    qstack->push(QPoint(i,y1));
    while(!qstack->empty())
    {
        QPoint p = qstack->pop();
        int len = StuffLine(qimage,pen,stuffcolor,p);
        PushStack(qimage,pen,stuffcolor,p,qstack,len);
    }
}

int DrawWidget::StuffLine(QImage *qimage, QColor pen, QColor stuffcolor, QPoint p)
{
    int len = 0;
    for(int i = p.x(); i <= 1100; i++)
    {
        QColor t = qimage->pixelColor(i,p.y());
        if(t == stuffcolor)
        {
            qimage->setPixelColor(i,p.y(),pen);
            len++;
        }
        else
        {
            break;
        }
    }
    return len;
}

void DrawWidget::PushStack(QImage *qimage, QColor pen, QColor stuffcolor, QPoint p, QStack<QPoint> *qstack, int len)
{
    int x = p.x();
    int y1 = p.y() - 1;
    QColor q = qimage->pixelColor(x,y1);
    if(q == stuffcolor)
    {
        int i = x;
        for(; i >= 0; i--)
        {
            QColor t = qimage->pixelColor(i,y1);
            if(t != stuffcolor)
                break;
        }
        i = i + 1;
        qstack->push(QPoint(i,y1));
        bool flag = false;
        for(int j = x; j < x + len; j++)
        {
            QColor t = qimage->pixelColor(j,y1);
            if(t == stuffcolor && flag == false)
            {
                ;
            }
            else if(t == stuffcolor && flag == true)
            {
                qstack->push_back(QPoint(j,y1));
                flag = false;
            }
            else if(t != stuffcolor && flag == false)
            {
                flag = true;
            }
            else
            {
                ;
            }
        }
    }
    else
    {
        bool flag = true;
        for(int j = x; j < x + len; j++)
        {
            QColor t = qimage->pixelColor(j,y1);
            if(t == stuffcolor && flag == false)
            {
                ;
            }
            else if(t == stuffcolor && flag == true)
            {
                qstack->push_back(QPoint(j,y1));
                flag = false;
            }
            else if(t != stuffcolor && flag == false)
            {
                flag = true;
            }
            else
            {
                ;
            }
        }
    }

    int y2 = p.y() + 1;
    QColor s = qimage->pixelColor(x,y2);
    if(s == stuffcolor)
    {
        int i = x;
        for(; i >= 0; i--)
        {
            QColor t = qimage->pixelColor(i,y2);
            if(t != stuffcolor)
                break;
        }
        i = i + 1;
        qstack->push(QPoint(i,y2));
        bool flag = false;
        for(int j = x; j < x + len; j++)
        {
            QColor t = qimage->pixelColor(j,y2);
            if(t == stuffcolor && flag == false)
            {
                ;
            }
            else if(t == stuffcolor && flag == true)
            {
                qstack->push_back(QPoint(j,y2));
                flag = false;
            }
            else if(t != stuffcolor && flag == false)
            {
                flag = true;
            }
            else
            {
                ;
            }
        }
    }
    else
    {
        bool flag = true;
        for(int j = x; j < x + len; j++)
        {
            QColor t = qimage->pixelColor(j,y2);
            if(t == stuffcolor && flag == false)
            {
                ;
            }
            else if(t == stuffcolor && flag == true)
            {
                qstack->push_back(QPoint(j,y2));
                flag = false;
            }
            else if(t != stuffcolor && flag == false)
            {
                flag = true;
            }
            else
            {
                ;
            }
        }
    }
}

void DrawWidget::Translation(QImage *qimage, QPoint a1, QPoint a2)
{
    int delta_x = a2.x() - a1.x();
    int delta_y = a2.y() - a1.y();
    QPoint delta = QPoint(delta_x,delta_y);
    QPoint p3 = p1 + delta;
    QPoint p4 = p2 + delta;
    vector<QPoint> tmp;
    tmp.clear();
    for(int i = 0; i < list.size(); i++)
        tmp.push_back((list[i] + delta));
    if(choose == 2)
        ddaLine(qimage,current_color,p3,p4);
    else if(choose == 3)
        BresenhamCircle(qimage,current_color,p3,p4);
    else if(choose == 4)
        DirectOval(qimage,current_color,p3,p4,-angle);
    else if(choose == 5)
    {
        for(int i = 0; i < tmp.size(); i++)
            FlagPoint(qimage,QColor(255,140,0),tmp[i]);
        if(tmp.size() >= 2)
            Polygon(qimage,current_color,tmp);
    }
    else if(choose == 6)
    {
        for(int i = 0; i < tmp.size(); i++)
            FlagPoint(qimage,QColor(255,0,0),tmp[i]);
        if(tmp.size() >= 2)
            CardinalCurve(qimage,current_color,tmp);
    }

    if(mouse_state == 3)
    {
        p1 = p3;
        p2 = p4;
        list.clear();
        for(int i = 0; i < tmp.size(); i++)
            list.push_back(tmp[i]);
    }
}

void DrawWidget::Rotation(QImage *qimage, QPoint b1, QPoint b2)
{
    int center_x = (p1.x()+p2.x())/2;
    int center_y = (p1.y()+p2.y())/2;
    double x = b2.x() - center_x;
    double y = b2.y() - center_y;
    angle = atan(y/x);
    if(choose == 2)
    {
        double len = sqrt(pow((p1.x()-p2.x()),2) + pow(p1.y()-p2.y(),2));
        double p1x = center_x + len/2;
        double p1y = center_y;
        //qDebug() << "bbb " <<len << " " << p1x << " " << p1y << endl;
        int x1 = (int)(coordinate_x(p1x,p1y,center_x,center_y,angle));
        int y1 = (int)(coordinate_y(p1x,p1y,center_x,center_y,angle));
        int x2 = 2 * center_x - x1;
        int y2 = 2 * center_y - y1;
        //qDebug() << "ccc " << x1 <<" " << y1 <<" "<< x2 <<" " << y2 << endl;
        QPoint pp1 = QPoint(x1,y1);
        QPoint pp2 = QPoint(x2,y2);
        ddaLine(qimage,current_color,pp1,pp2);
        if(mouse_state == 3)
        {
            p1 = pp1;
            p2 = pp2;
        }
    }
    else if(choose == 3)
    {
        BresenhamCircle(qimage,current_color,p1,p2);
    }
    else if(choose == 4)
    {
        DirectOval(qimage,current_color,p1,p2,-angle);
    }
    else if(choose == 5)
    {
        for(int i = 0; i < list.size(); i++)
            FlagPoint(qimage,QColor(255,140,0),list[i]);
        if(list.size() >= 2)
            Polygon(qimage,current_color,list);
    }
    else if(choose == 6)
    {
        for(int i = 0; i < list.size(); i++)
            FlagPoint(qimage,QColor(255,0,0),list[i]);
        if(list.size() >= 2)
            CardinalCurve(qimage,current_color,list);
    }
}

void DrawWidget::Editing(QImage *qimage, QPoint c1, QPoint c2)
{
    if(choose == 2)
    {
        if(valid_dot == -1)
            ddaLine(qimage,current_color,p1,p2);
        else if(valid_dot == 1)
            ddaLine(qimage,current_color,c2,p2);
        else if(valid_dot == 2)
            ddaLine(qimage,current_color,p1,c2);
        if(mouse_state == 3)
        {
            if(valid_dot == 1)
                p1 = c2;
            else if(valid_dot == 2)
                p2 = c2;
            valid_dot = -1;
        }
    }
    else if(choose == 3)
    {
        int d = p2.x() - p1.x();
        if(valid_dot == -1)
            BresenhamCircle(qimage,current_color,p1,p2);
        else if(valid_dot == 1)
        {
            QPoint pp2 = QPoint(p1.x()+d,p1.y()+d);
            int delta = pp2.x() - c2.x();
            QPoint pp1 = QPoint(pp2.x()-delta,pp2.y()-delta);
            BresenhamCircle(qimage,current_color,pp1,pp2);
        }
        else if(valid_dot == 2)
        {
            QPoint t = QPoint(p1.x(),p1.y()+d);
            int delta = c2.x() - t.x();
            QPoint pp1 = QPoint(t.x(),t.y()-delta);
            QPoint pp2 = QPoint(t.x()+delta,t.y());
            BresenhamCircle(qimage,current_color,pp1,pp2);
        }
        else if(valid_dot == 3)
        {
            QPoint t = QPoint(p1.x()+d,p1.y());
            int delta = t.x() - c2.x();
            QPoint pp1 = QPoint(t.x()-delta,t.y());
            QPoint pp2 = QPoint(t.x(),t.y()+delta);
            BresenhamCircle(qimage,current_color,pp1,pp2);
        }
        else if(valid_dot == 4)
        {
            QPoint pp1 = p1;
            int delta = c2.x() - pp1.x();
            QPoint pp2 = QPoint(pp1.x()+delta,pp1.y()+delta);
            BresenhamCircle(qimage,current_color,pp1,pp2);
        }
        if(mouse_state == 3)
        {
            if(valid_dot == 1)
            {
                QPoint pp2 = QPoint(p1.x()+d,p1.y()+d);
                int delta = pp2.x() - c2.x();
                QPoint pp1 = QPoint(pp2.x()-delta,pp2.y()-delta);
                p1 = pp1;
                p2 = pp2;
            }
            else if(valid_dot == 2)
            {
                QPoint t = QPoint(p1.x(),p1.y()+d);
                int delta = c2.x() - t.x();
                QPoint pp1 = QPoint(t.x(),t.y()-delta);
                QPoint pp2 = QPoint(t.x()+delta,t.y());
                p1 = pp1;
                p2 = pp2;
            }
            else if(valid_dot == 3)
            {
                QPoint t = QPoint(p1.x()+d,p1.y());
                int delta = t.x() - c2.x();
                QPoint pp1 = QPoint(t.x()-delta,t.y());
                QPoint pp2 = QPoint(t.x(),t.y()+delta);
                p1 = pp1;
                p2 = pp2;
            }
            else if(valid_dot == 4)
            {
                QPoint pp1 = p1;
                int delta = c2.x() - pp1.x();
                QPoint pp2 = QPoint(pp1.x()+delta,pp1.y()+delta);
                p1 = pp1;
                p2 = pp2;
            }
            valid_dot = -1;
        }
    }
    else if(choose == 4)
    {
        if(valid_dot == -1)
            DirectOval(qimage,current_color,p1,p2,-angle);
        else if(valid_dot == 1)
            DirectOval(qimage,current_color,c2,p2,-angle);
        else if(valid_dot == 2)
        {
            QPoint pp1 = QPoint(p1.x(),c2.y());
            QPoint pp2 = QPoint(c2.x(),p2.y());
            DirectOval(qimage,current_color,pp1,pp2,-angle);
        }
        else if(valid_dot == 3)
        {
            QPoint pp1 = QPoint(c2.x(),p1.y());
            QPoint pp2 = QPoint(p2.x(),c2.y());
            DirectOval(qimage,current_color,pp1,pp2,-angle);
        }
        else if(valid_dot == 4)
            DirectOval(qimage,current_color,p1,c2,-angle);
        if(mouse_state == 3)
        {
            if(valid_dot == 1)
                p1 = c2;
            else if(valid_dot == 2)
            {
                QPoint pp1 = QPoint(p1.x(),c2.y());
                QPoint pp2 = QPoint(c2.x(),p2.y());
                p1 = pp1;
                p2 = pp2;
            }
            else if(valid_dot == 3)
            {
                QPoint pp1 = QPoint(c2.x(),p1.y());
                QPoint pp2 = QPoint(p2.x(),c2.y());
                p1 = pp1;
                p2 = pp2;
            }
            else if(valid_dot == 4)
                p2 = c2;
            valid_dot = -1;
        }
    }
    else if(choose == 5)
    {
        if(dot_index == -1)
        {
            for(int i = 0; i < list.size(); i++)
                FlagPoint(qimage,QColor(255,0,0),list[i]);
            if(list.size() >= 2)
                Polygon(qimage,current_color,list);
        }
        else
        {
            list.erase(list.begin()+dot_index);
            list.insert(list.begin()+dot_index,c2);
            for(int i = 0; i < list.size(); i++)
                FlagPoint(qimage,QColor(255,0,0),list[i]);
            if(list.size() >= 2)
                Polygon(qimage,current_color,list);
        }
        if(mouse_state == 3)
        {
            dot_index = -1;
        }
    }
    else if(choose == 6)
    {
        if(dot_index == -1)
        {
            for(int i = 0; i < list.size(); i++)
                FlagPoint(qimage,QColor(255,0,0),list[i]);
            if(list.size() >= 2)
                CardinalCurve(qimage,current_color,list);
        }
        else
        {
            list.erase(list.begin()+dot_index);
            list.insert(list.begin()+dot_index,c2);
            for(int i = 0; i < list.size(); i++)
                FlagPoint(qimage,QColor(255,0,0),list[i]);
            if(list.size() >= 2)
                CardinalCurve(qimage,current_color,list);
        }
        if(mouse_state == 3)
        {
            dot_index = -1;
        }
    }
}

void DrawWidget::EditFrame(QImage *qimage, QPoint p1, QPoint p2)
{
    Rectangle(qimage,QColor(0,0,255),p1,p2);
}

void DrawWidget::SaveImage()
{
    QImage t = tmp_qimage->copy(0,0,tmp_qimage->width(),tmp_qimage->height());
    bool isOK;
    QString path = QInputDialog::getText(this,tr("图片保存"),tr("输入绝对路径："),QLineEdit::Normal,tr("D:/Qt/Qt workspace/Paint/images/save.png"),&isOK,Qt::Dialog);
    if(isOK)
        t.save(path);
}

void DrawWidget::LiangBarskyLineCut(QImage *qimage, QPoint d1, QPoint d2)
{
    bool judge = false;
    double u1 = 0, u2 = 1;
    int p[4], q[4];
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    int min_x = d1.x();
    int max_x = d2.x();
    int max_y = d2.y();
    int min_y = d1.y();
    p[0] = x1 - x2;
    p[1] = x2 - x1;
    p[2] = y1 - y2;
    p[3] = y2 - y1;
    q[0] = x1 - min_x;
    q[1] = max_x - x1;
    q[2] = y1 - min_y;
    q[3] = max_y - y1;
    double r;
    for(int i = 0; i < 4; i++)
    {
        r = (double)q[i] / (double)p[i];
        if(p[i] < 0)
        {
            u1 = qMax(u1,r);
            if(u1 > u2)
                judge = true;
        }
        if(p[i] > 0)
        {
            u2 = qMin(u2,r);
            if(u1 > u2)
                judge = true;
        }
        if(p[i] == 0 && q[i] < 0)
        {
            judge = true;
        }
    }
    if(judge)
    {
        return;
    }
    QPoint pp1 = QPoint((int)(x1 - u1 * (x1 - x2)),(int)(y1 - u1 * (y1 - y2)));
    QPoint pp2 = QPoint((int)(x1 - u2 * (x1 - x2)),(int)(y1 - u2 * (y1 - y2)));
    p1 = pp1;
    p2 = pp2;
    ddaLine(qimage,current_color,p1,p2);
}

void DrawWidget::FlagPoint(QImage *qimage, QColor pen, QPoint p)
{
    QPoint p1 = p + QPoint(-4,-4);
    QPoint p2 = p + QPoint(4,4);
    BresenhamCircle(qimage,pen,p1,p2);
    SeadStuff(qimage,pen,qimage->pixelColor(p.x(),p.y()),p);
}

void DrawWidget::CardinalCurve(QImage *qimage, QColor pen, vector<QPoint> list)
{
    int len = list.size();
    QPoint start = list[0];
    QPoint end = list[list.size()-1];
    list.insert(list.begin(),start);
    list.insert(list.end(),end);
    int dotex = 0, dot0 = 1, dot1 =2, dot2 = 3;
    for(int i = 1; i < len; i++)
    {
        int grain = (int)sqrt(pow(list[dot1].x() - list[dot0].x(),2) + pow(list[dot1].y() - list[dot0].y(),2));
        for(int j = 0; j < grain; j++)
        {
            double x = Matrix(list[dotex].x(),list[dot0].x(),list[dot1].x(),list[dot2].x(),(j * (1.0 / grain)));
            double y = Matrix(list[dotex].y(),list[dot0].y(),list[dot1].y(),list[dot2].y(),(j * (1.0 / grain)));
            qimage->setPixelColor((int)x,(int)y,pen);
        }
        dotex++;
        dot0++;
        dot1++;
        dot2++;
    }
}

void DrawWidget::MatrixGet()
{
    m.push_back(-tension);m.push_back(2-tension);m.push_back(tension-2);m.push_back(tension);
    m.push_back(2*tension);m.push_back(tension-3);m.push_back(3-2*tension);m.push_back(-tension);
    m.push_back(-tension);m.push_back(0);m.push_back(tension);m.push_back(0);
    m.push_back(0);m.push_back(1);m.push_back(0);m.push_back(0);
}

double DrawWidget::Matrix(int p0, int p1, int p2, int p3, double u)
{
    double a, b, c, d;
    a = m[0] * p0 + m[1] * p1 + m[2] * p2 + m[3] * p3;
    b = m[4] * p0 + m[5] * p1 + m[6] * p2 + m[7] * p3;
    c = m[8] * p0 + m[9] * p1 + m[10] * p2 + m[11] * p3;
    d = m[12] * p0 + m[13] * p1 + m[14] * p2 + m[15] * p3;
    double ret = ((a*u+b)*u+c)*u+d;
    return ret;
}

//信号和槽函数
void DrawWidget::draw(QAction *q)
{
    emit draw_shape(q->data().value<int>());
}

void DrawWidget::setChoose(int choose)
{
    this->mode = 1;
    angle = 0.0;
    this->choose = choose;
    if(choose == -1)
        update();
    if(choose == 0)
        SaveImage();
    if(choose == 6 || choose == 5)
    {
        list.clear();
        finish = false;
    }
}

void DrawWidget::pick(QAction *q)
{
    emit pick_color(q->data().value<QColor>());
}

void DrawWidget::setColor(QColor color)
{
    this->current_color = color;
}

void DrawWidget::transform(QAction *q)
{
    emit transform_shape(q->data().value<int>());
}

void DrawWidget::setMode(int mode)
{
    this->mode = mode;
    finish = false;
    if(choose != -1 && choose != 1 && choose != 2 && this->mode == 4)
        update();
    if(this->mode == 3)
        angle = 0.0;
}
