/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ERROR_H
#define ERROR_H

#include <QObject>
#include <QString>

//this abstract class is the base class for errors,
//it contains different information (name and description)
class Error : public QObject
{
    Q_OBJECT

public:

    explicit Error(QObject* parent = 0);
    Error(const QString& name,
          const QString& description = QString(),
          QObject* parent = 0);
    virtual ~Error();

    const QString name() const;
    const QString description() const;

protected:

    void name(const QString& name);
    void description(const QString& description);

private:

    QString m_name;
    QString m_description;
};

#endif // ERROR_H //
