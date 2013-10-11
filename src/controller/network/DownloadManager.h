/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QPointer>
#include <QObject>

// this object is a simple wrapper for network reply to handle
// asynchronus downloads 
class Error;
class NetworkReply;

namespace async
{
    // Asynchronous data request handle used to synchronize request events
    // (such as finished, error etc.).
   
    class DataRequest : public QObject 
    {
        Q_OBJECT
        
    public:
        
        enum Code
        {
            CodeSuccess = 0x01,
            CodeAbort = 0x02,
            CodeError = 0x04,
            CodeTimedOut = 0x08,
            CodePresent = 0x16
        };
        Q_DECLARE_FLAGS(ReturnCodes, Code); 
        
        explicit DataRequest(QObject* parent = 0);
        virtual ~DataRequest();
        
        inline void return_code(Code code){m_return_code = code;}
        inline ReturnCodes return_code(){return m_return_code;}
        
    public slots:
        
        //this is to be used from a page
        void slotAbort();
        //this is to be used from the download manager
        void slotFinished();
        // this is to be used from the download manager
        void slotError(Error*);
        
    signals:
        
        void signalFinished();
        
        void signalAbort();
        
        void signalError(Error*);
        
    private:
        
        ReturnCodes m_return_code;
        
    };
    
    class DownloadManager: public QObject
    {

        Q_OBJECT
        
    public:
        
        DownloadManager(async::DataRequest *request, QObject * parent = 0);
        
        virtual ~DownloadManager();
        
        void addItem(NetworkReply *item);
        
        void delItem(NetworkReply *item);
        
        void addError(Error *error);
        
        unsigned countItems() const;
        unsigned countErrors() const;

        //check errors and replies left and send signal to DataRequest
        void finish();
        
        //start a timer that will call finish after some seconds
        void start();
        
    public slots:
        
        void timedOut();
        void slotAbort();
        
    private: 
        
        QPointer<async::DataRequest> m_request;
        QList<Error*> m_error_list;
        QList<NetworkReply*> m_reply_list;    
    };

} // namespace async //

#endif	/* // DOWNLOADMANAGER_H */
