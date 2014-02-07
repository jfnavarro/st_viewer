/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ERROR_H
#define ERROR_H

#include <QDebug>

#include <QObject>
#include <QString>

//To add a new error, the qHash() functions needs to be called with the error descfiption text, then add
//the error hash to the error enum class so it can be parsed and recognized, amm also add it to the translation file...

//this abstract class is the base class for errors, it contains different information ( level, type(hash), name and description)
class Error : public QObject
{
    Q_OBJECT

public:

    explicit Error(QObject* parent = 0);
    explicit Error(const QString& name, const QString& description = QString(), QObject* parent = 0);
    virtual ~Error();

    const QString& name() const;
    const QString& description() const;

protected:

    void name(const QString& name);
    void description(const QString& description);

private:

    QString m_name;
    QString m_description;
};

#endif // ERROR_H //
