#ifndef DATASETIMPORTER_H
#define DATASETIMPORTER_H

#include <QDialog>

namespace Ui
{
class DatasetImporter;
}

// This widget allows the user to import a dataset.
// The widget asks the user to introduce the STData (matrix),
// the cell tissue image and optionally a 3x3 alignment matrix
//TODO factor out the file open dialog
class DatasetImporter : public QDialog
{
    Q_OBJECT

public:
    explicit DatasetImporter(QWidget *parent = 0);
    ~DatasetImporter();

    const QString datasetName() const;
    const QByteArray STDataFile() const;
    const QTransform alignmentMatrix() const;
    const QByteArray mainImageFile() const;
    const QString species() const;
    const QString tissue() const;
    const QString comments() const;

private slots:

    void slotLoadSTDataFile();
    void slotLoadMainImageFile();
    void slotLoadAlignmentFile();
    void slotValidateForm();

private:
    QScopedPointer<Ui::DatasetImporter> m_ui;
};

#endif // DATASETIMPORTER_H
