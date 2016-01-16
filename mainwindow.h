#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void addMarker();

private:
    Ui::MainWindow *ui;

    int setupCamera(const char *cparam_name, char *vconf, ARParamLT **cparamLT_p, ARHandle **arhandle, AR3DHandle **ar3dhandle);

    int windowed = TRUE;                     // Use windowed (TRUE) or fullscreen mode (FALSE) on launch.
    int windowWidth = 640;					// Initial window width, also updated during program execution.
    int windowHeight = 480;                  // Initial window height, also updated during program execution.
    int windowDepth = 32;					// Fullscreen mode bit depth.
    int windowRefresh = 0;					// Fullscreen mode refresh rate. Set to 0 to use default rate.
};

#endif // MAINWINDOW_H
