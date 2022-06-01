#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QImage>
#include <QAction>
#include <vector>
using namespace std;

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWidget(QWidget *parent = nullptr);  //构造函数
    ~DrawWidget();

public:
    void paintEvent(QPaintEvent *event);  //绘图事件
    void mousePressEvent(QMouseEvent *event);  //鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event);  //鼠标拖动事件
    void mouseReleaseEvent(QMouseEvent *event);  //鼠标释放事件
    double coordinate_x(double x, double y, int a, int b, double angle);  //计算旋转后的坐标
    double coordinate_y(double x, double y, int a, int b, double angle);
    void ddaLine(QImage *qimage, QColor pen, QPoint p1, QPoint p2);  //画直线
    void BresenhamCircle(QImage *qimage, QColor pen, QPoint p1, QPoint p2);  //画圆
    void BresenhamOval(QImage *qimage, QColor pen, QPoint p1, QPoint p2);  //画椭圆
    void DirectOval(QImage *qimage, QColor pen, QPoint p1, QPoint p2, double angle);  //画斜椭圆
    void Rectangle(QImage *qimage, QColor pen, QPoint p1, QPoint p2);  //画矩形
    void Polygon(QImage *qimage, QColor pen, vector<QPoint> plist); //画多边形
    void SeadStuff(QImage *qimage, QColor pen, QColor stuffcolor, QPoint p2);  //填充
    int StuffLine(QImage *qimage, QColor pen, QColor stuffcolor, QPoint p);
    void PushStack(QImage *qimage, QColor pen, QColor stuffcolor, QPoint p, QStack<QPoint> *qstack, int len);
    void Translation(QImage *qimage, QPoint a1, QPoint a2);  //平移
    void Rotation(QImage *qimage, QPoint b1, QPoint b2);  //旋转
    void Editing(QImage *qimage, QPoint c1, QPoint c2);  //编辑
    void EditFrame(QImage *qimage, QPoint p1, QPoint p2);  //画包围盒
    void SaveImage();  //保存图片
    void LiangBarskyLineCut(QImage *qimage, QPoint d1, QPoint d2); //线段的裁剪
    void FlagPoint(QImage *qimage, QColor pen, QPoint p);  //标记曲线的控制点
    void CardinalCurve(QImage *qimage, QColor pen, vector<QPoint> list); //画曲线
    void MatrixGet(); //得到参数矩阵
    double Matrix(int p0, int p1, int p2, int p3, double u);  //计算坐标

signals:
    void draw_shape(int choose);
    void pick_color(QColor color);
    void transform_shape(int mode);

public slots:
    void draw(QAction *q);
    void setChoose(int choose);
    void pick(QAction *q);
    void setColor(QColor color);
    void transform(QAction *q);
    void setMode(int mode);

private:
    QPoint p1, p2;  //绘图时鼠标的起点和终点
    QPoint a1, a2;  //平移时鼠标的起点和终点
    QPoint b1, b2;  //旋转时鼠标的起点和终点
    QPoint c1, c2;  //编辑时鼠标的起点和终点
    QPoint d1, d2;  //裁剪时鼠标的起点和终点
    int mode;   //当前模式的标志位 -1无模式 1绘画  2平移  3旋转  4编辑  5裁剪
    int choose; //当前图元的标志位 -1清屏 0保存图片 1填充 2直线 3圆 4椭圆 5多边形 6曲线
    int mouse_state; //当前鼠标状态的标志位 -1无状态 1点击  2拖动  3释放
    QColor current_color;  //当前画笔颜色
    QImage *qimage;  //当前的画布
    QImage *tmp_qimage;  //为实现动态作画而创建的画布
    QImage *editframe;  //画包围盒的画布
    QImage *cutframe;  //画裁剪窗口
    int valid_dot;  //编辑时选中的点的标志位 -1没选中 1选中p1  2选中p2  3选中p3  4选中p4
    double angle; //旋转时的角度 0~2Pi
    vector<QPoint> list; //存储每一次画曲线的控制点
    vector<double> m; //曲线变换矩阵
    double tension; //张量
    bool finish; //绘制曲线的标志
    int dot_index; //选中的曲线的控制点的标志位
};

#endif // DRAWWIDGET_H
