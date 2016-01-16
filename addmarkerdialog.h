#ifndef ADDMARKERDIALOG_H
#define ADDMARKERDIALOG_H

#include <QDialog>

#include <armarker.h>

namespace Ui {
class AddMarkerDialog;
}

class AddMarkerDialog : public QDialog
{
    Q_OBJECT

public:

    struct MarkerData
    {
        QString markerFile;
        QString modelFile;
        ARMarker::ARMarkerType type;
    };

    explicit AddMarkerDialog(QWidget *parent = 0);
    ~AddMarkerDialog();

    MarkerData data() const;
    void setData(const MarkerData &data);

private slots:
    void chooseFile();
    void changeType(QString type);

private:
    Ui::AddMarkerDialog *m_ui;

    MarkerData m_data;
};

#endif // ADDMARKERDIALOG_H
