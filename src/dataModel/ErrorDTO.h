/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ERRORDTO_H
#define ERRORDTO_H

#include <QObject>

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// This DTO is used to parse network errors

// TODO move definitions to CPP and/or consider removing DTOs
class ErrorDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString error READ errorName WRITE errorName)
    Q_PROPERTY(QString error_description READ errorDescription WRITE errorDescription)

public:
    explicit ErrorDTO(QObject* parent = 0)
        : QObject(parent)
        , m_errorName()
        , m_errorDescription()
    {
    }
    ~ErrorDTO() {}

    // binding
    const QString& errorName() const { return m_errorName; }
    const QString& errorDescription() const { return m_errorDescription; }

    void errorName(const QString& errorName) { m_errorName = errorName; }
    void errorDescription(const QString& errorDescription)
    {
        m_errorDescription = errorDescription;
    }

private:
    QString m_errorName;
    QString m_errorDescription;
};

#endif // ERRORDTO_H //
