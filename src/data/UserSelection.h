#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <QString>
#include "STData.h"
#include "Spot.h"

// UserSelection represents a selection of spots made by the user trough the UI.
// Users can select spots manually (lazo, rubberband ..) or by using the selection search
// box with specific gene names (reg-exp).
class UserSelection
{

public:

    typedef QList<QString> GeneListType;
    typedef QList<Spot::SpotType> SpotListType;
    typedef STData::Matrix Matrix;

    explicit UserSelection(QSharedPointer<STData> data);
    UserSelection(const UserSelection &other);
    ~UserSelection();

    UserSelection &operator=(const UserSelection &other);
    bool operator==(const UserSelection &other) const;

    // the name of the selection
    const QString name() const;
    // the name of the dataset where the selection has been made
    const QString dataset() const;
    // the data matrix of counts
    const STData::Matrix &data() const;
    // the genes
    const GeneListType &genes() const;
    // the spots
    const SpotListType &spots() const;
    // some meta-data
    const QString comment() const;

    // obtained from the data object
    int totalGenes() const;
    int totalSpots() const;

    // Setters
    void name(const QString &name);
    void dataset(const QString &dataset);
    void comment(const QString &comment);

    // import/export
    void save(const QString filename) const;
    void load(const QString filename);

private:
    void init(QSharedPointer<STData> data);
    QString m_name;
    QString m_dataset;
    Matrix m_counts;
    QList<QString> m_genes;
    QList<Spot::SpotType> m_spots;
    QString m_comment;
};

#endif // USERSELECTION_H
