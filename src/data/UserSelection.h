#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <QString>
#include "STData.h"

// UserSelection represents a selection of spots made by the user trough the UI.
// Users can select spots manually (lazo, rubberband ..) or trough the AnalysisClustering
// Widget that allows to export clusters as selections. Selections can be exported/imported
// A selection is basically a wrapper around a dataset with metainfo and now image
class UserSelection
{

public:

    typedef STData::STDataFrame STDataFrame;

    UserSelection();
    explicit UserSelection(const STData::STDataFrame &data);
    UserSelection(const UserSelection &other);
    ~UserSelection();

    UserSelection &operator=(const UserSelection &other);
    bool operator==(const UserSelection &other) const;

    // the name of the selection
    const QString name() const;

    // the name of the dataset where the selection has been made
    const QString dataset() const;

    // the ST data object
    const STData::STDataFrame &data() const;

    // some meta-data
    const QString comment() const;

    // obtained from the data object
    int totalGenes() const;
    int totalSpots() const;

    // Setters
    void name(const QString &name);
    void dataset(const QString &dataset);
    void comment(const QString &comment);
    void data(const STData::STDataFrame &data);

private:

    QString m_name;
    QString m_dataset;
    STData::STDataFrame m_data;
    QString m_comment;
};

#endif // USERSELECTION_H
