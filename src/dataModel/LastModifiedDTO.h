#ifndef LASTMODIFIEDDTO_H
#define LASTMODIFIEDDTO_H

#include <QObject>

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// This DTO parses the last_modified endpoints

// TODO move definitions to CPP and/or consider removing DTOs
class LastModifiedDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString last_modified READ lastModified WRITE lastModified)

public:
    explicit LastModifiedDTO(QObject *parent = 0)
        : QObject(parent)
    {
    }
    ~LastModifiedDTO() {}

    // binding
    const QString &lastModified() const { return m_lastModified; }

    // getters
    void lastModified(const QString &lastModified) { m_lastModified = lastModified; }

private:
    QString m_lastModified;
};

#endif // LASTMODIFIEDDTO_H
