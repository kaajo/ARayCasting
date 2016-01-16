#ifndef RAY_H
#define RAY_H

#include <AR/ar.h>

#include <QVector2D>
#include <QVector4D>
#include <QMatrix4x4>

#include <sys/cdefs.h>

class Ray
{
public:
    Ray(QVector4D startPoint = QVector4D(), QVector4D endPoint = QVector4D());

    QVector4D startPoint() const;
    QVector4D endPoint() const;

    void setEndPoint(QVector4D vec);

    void applyTransformation(QMatrix4x4 &transformation);
    static QMatrix4x4 makeTransformationMatrix(ARdouble t[16], bool needTranspose = false);

    static float intersectionWithPolygon(Ray &ray, const QVector<QVector4D> polygon, QVector3D &normal, QVector3D &intersection, Ray &mirror);
    static float intersectionWithPolygonAlt(Ray &ray,const QVector<QVector4D> polygon, QVector3D &normal, QVector3D &intersection, Ray &mirror);

    static float distanceFromVertex(const Ray &ray, const QVector4D &vertex);

    static float calcAngleSum(QVector3D q, QVector<QVector4D> p);

private:
    QVector4D m_startPoint;
    QVector4D m_endPoint;

    int m_iteration = 0;
};

#endif // RAY_H
