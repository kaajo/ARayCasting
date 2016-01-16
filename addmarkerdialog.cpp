#include "addmarkerdialog.h"
#include "ui_addmarkerdialog.h"

#include <QFileDialog>

AddMarkerDialog::AddMarkerDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AddMarkerDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->markerButton, &QPushButton::clicked, this, &AddMarkerDialog::chooseFile);
    connect(m_ui->modelButton, &QPushButton::clicked, this, &AddMarkerDialog::chooseFile);
    connect(m_ui->TypeComboBox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated) , this, &AddMarkerDialog::changeType);
}

AddMarkerDialog::~AddMarkerDialog()
{
    delete m_ui;
}

void AddMarkerDialog::chooseFile()
{
    auto snd = sender();

    if(snd == m_ui->markerButton)
    {
        m_data.markerFile = QFileDialog::getOpenFileName(this,
                            tr("Open Marker File"), QDir::currentPath(), tr("Marker files (*.png *.jpg *.bmp)"));
        m_ui->markerLine->setText(m_data.markerFile);
    }
    else if(snd == m_ui->modelButton)
    {
        m_data.modelFile = QFileDialog::getOpenFileName(this,
                           tr("Open Model File"), QDir::currentPath(), tr("3D models (*.obj *.fbx *.3ds)"));
        m_ui->modelLine->setText(m_data.modelFile);
    }
}

void AddMarkerDialog::changeType(QString type)
{
    if(type == "Lightsource")
    {
        m_ui->modelButton->setEnabled(false);
        m_data.type = ARMarker::ARMarkerType::LIGHTSOURCE;
    }
    else
    {
        m_ui->modelButton->setEnabled(true);
        m_data.type = ARMarker::ARMarkerType::OBJECT;
    }
}

AddMarkerDialog::MarkerData AddMarkerDialog::data() const
{
    return m_data;
}

void AddMarkerDialog::setData(const MarkerData &data)
{
    m_data = data;
}
