#include "DatasetImporter.h"
#include "ui_datasetImporter.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

DatasetImporter::DatasetImporter(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::DatasetImporter)
{
    m_ui->setupUi(this);
    connect(m_ui->loadFeaturesFile, SIGNAL(clicked(bool)), this, SLOT(slotLoadFeaturesFile()));
    connect(m_ui->loadMainImageFile, SIGNAL(clicked(bool)), this, SLOT(slotLoadMainImageFile()));
    connect(m_ui->loadSecondImageFile,
            SIGNAL(clicked(bool)),
            this,
            SLOT(slotLoadSecondImageFile()));
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

const QByteArray DatasetImporter::featuresFile() const
{
    QFile file(m_ui->featuresFile->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
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

const QByteArray DatasetImporter::secondImageFile() const
{
    QFile file(m_ui->secondImageFile->text());
    if (!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }
    return file.readAll();
}

const QRect DatasetImporter::chipDimensions() const
{
    //NOTE hardcoded for now..
    return QRect(QPoint(2, 2),
                 QPoint(32, 34));
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
        while(!in.atEnd()) {
            QString line = in.readLine();
            // TODO check it is only one line and 9 columns
            QStringList fields = line.split(" ");
            a11 = fields.at(0).toFloat();
            a12 = fields.at(1).toFloat();
            a13 = fields.at(2).toFloat();
            a21 = fields.at(3).toFloat();
            a22 = fields.at(4).toFloat();
            a23 = fields.at(5).toFloat();
            a31 = fields.at(6).toFloat();
            a32 = fields.at(7).toFloat();
            a33 = fields.at(8).toFloat();
        }
    }
    file.close();

    return QTransform(a11, a12, a13, a21, a22, a23, a31, a32, a33);
}

void DatasetImporter::slotLoadFeaturesFile()
{
    const QString filename
        = QFileDialog::getOpenFileName(this,
                                       tr("Open Features File"),
                                       QDir::homePath(),
                                       QString("%1").arg(tr("JSON Files (*.json)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("Features File"), tr("File is incorrect or not readable"));
    } else {
        m_ui->featuresFile->insert(filename);
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

void DatasetImporter::slotLoadSecondImageFile()
{
    const QString filename
        = QFileDialog::getOpenFileName(this,
                                       tr("Open Second Image File"),
                                       QDir::homePath(),
                                       QString("%1").arg(tr("JPEG Files (*.jpg)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Second Image File"),
                              tr("File is incorrect or not readable"));
    } else {
        m_ui->secondImageFile->insert(filename);
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
    } else if (m_ui->featuresFile->text().isEmpty()) {
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
