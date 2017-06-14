#ifndef DATASETIMPORTER_H
#define DATASETIMPORTER_H

#include <QDialog>
#include <QDir>

namespace Ui
{
class DatasetImporter;
}

class Dataset;

// This widget allows the user to import a dataset.
// The widget asks the user to introduce the STData (matrix),
// the tissue image and optionally a 3x3 alignment matrix and
// a file mapping coordiantes to new coordinates
class DatasetImporter : public QDialog
{
    Q_OBJECT

public:
    DatasetImporter(QWidget *parent = 0);
    DatasetImporter(Dataset dataset, QWidget *parent = 0);
    ~DatasetImporter();

    const QString datasetName() const;
    const QString STDataFile() const;
    const QString alignmentMatrix() const;
    const QString spotsMapFile() const;
    const QString mainImageFile() const;
    const QString species() const;
    const QString tissue() const;
    const QString comments() const;

    // To import a dataset from a folder
    // the function assumes that
    // the image is called image.jpg
    // the data is called stdata.tsv
    // the aligment is called alignment.txt
    // the spots file is called spots.txt
    // the metadata is present in a JSON file called info.json
    void slotParseFolder();

private slots:

    void slotLoadSTDataFile();
    void slotLoadMainImageFile();
    void slotLoadSpotsMapFile();
    void slotLoadAlignmentFile();
    void slotValidateForm();

private:
    void init();
    QScopedPointer<Ui::DatasetImporter> m_ui;

};

#endif // DATASETIMPORTER_H
