#include "DatasetImporter.h"

#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QCommandLinkButton>
#include <QFileDialog>
#include <QDirIterator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>

#include "Dataset.h"

#include "ui_datasetImporter.h"

DatasetImporter::DatasetImporter(const Dataset &dataset, QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::DatasetImporter)
{
    init();
    m_ui->datasetName->setText(dataset.name());
    m_ui->comments->setText(dataset.statComments());
    m_ui->stDataFile->setText(dataset.dataFile());
    m_ui->mainImageFile->setText(dataset.imageFile());
    m_ui->spotMapFile->setText(dataset.spotsFile());
    m_ui->is3D->setChecked(dataset.is3D());
    m_ui->meshFile->setText(dataset.meshFile());
    m_ui->scaling->setValue(dataset.scalingFactor());
    m_ui->scaling->setEnabled(!dataset.is3D());
}

DatasetImporter::DatasetImporter(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::DatasetImporter)
{
    init();
}

void DatasetImporter::init()
{
    m_ui->setupUi(this);

    m_ui->is3D->setChecked(Qt::Unchecked);
    m_ui->loadMeshFile->setEnabled(false);
    m_ui->meshFile->setEnabled(false);
    m_ui->scaling->setEnabled(true);
    m_ui->mainImageFile->setEnabled(true);
    m_ui->loadMainImageFile->setEnabled(true);

    connect(m_ui->loadSTDataFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadSTDataFile);
    connect(m_ui->loadSpotMapFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadSpotsMapFile);
    connect(m_ui->loadMainImageFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadMainImageFile);
    connect(m_ui->loadMeshFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadMeshFile);
    connect(m_ui->loadFolder,
            &QCommandLinkButton::clicked, this, &DatasetImporter::slotParseFolder);
    connect(m_ui->loadMetaFile,
            &QCommandLinkButton::clicked, this, &DatasetImporter::slotParseMetaFile);
    connect(m_ui->is3D,
            &QCheckBox::stateChanged, this, &DatasetImporter::slotChange3D);

}

DatasetImporter::~DatasetImporter()
{
}

const QString DatasetImporter::datasetName() const
{
    return m_ui->datasetName->text();
}

const QString DatasetImporter::comments() const
{
    return m_ui->comments->toPlainText();
}

const QString DatasetImporter::STDataFile() const
{
    return m_ui->stDataFile->text();
}

const QString DatasetImporter::mainImageFile() const
{
    return m_ui->mainImageFile->text();
}

const QString DatasetImporter::meshFile() const
{
    return m_ui->meshFile->text();
}

const QString DatasetImporter::spotsMapFile() const
{
    return m_ui->spotMapFile->text();
}

double DatasetImporter::scalingFactor() const
{
    return m_ui->scaling->value();
}

bool DatasetImporter::is3D() const
{
    return m_ui->is3D->isChecked();
}

void DatasetImporter::slotChange3D(int state)
{
    const bool is3D = state == Qt::Checked;
    m_ui->scaling->setEnabled(!is3D);
    m_ui->mainImageFile->setEnabled(!is3D);
    m_ui->loadMainImageFile->setEnabled(!is3D);
    m_ui->loadMeshFile->setEnabled(is3D);
    m_ui->meshFile->setEnabled(is3D);
}

void DatasetImporter::slotLoadSTDataFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open ST Data File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TSV Files (*.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("ST Data File"), tr("File is incorrect or not readable"));
    } else {
        m_ui->stDataFile->setText(filename);
    }
}

void DatasetImporter::slotLoadMainImageFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Tissue Image File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("JPEG Files (*.jpg *.jpeg)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("Tissue Image File"), tr("File is incorrect or not readable"));
    } else {
        m_ui->mainImageFile->setText(filename);
    }
}

void DatasetImporter::slotLoadMeshFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Mesh File (3D)"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("OBJ Files (*.obj)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("Mesh File (3D)"), tr("File is incorrect or not readable"));
    } else {
        m_ui->meshFile->setText(filename);
    }
}

