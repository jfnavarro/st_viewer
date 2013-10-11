/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef STRINGLINK_H
#define STRINGLINK_H

#include <QObject>

// The link classes provide convenience methods for interpreting data between
// signals and slots in QT. StringLink adds functionality to generate new
// signals based on strings from external <function>(QString) signals.
// Eg. the signalIsEmpty(bool) is emitted when the input string is empty.
class StringLink : public QObject
{
   Q_OBJECT

public:
    
    explicit StringLink(QObject* parent = 0);
    virtual ~StringLink();

signals:
	// this signal is emitted when the input string is empty
    void signalIsEmpty(bool empty);

public slots:
    // connect the original string to this slot 
    void slotString(const QString& string);
};

#endif // STRINGLINK_H //
