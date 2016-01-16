#include <GL/glut.h>

#include "mainwindow.h"
#include <QApplication>

#include <fbxsdk.h>
#include <ray.h>

int main(int argc, char** argv)
{
/*
    char glutGamemode[32];
    char cparam_name[] = "Data/camera_para.dat";
    char vconf[] = "v4l2src device=/dev/video0 use-fixed-fps=false ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb ! identity name=artoolkit ! fakesink";
    char patt_name[]  = "Data/patt.hiro";
*/
    //
    // Library inits.
    //

    Ray r(QVector4D(0.5, 0.5, -1000, 1.0), QVector4D(1, 1, 1000, 1.0));

    QVector4D d1(0,0,0,1), d2(1,0,0,1), d3(1,1,0,1), d4(0,1,0,1);
    QVector<QVector4D> v;
    v.push_back(d1);
    v.push_back(d2);
    v.push_back(d3);
    v.push_back(d4);

    QVector3D intersection;
    QVector3D normal;
    Ray mirror;

    qDebug() << Ray::intersectionWithPolygon(r, v, normal, intersection, mirror);
    qDebug()<< intersection <<  mirror.endPoint() - mirror.startPoint();


    qDebug() << Ray::intersectionWithPolygonAlt(r, v, normal, intersection, mirror);
    qDebug()<< intersection <<  mirror.endPoint() - mirror.startPoint();

    qRegisterMetaType<FbxVector4>("FbxVector4");
    qRegisterMetaType<FbxMesh*>("FbxMesh*");

    glutInit(&argc, argv);

    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();

}
