#ifndef PAINTTHREED_H
#define PAINTTHREED_H

#include <QtOpenGL>
#include <QWidget>
#include <vector>
#include <QTextStream>
#include <QFile>
#include <QDebug>
using namespace std;

namespace Ui {
class PaintThreeD;
}

struct VertexValue
{
    GLfloat v1;
    GLfloat v2;
    GLfloat v3;
};

struct VertexID
{
    int id1;
    int id2;
    int id3;
};

class PaintThreeD : public QGLWidget
{
    Q_OBJECT

public:
    explicit PaintThreeD(QGLWidget *parent = nullptr);
    ~PaintThreeD();

protected:
    void initializeGL();
    void paintGL();       //作图
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent *e);    //处理键盘事件
    void Parseoff(QString filepath);     //解析OFF文件

protected:
    double angle1;  //控制旋转角度的参数
    double angle2;
    double x;       //控制放大缩小的参数
    double y;
    double z;
    vector<VertexValue> vertex;  //存三维模型的顶点
    vector<VertexID> slide;      //存三维模型的面

private:
    Ui::PaintThreeD *ui;
};

#endif // PAINTTHREED_H
