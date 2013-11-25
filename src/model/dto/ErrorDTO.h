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

// ErrorDTO defines the parsing object for the underlying Error data object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class ErrorDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString error READ errorName WRITE errorName)
    Q_PROPERTY(QString error_description READ errorDescription WRITE errorDescription)

public:

    explicit ErrorDTO(QObject* parent = 0);
    virtual ~ErrorDTO();

    // binding
    inline const QString& errorName() const
    {
        return m_errorName;
    }
    inline const QString& errorDescription() const
    {
        return m_errorDescription;
    }

    inline void errorName(const QString& errorName)
    {
        m_errorName = errorName;
    }
    inline void errorDescription(const QString& errorDescription)
    {
        m_errorDescription = errorDescription;
    }

private:

    QString m_errorName;
    QString m_errorDescription;
};

#endif // ERRORDTO_H //
