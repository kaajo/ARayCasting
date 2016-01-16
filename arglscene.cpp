#include "SceneContext.h"
#include "GL/glut.h"

#include "arglscene.h"
#include "ui_arglscene.h"

#include "Common.h"

#include <functional>
#include <limits>

#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QMutex>
#include <QVector3D>
#include <QVector4D>
#include <QtConcurrent/QtConcurrent>
#include <QTime>

#define VIEW_SCALEFACTOR		1.0         // Units received from ARToolKit tracking will be multiplied by this factor before being used in OpenGL drawing.
#define VIEW_DISTANCE_MIN		40.0        // Objects closer to the camera than this will not be displayed. OpenGL units.
#define VIEW_DISTANCE_MAX		10000.0     // Objects further away from the camera than this will not be displayed. OpenGL units.

ARGlScene::ARGlScene(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::ARGlScene)
{
    ui->setupUi(this);

    InitializeSdkObjects(mSdkManager);
    connect(this, &ARGlScene::startMainLoop, this, &ARGlScene::mainLoop);

    ended = true;
}

ARGlScene::~ARGlScene()
{
    arVideoCapStop();
    arParamLTFree(&gCparamLT);
    arVideoClose();
    ar3DDeleteHandle(&gAR3DHandle);
    arglCleanup(gArglSettings);
    gArglSettings = NULL;
    arPattDetach(gARHandle);
    arDeleteHandle(gARHandle);

    qDeleteAll(markers);

    delete ui;
}

void ARGlScene::initializeGL()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    InitializeOpenGL();
}

void ARGlScene::resizeGL(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

}

void ARGlScene::paintGL()
{
    ARdouble p[16];

    // Select correct buffer for this context.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers for new frame.

    arglDispImage(gARTImage, &(gCparamLT->param), VIEW_SCALEFACTOR, gArglSettings);	// zoom = 1.0.
    //gARTImage = NULL; // Invalidate image data.

    // Projection transformation.
    arglCameraFrustumRH(&(gCparamLT->param), 5, 1000, p); //!!!
    glMatrixMode(GL_PROJECTION);

    glLoadMatrixd(p);

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);

    // Viewing transformation.
    glLoadIdentity();
    // Lighting and geometry that moves with the camera should go here.
    // (I.e. must be specified before viewing transformations.)
    //none
    drawLines();

    /*
    glPointSize(4.5);

    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);

    foreach(const ARMarker *marker, markers)
    {
        for(const QVector4D &vec : marker->m_vertexes)
        {
            glVertex4d(vec.x(), vec.y(), vec.z(), vec.w());
        }
    }
    */

    glEnd();

    foreach(const ARMarker *marker, markers)
    {
        if (marker->found())
        {
            ARdouble m[16];

            // Calculate the camera position relative to the marker.
            // Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
            arglCameraViewRH(marker->gPatt_trans, m, 1);
            glLoadMatrixd(m);

            // All lighting and geometry to be drawn relative to the marker goes here.
            marker->draw();
        } // gPatt_found
    }

    if(ended)
    {
        future =  QtConcurrent::run(this, &ARGlScene::mainLoop);
    }
}

void ARGlScene::drawLines() const
{

    glColor3f(0.8f, 0.8f, 0.8f);

    glLineWidth(5.0);
    glPointSize(5);

    glBegin(GL_LINES);
    foreach(const Ray &vec, m_lineStrip)
    {

        glVertex4f(vec.startPoint().x(), vec.startPoint().y(), vec.startPoint().z(), vec.startPoint().w());
        glVertex4f(vec.endPoint().x(), vec.endPoint().y(),vec.endPoint().z() , vec.endPoint().w());

        /*
        glBegin(GL_POINTS);
        glVertex4f(vec.startPoint().x(), vec.startPoint().y(), vec.startPoint().z(), vec.startPoint().w());
        glEnd();
        */
    }
    glEnd();
    glLineWidth(1.0);
}

void ARGlScene::addMarker(const char* modelPath, const char *patt_name, ARMarker::ARMarkerType type, float size)
{
    ARMarker *marker = new ARMarker(mSdkManager,modelPath, patt_name,type);
    marker->setModelSize(size);
    addMarker(marker);
}

void ARGlScene::addMarker(ARMarker *marker)
{
    connect(marker, &ARMarker::meshForCheck, this, &ARGlScene::checkMesh);
    markers.push_back(marker);
}

void ARGlScene::setup()
{
    if((arPattAttach(gARHandle, ARMarker::ARPatternHandle)) < 0)
    {
        ARLOGe("setupMarker(): Error attach markers\n");
        exit(-1);
    }

    if ((gArglSettings = arglSetupForCurrentContext(&(gCparamLT->param), arVideoGetPixelFormat())) == nullptr) {
        ARLOGe("main(): arglSetupForCurrentContext() returned error.\n");
        exit(-1);
    }
    arglSetupDebugMode(gArglSettings, gARHandle);
    arUtilTimerReset();

    future =  QtConcurrent::run(this, &ARGlScene::mainLoop);
}

void ARGlScene::drawBackground(const float width, const float height, const float x, const float y)
{
    GLfloat vertices[4][2];

    vertices[0][0] = x; vertices[0][1] = y;
    vertices[1][0] = width + x; vertices[1][1] = y;
    vertices[2][0] = width + x; vertices[2][1] = height + y;
    vertices[3][0] = x; vertices[3][1] = height + y;
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);	// 50% transparent black.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Opaque white.
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_BLEND);
}

