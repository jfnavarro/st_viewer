#include "DatasetImporter.h"
#include "ui_datasetImporter.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

DatasetImporter::DatasetImporter(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::DatasetImporter)
{
    m_ui->setupUi(this);
    connect(m_ui->loadSTDataFile, SIGNAL(clicked(bool)), this, SLOT(slotLoadSTDataFile()));
    connect(m_ui->loadSpotMapFile, SIGNAL(clicked(bool)), this, SLOT(slotLoadSpotsMapFile()));
    connect(m_ui->loadMainImageFile, SIGNAL(clicked(bool)), this, SLOT(slotLoadMainImageFile()));
    connect(m_ui->loadImageAlignmentFile,
            SIGNAL(clicked(bool)),
            this,
            SLOT(slotLoadAlignmentFile()));
    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotValidateForm()));
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

const QByteArray DatasetImporter::STDataFile() const
{
    QFile file(m_ui->stDataFile->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening ST data file " << file.errorString();
        return QByteArray();
    }
    return file.readAll();
}

const QByteArray DatasetImporter::mainImageFile() const
{
    QFile file(m_ui->mainImageFile->text());
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }
    return file.readAll();
}

const QTransform DatasetImporter::alignmentMatrix() const
{
    float a11 = 1.0;
    float a12 = 0.0;
    float a13 = 0.0;
    float a21 = 0.0;
    float a22 = 1.0;
    float a23 = 0.0;
    float a31 = 0.0;
    float a32 = 0.0;
    float a33 = 1.0;

    QFile file(m_ui->imageAlignmentFile->text());
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        if (fields.length() == 9) {
           a11 = fields.at(0).toFloat();
           a12 = fields.at(1).toFloat();
           a13 = fields.at(2).toFloat();
           a21 = fields.at(3).toFloat();
           a22 = fields.at(4).toFloat();
           a23 = fields.at(5).toFloat();
           a31 = fields.at(6).toFloat();
           a32 = fields.at(7).toFloat();
           a33 = fields.at(8).toFloat();
        } else {
            qDebug() << "Error parsing alignment matrix";
        }
    }
    file.close();

    return QTransform(a11, a12, a13, a21, a22, a23, a31, a32, a33);
}

const QByteArray DatasetImporter::spotsMapFile() const
{
    QFile file(m_ui->spotMapFile->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening Spots Map file " << file.errorString();
        return QByteArray();
    }
    return file.readAll();
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
        m_ui->stDataFile->insert(filename);
    }
}

void DatasetImporter::slotLoadMainImageFile()
{
    const QString filename
        = QFileDialog::getOpenFileName(this,
                                       tr("Open Main Image File"),
                                       QDir::homePath(),
                                       QString("%1").arg(tr("JPEG Files (*.jpg)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("Main Image File"), tr("File is incorrect or not readable"));
    } else {
        m_ui->mainImageFile->insert(filename);
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
        m_ui->spotMapFile->insert(filename);
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
        m_ui->imageAlignmentFile->insert(filename);
    }
}

void DatasetImporter::slotValidateForm()
{
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
        QDialog::done(QDialog::Rejected);
        // TODO maybe we should not close the dialog
    } else {
        QDialog::done(QDialog::Accepted);
    }
}
