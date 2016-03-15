#ifndef DATASETIMPORTER_H
#define DATASETIMPORTER_H

#include <QDialog>

namespace Ui {
class DatasetImporter;
}

class DatasetImporter : public QDialog
{
    Q_OBJECT

public:
    explicit DatasetImporter(QWidget *parent = 0);
    ~DatasetImporter();

    const QString datasetName();
    const QByteArray featuresFile();
    const QRect chipDimensions();
    const QTransform alignmentMatrix();
    const QByteArray mainImageFile();
    const QByteArray secondImageFile();

private slots:

    void slotLoadFeaturesFile();
    void slotLoadMainImageFile();
    void slotLoadSecondImageFile();
    void slotValidateForm();

private:
    std::unique_ptr<Ui::DatasetImporter> m_ui;
};

#endif // DATASETIMPORTER_H
