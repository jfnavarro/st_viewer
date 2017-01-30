#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <STData.h>
#include <QString>

// Gene selection represents a selection of spots made by the user trough the UI.
// Users can select spots manually (lazo, rubberband ..) or by using the selection search
// box with specific gene names
class UserSelection
{

public:

    enum Type { Rubberband = 1, Lazo = 2, Segmented = 3, Console = 4, Cluster = 5, Other = 6 };
    Q_DECLARE_FLAGS(Types, Type)

    UserSelection();
    explicit UserSelection(const UserSelection &other);
    ~UserSelection();

    UserSelection &operator=(const UserSelection &other);
    bool operator==(const UserSelection &other) const;

    // the name of the selection
    const QString name() const;
    // the name of the dataset where the selection has been made
    const QString dataset() const;
    // the list of selected spots
    const STData::spot_list selectedSpots() const;
    const STData::gene_list selectedGenes() const;
    // the list
    // some metat-data
    const QString comment() const;
    // the type of the selection
    Type type() const;
    // a image snapshot of the tissue taken when the selection was made
    const QByteArray tissueSnapShot() const;
    // returns the total sum of reads in the selection
    int totalReads() const;
    // returns the total number of genes in the selection
    int totalGenes() const;
    // returns the total number of spots in the selection
    int totalSpots() const;

    // Setters
    void name(const QString &name);
    void dataset(const QString &dataset);
    void selectedSpots(const STData::spot_list &spots);
    void selectedGenes(const STData::gene_list &genes);
    void comment(const QString &comment);
    void type(const Type &type);
    void tissueSnapShot(const QByteArray &tissueSnapShot);
    void totalReads(const int totalReads);

    static const QString typeToQString(const UserSelection::Type &type);
    static UserSelection::Type QStringToType(const QString &type);
private:
    QString m_name;
    QString m_dataset;
    STData::spot_list m_selectedSpots;
    STData::gene_list m_selectedGenes;
    Type m_type;
    QString m_comment;
    QByteArray m_tissueSnapShot;
    int m_totalReads;
};

#endif // USERSELECTION_H
