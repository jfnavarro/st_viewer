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
    UserExperiment(const UserExperiment& other);
    UserExperiment(const QString& datasetId, const QString& userId, const QString& type,
                   const QString& inputData, const QString& outputData);
    ~UserExperiment();

    UserExperiment& operator=(const UserExperiment& other);
    bool operator==(const UserExperiment& other) const;

    inline const QString& datasetId() const
    {
        return m_datasetId;
    }
    inline const QString& userId() const
    {
        return m_userId;
    }
    inline const QString& type() const
    {
        return m_type;
    }
    inline const QString& inputData() const
    {
        return m_inputData;
    }
    inline const QString& outputData() const
    {
        return m_outputData;
    }

    inline void datasetId(const QString& datasetId)
    {
        m_datasetId = datasetId;
    }
    inline void userId(const QString& userId)
    {
        m_userId = userId;
    }
    inline void type(const QString& type)
    {
        m_type = type;
    }
    inline void inputData(const QString& inputData)
    {
        m_inputData  = inputData;
    }
    inline void outputData(const QString& outputData)
    {
        m_outputData = outputData;
    }

private:

    QString m_datasetId;
    QString m_userId;
    QString m_type;
    QString m_inputData;
    QString m_outputData;
};

#endif // USER_EXPERIMENT_H
