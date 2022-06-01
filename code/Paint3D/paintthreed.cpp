#include "paintthreed.h"
#include "ui_paintthreed.h"

#include <QtGui>
#include <QtCore>
#include <QtOpenGL>
#include <GL/glu.h>

PaintThreeD::PaintThreeD(QGLWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::PaintThreeD)
{
    ui->setupUi(this);
    angle1 = 0.0;
    angle2 = 0.0;
    x = 1.0;
    y = 1.0;
    z = 1.0;
    vertex.clear();
    slide.clear();
    bool isOK;
    QString path = QInputDialog::getText(this,tr("选择OFF文件"),tr("输入绝对路径："),QLineEdit::Normal,tr("D:/Qt/Qt workspace/Paint3D/DataSet/piano-p865.off"),&isOK,Qt::Dialog);
    if(isOK)
        Parseoff(path);
}

PaintThreeD::~PaintThreeD()
{
    delete ui;
    vertex.clear();
    slide.clear();
}

void PaintThreeD::Parseoff(QString filepath)
{
    QFile f(filepath);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open failed." << endl;
        return;
    }
    QTextStream input(&f);
    QString lineStr;
    input >> lineStr;
    int v_num, s_num, nothing;
    input >> v_num >> s_num >> nothing;
    for(int i = 0; i < v_num; i++)
    {
        VertexValue vv;
        input >> vv.v1 >> vv.v2 >> vv.v3;
        vertex.push_back(vv);
    }
    for(int j = 0; j < s_num; j++)
    {
        int t;
        input >> t;
        VertexID vd;
        input >> vd.id1 >> vd.id2 >> vd.id3;
        slide.push_back(vd);
    }
    f.close();
}

void PaintThreeD::initializeGL()
{
    showFullScreen();
    glShadeModel(GL_SMOOTH);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glEnable(GL_TEXTURE_3D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}

void PaintThreeD::paintGL()
{

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glRotatef((GLfloat)angle1, 0.0, 1.0, 0.0);
        glRotatef((GLfloat)angle2, 1.0, 0.0, 0.0);
        glScalef((GLfloat)x,(GLfloat)y,(GLfloat)z);
        glBegin(GL_TRIANGLES);
        //glColor3f((GLfloat)0.69, (GLfloat)0.77, (GLfloat)0.87);
        for(int j = 0; j < (int)slide.size(); j++)
        {
            glColor3f((GLfloat)0.2f, (GLfloat)0.2f, (GLfloat)0.2f);
            glVertex3f(vertex[slide[j].id1].v1, vertex[slide[j].id1].v2, vertex[slide[j].id1].v3);
            glColor3f((GLfloat)0.4f, (GLfloat)0.4f, (GLfloat)0.4f);
            glVertex3f(vertex[slide[j].id2].v1, vertex[slide[j].id2].v2, vertex[slide[j].id2].v3);
            glColor3f((GLfloat)0.8f, (GLfloat)0.8f, (GLfloat)0.8f);
            glVertex3f(vertex[slide[j].id3].v1, vertex[slide[j].id3].v2, vertex[slide[j].id3].v3);
        }
        glEnd();
}

void PaintThreeD::resizeGL(int width, int height)
{
    if(height == 0)
        height = 1;
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void PaintThreeD::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_Escape:
            close();break;
        case Qt::Key_Left:
            angle1 -= 2;updateGL();break;
        case Qt::Key_Right:
            angle1 += 2;updateGL();break;
        case Qt::Key_Up:
            angle2 -= 2;updateGL();break;
        case Qt::Key_Down:
            angle2 += 2;updateGL();break;
        case Qt::Key_Plus:
            x += 0.05; y += 0.05; z += 0.05;updateGL();break;
        case Qt::Key_Minus:
            x -= 0.05; y -= 0.05; z -= 0.05;updateGL();break;
    }
}
