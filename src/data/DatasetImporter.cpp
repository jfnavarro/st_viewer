#include "DatasetImporter.h"
#include "ui_datasetImporter.h"
#include <QFileDialog>
#include <QMessageBox>

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
    return QRect(QPoint(m_ui->chip_x1->value(), m_ui->chip_y1->value()),
                 QPoint(m_ui->chip_x2->value(), m_ui->chip_y2->value()));
}

const QTransform DatasetImporter::alignmentMatrix() const
{
    return QTransform(m_ui->alignment_a11->value(),
                      m_ui->alignment_a12->value(),
                      m_ui->alignment_a13->value(),
                      m_ui->alignment_a21->value(),
                      m_ui->alignment_a22->value(),
                      m_ui->alignment_a23->value(),
                      m_ui->alignment_a31->value(),
                      m_ui->alignment_a32->value(),
                      m_ui->alignment_a33->value());
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

void DatasetImporter::slotValidateForm()
{
    QString error_msg;
    bool isValid = true;
    if (m_ui->mainImageFile->text().isEmpty()) {
        isValid = false;
        error_msg = tr("Main image is missing!");
    } else if (m_ui->secondImageFile->text().isEmpty()) {
        isValid = false;
        error_msg = tr("Second image is missing!");
    } else if (m_ui->featuresFile->text().isEmpty()) {
        isValid = false;
        error_msg = tr("Features file is missing!");
    } else if (m_ui->datasetName->text().isEmpty()) {
        isValid = false;
        error_msg = tr("Dataset name is missing!");
    } else if (m_ui->chip_x2->value() == 0 || m_ui->chip_y2->value() == 0) {
        isValid = false;
        error_msg = tr("Chip values are invalid!");
    }

    if (!isValid) {
        QMessageBox::critical(this, tr("Import dataset"), error_msg);
        QDialog::done(QDialog::Rejected);
        // TODO maybe we should not close the dialog
    } else {
        QDialog::done(QDialog::Accepted);
    }
}