void ARGlScene::computeRays()
{
    m_lineStrip.clear();

    this->blockSignals(true);

    //find all lightsources and compute initial (0 iteration) of rays
    for (ARMarker *marker: markers)
    {
        ARdouble p[16];

        // Calculate the camera position relative to the marker.
        // Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
        arglCameraViewRH(marker->gPatt_trans, p, 1);
        marker->transformation = Ray::makeTransformationMatrix(p, true);

        if(marker && marker->found() && marker->type() == ARMarker::ARMarkerType::LIGHTSOURCE)
        {
            QVector4D startPoint(9.0f, 15.0f, 62.0f, 1.0f);
            QVector4D endPoint(9.0f, 1000.0f, 62.0f, 1.0f);
            Ray ray(startPoint, endPoint);
            ray.applyTransformation(marker->transformation);

            m_lineStrip.push_back(ray);
        }

        marker->m_vertexes.clear();
    }

    for(Ray &ray : m_lineStrip)
    {
        //find intersection with every ray

        for(const ARMesh &mesh : m_meshesToCheck)
        {
            if(!mesh.mesh || !mesh.marker || !mesh.points) return;

            std::vector<FbxVector4> auxVector;
            auxVector.assign(mesh.points, mesh.points + mesh.mesh->GetPolygonCount());
            QVector<FbxVector4> vec = QVector<FbxVector4>::fromStdVector(auxVector);

            mesh.marker->transformation.scale(mesh.marker->modelSize());

            auto f = std::bind(&ARGlScene::makeVertexes, mesh.marker->transformation, mesh.posMat , std::placeholders::_1);

            mesh.marker->m_vertexes.append(QtConcurrent::blockingMapped<QVector<QVector4D>>(vec, f));

            for(int i = 0; i < mesh.mesh->GetPolygonCount(); ++i)
            {
                int polySize = mesh.mesh->GetPolygonSize(i);

                if(polySize < 3) continue;

                QVector<QVector4D> polygon;

                for(int j = 0; j < polySize; ++j)
                {
                    int vertexIndex = mesh.mesh->GetPolygonVertex(i, j);

                    polygon.push_back(mesh.marker->m_vertexes[vertexIndex]);
                }

                QVector3D intersection;
                QVector3D normal;
                Ray mirror;

                float in = Ray::intersectionWithPolygon(ray, polygon,normal, intersection, mirror);

                if(in < 0.3 && std::atan2(QVector3D::crossProduct(normal, intersection - ray.startPoint().toVector3D()).length(), QVector3D::dotProduct(normal, intersection - ray.startPoint().toVector3D())) < 90.0f)
                {
                    ray.setEndPoint({intersection, 1.0f});
                    //m_lineStrip.push_back(mirror);
                    break;
                }
            }
        }
    }

    for(ARMesh &mesh : m_meshesToCheck)
    {
        if(mesh.points) delete[] mesh.points;
        mesh.points = nullptr;
    }

    m_meshesToCheck.clear();

    this->blockSignals(false);
}

void ARGlScene::mainLoop()
{
    if(!ended) return;

    ended = false;

    ARUint8 *image = nullptr;

    int j, k;

    // Grab a video frame.
    /*forever
    {*/

        QMutex mutex;
        mutex.lock();

        if ((image = arVideoGetImage()) != NULL) {
            gARTImage = image;	// Save the fetched image.

            // Detect the markers in the video frame.
            if (arDetectMarker(gARHandle, gARTImage) < 0) {
                exit(-1);
            }

            foreach (ARMarker *marker, markers)
            {
                // Check through the marker_info array for highest confidence
                // visible marker matching our preferred pattern.
                k = -1;
                for (j = 0; j < gARHandle->marker_num; j++)
                {

                    if (gARHandle->markerInfo[j].id == marker->id())
                    {
                        if (k == -1) k = j; // First marker detected.
                        else if (gARHandle->markerInfo[j].cf > gARHandle->markerInfo[k].cf) k = j; // Higher confidence marker detected.
                    }
                }

                if (k != -1) {
                    // Get the transformation between the marker and the real camera into gPatt_trans.
                    marker->makeTransformationMatrix(&(gARHandle->markerInfo[k]), gAR3DHandle);
                    marker->setFound(true);
                }
                else
                {
                    marker->setFound(false);
                }
            }

            computeRays();

            mutex.unlock();

            ended = true;

            // Tell GLUT the display has changed.
            update();
        }
    /*}*/
}

void ARGlScene::stopMainLoop()
{
    future.cancel();
}

void ARGlScene::checkMesh(FbxMesh *mesh, FbxVector4 *vertexes, FbxAMatrix matrix)
{
    //qDebug() << "checking mesh " << mesh->GetPolygonCount();

    ARMesh armesh;
    armesh.marker = qobject_cast<ARMarker*>(sender());
    armesh.mesh = mesh;
    armesh.points = vertexes;
    armesh.posMat = matrix;

    m_meshesToCheck.push_back(armesh);
}

QVector4D ARGlScene::makeVertexes(const QMatrix4x4 &matrix, const FbxAMatrix &mat, FbxVector4 point)
{
    QMatrix4x4 m;

    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            m(i,j) = mat[j][i];
        }
    }

    QVector3D retVal(point.mData[0], point.mData[1], point.mData[2]);

    retVal = m * retVal;
    retVal = matrix * retVal;

    return QVector4D(retVal, 1.0);
}
