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
    // the sliced data
    const STData& slicedData() const;
    // some metat-data
    const QString comment() const;
    // the type of the selection
    Type type() const;
    // a image snapshot of the tissue taken when the selection was made
    const QByteArray tissueSnapShot() const;

    // Setters
    void name(const QString &name);
    void dataset(const QString &dataset);
    void slicedData(const STData &data);
    void comment(const QString &comment);
    void type(const Type &type);
    void tissueSnapShot(const QByteArray &tissueSnapShot);

    // convenience functions to convert Type to QString and vice-versa
    static const QString typeToQString(const UserSelection::Type &type);
    static UserSelection::Type QStringToType(const QString &type);

private:
    QString m_name;
    QString m_dataset;
    STData m_sliced_data;
    Type m_type;
    QString m_comment;
    QByteArray m_tissueSnapShot;
};

#endif // USERSELECTION_H
