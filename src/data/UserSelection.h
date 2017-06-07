#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <QString>
#include "STData.h"

// UserSelection represents a selection of spots made by the user trough the UI.
// Users can select spots manually (lazo, rubberband ..) or by using the selection search
// box with specific gene names (reg-exp).
class UserSelection
{

public:

    UserSelection();
    UserSelection(const UserSelection &other);
    ~UserSelection();

    UserSelection &operator=(const UserSelection &other);
    bool operator==(const UserSelection &other) const;

    // the name of the selection
    const QString name() const;
    // the name of the dataset where the selection has been made
    const QString dataset() const;
    // the data matrix of counts
    QSharedPointer<STData> data() const;
    // some meta-data
    const QString comment() const;

    // obtained from the data object
    int totalGenes() const;
    int totalReads() const;

    // Setters
    void name(const QString &name);
    void dataset(const QString &dataset);
    void data(const QSharedPointer<STData> data);
    void comment(const QString &comment);

private:
    QString m_name;
    QString m_dataset;
    QSharedPointer<STData> m_data;
    QString m_comment;
};

#endif // USERSELECTION_H
