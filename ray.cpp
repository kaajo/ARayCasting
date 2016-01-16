#include "ray.h"

Ray::Ray(QVector4D startPoint, QVector4D endPoint)
{
    m_startPoint = startPoint;
    m_endPoint = endPoint;
}

QVector4D Ray::startPoint() const
{
    return m_startPoint;
}

QVector4D Ray::endPoint() const
{
    return m_endPoint;
}

void Ray::setEndPoint(QVector4D vec)
{
    m_endPoint = vec;
}

void Ray::applyTransformation(QMatrix4x4 &transformation)
{
    m_startPoint = transformation * m_startPoint;
    m_endPoint = transformation * m_endPoint;
}

QMatrix4x4 Ray::makeTransformationMatrix(ARdouble t[16], bool needTranspose)
{
    QMatrix4x4 proj(t[0], t[1], t[2], t[3],
                    t[4], t[5], t[6], t[7],
                    t[8], t[9], t[10], t[11],
                    t[12], t[13], t[14], t[15]);

    if(needTranspose)
    {
        proj = proj.transposed();
    }

    return proj;
}

float Ray::intersectionWithPolygon(Ray &ray,const QVector<QVector4D> polygon, QVector3D &normal, QVector3D &intersection, Ray &mirror)
{
    if(polygon.size() < 3) return std::numeric_limits<float>::max();

    normal = QVector3D::crossProduct((polygon[1] - polygon[0]).toVector3D(), (polygon[2] - polygon[1]).toVector3D());

    float distanceFromOrigin = QVector3D::dotProduct(normal, polygon[0].toVector3D());

    QVector4D ba = ray.endPoint() - ray.startPoint();

    float nDotA = QVector3D::dotProduct(normal, ray.startPoint().toVector3D());
    float nDotBA = QVector3D::dotProduct(normal, ba.toVector3D());

    intersection = ray.startPoint().toVector3D() + (((distanceFromOrigin - nDotA)/nDotBA) * ba.toVector3D());

    QVector3D vector(QVector4D(intersection,1.0f)- ray.startPoint());

    QMatrix4x4 mat;
    mat.rotate(180, normal);

    vector = mat * vector;

    mirror = Ray(QVector4D(intersection,1.0), QVector4D(vector+intersection,1.0f));

    return calcAngleSum(intersection, polygon);
}

float Ray::intersectionWithPolygonAlt(Ray &ray, const QVector<QVector4D> polygon, QVector3D &normal, QVector3D &intersection, Ray &mirror)
{
    if(polygon.size() < 3) return std::numeric_limits<float>::max();

    normal = QVector3D::crossProduct((polygon[1] - polygon[0]).toVector3D(), (polygon[2] - polygon[1]).toVector3D());

    float distanceFromOrigin = QVector3D::dotProduct(normal, polygon[1].toVector3D() - polygon[0].toVector3D());

    QVector4D ba = ray.endPoint() - ray.startPoint();
    QVector4D pa = polygon[2] - ray.startPoint();

    float nDotA = QVector3D::dotProduct(normal, pa.toVector3D());
    float nDotBA = QVector3D::dotProduct(normal, ba.toVector3D());

    intersection = ray.startPoint().toVector3D() + (((distanceFromOrigin - nDotA)/nDotBA) * ba.toVector3D());

    QVector3D vector(QVector4D(intersection,1.0f)- ray.startPoint());

    QMatrix4x4 mat;
    mat.rotate(180, normal);

    vector = mat * vector;

    mirror = Ray(QVector4D(intersection,1.0), QVector4D(vector+intersection,1.0f));

    return calcAngleSum(intersection, polygon);
}

float Ray::distanceFromVertex(const Ray &ray, const QVector4D &vertex)
{
    /*
    float t = - QVector4D::dotProduct((ray.startPoint() - vertex),(ray.endPoint() - ray.startPoint()))/ ((ray.endPoint()- ray.startPoint()).lengthSquared());

    //return ((ray.startPoint() - vertex) + (ray.endPoint() - ray.startPoint())*t)*((ray.startPoint() - vertex) + (ray.endPoint() - ray.startPoint())*t) + ((ray.startPoint().z() - vertex.z()) + (ray.endPoint().z() - ray.startPoint().z())*t)*((ray.startPoint().z() - vertex.z()) + (ray.endPoint().z() - ray.startPoint().z())*t);

    QVector4D vec = ray.endPoint() - ray.endPoint();

    QVector4D pnt = ray.startPoint() + vec * t;

    return QPair<float,float>((pnt-vertex).length(), t) ;
    */

    float retVal = QVector3D::crossProduct((ray.endPoint() - ray.startPoint()).toVector3D(), (ray.startPoint() - vertex).toVector3D()).length() / (ray.endPoint() - ray.startPoint()).length();

    return retVal;
}

float Ray::calcAngleSum(QVector3D q, QVector<QVector4D> p)
{
    float m1,m2;
    float anglesum=0,costheta;
    QVector3D p1,p2;

    float twoPi = M_PI * 2;

    int n = p.size();

    for (int i=0;i<n;++i) {

        p1.setX(p[i].x() - q.x());
        p1.setY(p[i].y() - q.y());
        p1.setZ(p[i].z() - q.z());
        p2.setX(p[(i+1)%n].x() - q.x());
        p2.setY(p[(i+1)%n].y() - q.y());
        p2.setZ(p[(i+1)%n].z() - q.z());

        m1 = p1.length();
        m2 = p2.length();
        if (m1*m2 <= 0.00001)
            return 0; /* We are on a node, consider this inside */
        else
            costheta = (p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z()) / (m1*m2);

        anglesum += acos(costheta);
    }

    return qAbs(anglesum - twoPi);
}

