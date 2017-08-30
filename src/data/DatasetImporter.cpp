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
    m_ui->species->setText(dataset.statSpecies());
    m_ui->tissue->setText(dataset.statTissue());
    m_ui->comments->setText(dataset.statComments());
    m_ui->stDataFile->setText(dataset.dataFile());
    m_ui->imageAlignmentFile->setText(dataset.imageAlignmentFile());
    m_ui->mainImageFile->setText(dataset.imageFile());
    m_ui->spotMapFile->setText(dataset.spotsFile());
    m_ui->spikeInFile->setText(dataset.spikeinFile());
    m_ui->sizeFactorsFile->setText(dataset.sizeFactorsFile());
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
    connect(m_ui->loadSTDataFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadSTDataFile);
    connect(m_ui->loadSpotMapFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadSpotsMapFile);
    connect(m_ui->loadMainImageFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadMainImageFile);
    connect(m_ui->loadImageAlignmentFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadAlignmentFile);
    connect(m_ui->loadSpikeInFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadSpikeInFile);
    connect(m_ui->loadSizeFactorsFile,
            &QToolButton::clicked, this, &DatasetImporter::slotLoadSizeFactorsFile);
    connect(m_ui->loadFolder,
            &QCommandLinkButton::clicked, this, &DatasetImporter::slotParseFolder);
    connect(m_ui->loadMetaFile,
            &QCommandLinkButton::clicked, this, &DatasetImporter::slotParseMetaFile);
}

DatasetImporter::~DatasetImporter()
{
}

const QString DatasetImporter::datasetName() const
{
    return m_ui->datasetName->text();
}

const QString DatasetImporter::species() const
{
    return m_ui->species->text();
}

const QString DatasetImporter::tissue() const
{
    return m_ui->tissue->text();
}

const QString DatasetImporter::comments() const
{
    return m_ui->comments->toPlainText();
}

const QRect DatasetImporter::chip() const
{
    return QRect(1,1,m_ui->chip_x->value(), m_ui->chip_y->value());
}

const QString DatasetImporter::STDataFile() const
{
    return m_ui->stDataFile->text();
}

const QString DatasetImporter::mainImageFile() const
{
    return m_ui->mainImageFile->text();
}

const QString DatasetImporter::alignmentMatrix() const
{
    return m_ui->imageAlignmentFile->text();
}

const QString DatasetImporter::spotsMapFile() const
{
    return m_ui->spotMapFile->text();
}

const QString DatasetImporter::spikeinFile() const
{
    return m_ui->spikeInFile->text();
}

const QString DatasetImporter::sizeFactorsFile() const
{
    return m_ui->sizeFactorsFile->text();
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
                                           tr("Open Main Image File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("JPEG Files (*.jpg *.jpeg)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("Main Image File"), tr("File is incorrect or not readable"));
    } else {
        m_ui->mainImageFile->setText(filename);
    }
}

void DatasetImporter::slotLoadSpotsMapFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Spots Map File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("Spots Map File"), tr("File is incorrect or not readable"));
    } else {
        m_ui->spotMapFile->setText(filename);
    }
}

void DatasetImporter::slotLoadAlignmentFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Alignment File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Alignment File"),
                              tr("File is incorrect or not readable"));
    } else {
        m_ui->imageAlignmentFile->setText(filename);
    }
}

void DatasetImporter::slotLoadSpikeInFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Spike-in File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Spike-in File"),
                              tr("File is incorrect or not readable"));
    } else {
        m_ui->spikeInFile->setText(filename);
    }
}

void DatasetImporter::slotLoadSizeFactorsFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Size Factors File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Size Factors File"),
                              tr("File is incorrect or not readable"));
    } else {
        m_ui->sizeFactorsFile->setText(filename);
    }
}

void DatasetImporter::done(int result)
{
    if(QDialog::Accepted == result)  {
        QString error_msg;
        bool isValid = true;
        if (m_ui->mainImageFile->text().isEmpty()) {
            isValid = false;
            error_msg = tr("Main image is missing!");
        } else if (m_ui->stDataFile->text().isEmpty()) {
            isValid = false;
            error_msg = tr("ST Data file is missing!");
        } else if (m_ui->datasetName->text().isEmpty()) {
            isValid = false;
            error_msg = tr("Dataset name is missing!");
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
            } else if (file.contains(".jpg")) {
                m_ui->mainImageFile->setText(file);
            } else if (file.contains("alignment")) {
                m_ui->imageAlignmentFile->setText(file);
            } else if (file.contains("spots")) {
                m_ui->spotMapFile->setText(file);
            } else if (file.contains("spikein")) {
                m_ui->spikeInFile->setText(file);
            } else if (file.contains("sizefactors")) {
                m_ui->sizeFactorsFile->setText(file);
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
        if (jsonObject.contains("species")) {
            m_ui->species->setText(jsonObject["species"].toString());
        }
        if (jsonObject.contains("tissue")) {
            m_ui->tissue->setText(jsonObject["tissue"].toString());
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
        if (jsonObject.contains("aligment")) {
            m_ui->imageAlignmentFile->setText(jsonObject["aligment"].toString());
        }
        if (jsonObject.contains("coordinates")) {
            m_ui->spotMapFile->setText(jsonObject["coordinates"].toString());
        }
        if (jsonObject.contains("spike_ins")) {
            m_ui->spikeInFile->setText(jsonObject["spike_ins"].toString());
        }
        if (jsonObject.contains("size_factors")) {
            m_ui->sizeFactorsFile->setText(jsonObject["size_factors"].toString());
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
        if (jsonObject.contains("species")) {
            m_ui->species->setText(jsonObject["species"].toString());
        }
        if (jsonObject.contains("tissue")) {
            m_ui->tissue->setText(jsonObject["tissue"].toString());
        }
        if (jsonObject.contains("comments")) {
            m_ui->comments->setText(jsonObject["comments"].toString());
        }
    } else {
        qDebug() << "Error parsing info.json for dataset";
    }
}
