/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef USER_EXPERIMENT_H
#define USER_EXPERIMENT_H

#include <QString>

// Data model class to store user experiment data.
class UserExperiment
{

public:

    UserExperiment();
    explicit UserExperiment(const UserExperiment& other);
    explicit UserExperiment(const QString& datasetId, const QString& userId, const QString& type,
                   const QString& inputData, const QString& outputData);
    
    virtual ~UserExperiment();

    UserExperiment& operator=(const UserExperiment& other);
    bool operator==(const UserExperiment& other) const;

     const QString& datasetId() const { return m_datasetId; }
     const QString& userId() const { return m_userId; }
     const QString& type() const { return m_type; }
     const QString& inputData() const { return m_inputData; }
     const QString& outputData() const { return m_outputData; }

     void datasetId(const QString& datasetId) { m_datasetId = datasetId; }
     void userId(const QString& userId) { m_userId = userId; }
     void type(const QString& type) { m_type = type; }
     void inputData(const QString& inputData) { m_inputData  = inputData; }
     void outputData(const QString& outputData) { m_outputData = outputData; }

private:

    QString m_datasetId;
    QString m_userId;
    QString m_type;
    QString m_inputData;
    QString m_outputData;
};

#endif // USER_EXPERIMENT_H
