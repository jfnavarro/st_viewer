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
// the tissue image and a file with spot coordinates
class DatasetImporter : public QDialog
{
    Q_OBJECT

public:
    explicit DatasetImporter(QWidget *parent = nullptr);
    DatasetImporter(const Dataset &dataset, QWidget *parent = nullptr);
    virtual ~DatasetImporter() override;

    const QString datasetName() const;
    const QString STDataFile() const;
    const QString spotsMapFile() const;
    const QString mainImageFile() const;
    const QString meshFile() const;
    const QString comments() const;
    double scalingFactor() const;
    bool is3D() const;

    // To import a dataset from a folder
    // the function assumes that
    // the image is called *.jpg
    // the data is called *.tsv
    // the aligment is called alignment.txt
    // the spots file is called spots.txt
    // the metadata is present in a JSON file called info.json
    void slotParseFolder();

    // To import a dataset from a metafile (JSON)
    // the file contains the path for each file of the datset
    // indentifiable with the key
    void slotParseMetaFile();

protected:

    void done(int result) override;

private slots:

    void slotLoadSTDataFile();
    void slotLoadMainImageFile();
    void slotLoadMeshFile();
    void slotLoadSpotsMapFile();
    void slotChange3D(int state);

private:
    void init();
    void parseInfoJSON(const QString &filename);
    QScopedPointer<Ui::DatasetImporter> m_ui;

};

#endif // DATASETIMPORTER_H
