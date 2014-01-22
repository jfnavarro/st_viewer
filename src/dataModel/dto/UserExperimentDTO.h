/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef USEREXPERIMENTDTO_H
#define USEREXPERIMENTDTO_H

#include <QObject>
#include <QString>

#include "model/UserExperiment.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// UserExperimentDTO defines the parsing object for the underlying
// UserExperiment data object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class UserExperimentDTO : public QObject
{

public:

    Q_OBJECT

    Q_PROPERTY(QString datasetId READ datasetId WRITE datasetId)
    Q_PROPERTY(QString userId READ userId WRITE userId)
    Q_PROPERTY(QString type READ type WRITE type)
    Q_PROPERTY(QString inputData READ inputData WRITE inputData)
    Q_PROPERTY(QString outputData READ outputData WRITE outputData)

public:

    explicit UserExperimentDTO(QObject* parent = 0);
    UserExperimentDTO(const UserExperiment& userExperiment, QObject* parent = 0);
    virtual ~UserExperimentDTO();

    // binding
    inline const QString& datasetId() const { return m_userExperiment.datasetId(); }
    inline const QString& userId() const { return m_userExperiment.userId(); }
    inline const QString& type() const { return m_userExperiment.type(); }
    inline const QString& inputData() const { return m_userExperiment.inputData(); }
    inline const QString& outputData() const { return m_userExperiment.outputData(); }

    inline void datasetId(QString datasetId) { m_userExperiment.datasetId(datasetId); }
    inline void userId(QString userId) { m_userExperiment.userId(userId); }
    inline void type(const QString& type) { m_userExperiment.type(type); }
    inline void inputData(const QString& inputData) { m_userExperiment.inputData(inputData); }
    inline void outputData(const QString& outputData) { m_userExperiment.outputData(outputData); }

    // get parsed data model
    inline const UserExperiment& userExperiment() const { return m_userExperiment; }
    inline UserExperiment& userExperiment() { return m_userExperiment; }

private:

    UserExperiment m_userExperiment;
};

#endif // USEREXPERIMENTDTO_H //