void DatasetImporter::slotLoadSpotsMapFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Coordinates File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT|TSV Files (*.txt *.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("Spots Coordinates File"), tr("File is incorrect or not readable"));
    } else {
        m_ui->spotMapFile->setText(filename);
    }
}

void DatasetImporter::done(int result)
{
    if(QDialog::Accepted == result)  {
        QString error_msg;
        bool isValid = true;
        if (m_ui->stDataFile->text().isEmpty()) {
            isValid = false;
            error_msg = tr("ST Data file is missing!");
        } else if (m_ui->datasetName->text().isEmpty()) {
            isValid = false;
            error_msg = tr("Dataset name is missing!");
        } else if (m_ui->spotMapFile->text().isEmpty()) {
            isValid = false;
            error_msg = tr("Spot coordinates file is missing!");
        }
        if (!isValid) {
            QMessageBox::critical(this, tr("Import dataset"), error_msg);
        } else {
            QDialog::done(QDialog::Accepted);
        }
    } else {
        QDialog::done(result);
        return;
    }
}

void DatasetImporter::slotParseFolder()
{
    QFileDialog dialog(this, tr("Select folder with the ST Dataset"), QDir::homePath());
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec()) {
        QDir selectedDir = dialog.directory();
        selectedDir.setFilter(QDir::Files);
        QDirIterator it(selectedDir, QDirIterator::NoIteratorFlags);
        while (it.hasNext()) {
            const QString file = it.next();
            qDebug() << "Parsing dataset file from folder " << file;
            if (file.contains(".tsv")) {
                m_ui->stDataFile->setText(file);
            } else if (file.contains(".jpg") || file.contains(".jpeg")) {
                m_ui->mainImageFile->setText(file);
            } else if (file.contains(".obj")) {
                m_ui->meshFile->setText(file);
            } else if (file.contains("spots")) {
                m_ui->spotMapFile->setText(file);
            } else if (file.contains("info.json")) {
                parseInfoJSON(file);
            }
        }
    }
}

void DatasetImporter::slotParseMetaFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Dataset's meta file"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("JSON Files (*.json)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Dataset's meta file"),
                              tr("File is incorrect or not readable"));
        return;
    }

    QFile file_data(filename);
    if (file_data.open(QIODevice::ReadOnly)) {
        const QByteArray &data = file_data.readAll();
        const QJsonDocument &loadDoc = QJsonDocument::fromJson(data);
        const QJsonObject &jsonObject = loadDoc.object();
        if (jsonObject.contains("name")) {
            m_ui->datasetName->setText(jsonObject["name"].toString());
        }
        if (jsonObject.contains("comments")) {
            m_ui->comments->setText(jsonObject["comments"].toString());
        }
        if (jsonObject.contains("data")) {
            m_ui->stDataFile->setText(jsonObject["data"].toString());
        }
        if (jsonObject.contains("image")) {
            m_ui->mainImageFile->setText(jsonObject["image"].toString());
        }
        if (jsonObject.contains("mesh")) {
            m_ui->meshFile->setText(jsonObject["mesh"].toString());
        }
        if (jsonObject.contains("coordinates")) {
            m_ui->spotMapFile->setText(jsonObject["coordinates"].toString());
        }
    } else {
        QMessageBox::critical(this,
                              tr("Dataset's meta file"),
                              tr("Error parsing file"));
    }
}


void DatasetImporter::parseInfoJSON(const QString &filename)
{
    QFile file_data(filename);
    if (file_data.open(QIODevice::ReadOnly)) {
        const QByteArray &data = file_data.readAll();
        const QJsonDocument &loadDoc = QJsonDocument::fromJson(data);
        const QJsonObject &jsonObject = loadDoc.object();
        if (jsonObject.contains("name")) {
            m_ui->datasetName->setText(jsonObject["name"].toString());
        }
        if (jsonObject.contains("comments")) {
            m_ui->comments->setText(jsonObject["comments"].toString());
        }
    } else {
        qDebug() << "Error parsing info.json for dataset";
    }
}
