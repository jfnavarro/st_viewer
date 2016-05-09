#ifndef DATASETIMPORTER_H
#define DATASETIMPORTER_H

#include <QDialog>

namespace Ui
{
class DatasetImporter;
}

// This widget allows the user to import a dataset.
// The widget asks the user to introduce the chip
// size, the alignment matrix, the features in JSON data
// and the images
class DatasetImporter : public QDialog
{
    Q_OBJECT

public:
    explicit DatasetImporter(QWidget *parent = 0);
    ~DatasetImporter();

    const QString datasetName() const;
    const QByteArray featuresFile() const;
    const QRect chipDimensions() const;
    const QTransform alignmentMatrix() const;
    const QByteArray mainImageFile() const;
    const QByteArray secondImageFile() const;
    const QString species() const;
    const QString tissue() const;
    const QString comments() const;

private slots:

    void slotLoadFeaturesFile();
    void slotLoadMainImageFile();
    void slotLoadSecondImageFile();
    void slotValidateForm();

private:
    QScopedPointer<Ui::DatasetImporter> m_ui;
};

#endif // DATASETIMPORTER_H
