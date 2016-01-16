#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addmarkerdialog.h"

#include <QMessageBox>

#include <GL/glut.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    char cparam_name[] = "Data/camera_para.dat";
    char vconf[] = "v4l2src device=/dev/video0 use-fixed-fps=false ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb ! identity name=artoolkit ! fakesink";

    if (!setupCamera(cparam_name, vconf, &ui->openGLWidget->gCparamLT, &ui->openGLWidget->gARHandle, &ui->openGLWidget->gAR3DHandle))
    {
        exit(-1);
    }
/*
    char modelName[] = "Data/box.fbx";
    char patt_name[]  = "Data/1.patt";
    ui->openGLWidget->addMarker(modelName, patt_name, ARMarker::ARMarkerType::OBJECT, 0.7f);
*/

    char modelName[] = "Data/bear.fbx";
    char patt_name[]  = "Data/1.patt";
    ui->openGLWidget->addMarker(modelName, patt_name, ARMarker::ARMarkerType::OBJECT, 5.0f);

    char modelName2[] = "Data/lampa.fbx";
    char patt_name2[]  = "Data/light.patt";
    ui->openGLWidget->addMarker(modelName2, patt_name2, ARMarker::ARMarkerType::LIGHTSOURCE, 0.3f);

    ui->openGLWidget->setup();

    connect(ui->actionAddMarker, &QAction::triggered, this, &MainWindow::addMarker);
}

MainWindow::~MainWindow()
{
    ui->openGLWidget->capture = false;
    ui->openGLWidget->future.waitForFinished();

    delete ui;
}

void MainWindow::addMarker()
{
    AddMarkerDialog marker;

    if(marker.exec() == QDialog::Accepted)
    {

    }
}

int MainWindow::setupCamera(const char *cparam_name, char *vconf, ARParamLT **cparamLT_p, ARHandle **arhandle, AR3DHandle **ar3dhandle)
{
    ARParam			cparam;
    int				xsize, ysize;
    AR_PIXEL_FORMAT pixFormat;

    // Open the video path.
    if (arVideoOpen(vconf) < 0) {
        QMessageBox::critical(this, "Camera setup failed" ,"Unable to open connection to camera.\n");
        return (FALSE);
    }

    // Find the size of the window.
    if (arVideoGetSize(&xsize, &ysize) < 0) {
        QMessageBox::critical(this, "Camera setup failed" ,"Unable to determine camera frame size.\n");
        arVideoClose();
        return (FALSE);
    }
    ARLOGi("Camera image size (x,y) = (%d,%d)\n", xsize, ysize);

    // Get the format in which the camera is returning pixels.
    pixFormat = arVideoGetPixelFormat();
    if (pixFormat == AR_PIXEL_FORMAT_INVALID) {
        QMessageBox::critical(this, "Camera setup failed" ,"Camera is using unsupported pixel format.\n");
        arVideoClose();
        return (FALSE);
    }

    // Load the camera parameters, resize for the window and init.
    if (arParamLoad(cparam_name, 1, &cparam) < 0) {
        QMessageBox::critical(this, "Camera setup failed" ,QString("Error loading parameter file" + QString(cparam_name) + "for camera.\n").toStdString().c_str());
        arVideoClose();
        return (FALSE);
    }
    if (cparam.xsize != xsize || cparam.ysize != ysize) {
        ARLOGw("*** Camera Parameter resized from %d, %d. ***\n", cparam.xsize, cparam.ysize);
        arParamChangeSize(&cparam, xsize, ysize, &cparam);
    }
#ifdef DEBUG
    ARLOG("*** Camera Parameter ***\n");
    arParamDisp(&cparam);
#endif
    if ((*cparamLT_p = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL) {
        QMessageBox::critical(this, "Camera setup failed" ,"Error: arParamLTCreate.\n");
        return (FALSE);
    }

    if ((*arhandle = arCreateHandle(*cparamLT_p)) == NULL) {
        QMessageBox::critical(this, "Camera setup failed" ,"Error: arCreateHandle.\n");
        return (FALSE);
    }
    if (arSetPixelFormat(*arhandle, pixFormat) < 0) {
        QMessageBox::critical(this, "Camera setup failed" ,"Error: arSetPixelFormat.\n");
        return (FALSE);
    }
    if (arSetDebugMode(*arhandle, AR_DEBUG_DISABLE) < 0) {
        QMessageBox::critical(this, "Camera setup failed" ,"Error: arSetDebugMode.\n");
        return (FALSE);
    }
    if ((*ar3dhandle = ar3DCreateHandle(&cparam)) == NULL) {
        QMessageBox::critical(this, "Camera setup failed" ,"Error: ar3DCreateHandle.\n");
        return (FALSE);
    }

    if (arVideoCapStart() != 0) {
        QMessageBox::critical(this, "Camera setup failed" ,"Unable to begin camera data capture.\n");
        return (FALSE);
    }

    return (TRUE);
}
