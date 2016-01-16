#ifndef ARGLSCENE_H
#define ARGLSCENE_H

#include "ray.h"
#include "armarker.h"

#include <QVector>
#include <QOpenGLWidget>
#include <QFuture>

#include <fbxsdk.h>

namespace Ui {
class ARGlScene;
}

class ARGlScene : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit ARGlScene(QWidget *parent = 0);
    ~ARGlScene();

    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    void setup();
    void addMarker(const char *modelPath, const char *patt_name, ARMarker::ARMarkerType type, float size = 1.0f);
    void addMarker(ARMarker *marker);

    ARParamLT *gCparamLT = nullptr;

    AR3DHandle *gAR3DHandle = nullptr;

    ARHandle *gARHandle = nullptr;

    bool capture = true;

    QFuture<void> future;
public slots:
    void stopMainLoop();
    void checkMesh(FbxMesh* mesh, FbxVector4* vertexes, FbxAMatrix matrix);

    void mainLoop();

   signals:
    void startMainLoop();
private:
    Ui::ARGlScene *ui;

    struct ARMesh
    {
        ARMarker* marker = nullptr;
        FbxMesh* mesh = nullptr;
        FbxVector4 *points = nullptr;
        FbxAMatrix posMat;
    };

    FbxManager * mSdkManager = nullptr;

    QVector<Ray> m_lineStrip;
    QVector<ARMesh> m_meshesToCheck;
    static QVector4D makeVertexes(const QMatrix4x4 &matrix, const FbxAMatrix &mat, FbxVector4 point);

    // Transformation matrix retrieval.

    ARGL_CONTEXT_SETTINGS_REF gArglSettings = nullptr;

    // Image acquisition.
    ARUint8		*gARTImage = nullptr;
    int          gARTImageSavePlease = false;

    QVector<ARMarker*> markers;

    int m_iterations = 5;

    volatile bool ended = false;

    bool LoadFile();

    void drawBackground(const float width, const float height, const float x, const float y);

    void printHelpKeys();

    void drawLines() const;

    void computeRays();

    void printMode();

    void print(const char *text, const float x, const float y, int calculateXFromRightEdge, int calculateYFromTopEdge);
};

#endif // ARGLSCENE_H
