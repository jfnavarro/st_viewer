/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "utils/Utils.h"

#if defined(Q_OS_WIN) || defined(_WIN32)
#define WIN32_LEAN_AND_CLEAN
#include <windows.h>
#endif // _WIN32 //

namespace Utils
{
    QDateTime decodeJsonDateTime(const QString &jsonDateTime)
    {
        if (jsonDateTime.contains('.')
            && (jsonDateTime.contains('E') || jsonDateTime.contains('e')))
        {
            int indexOfDot = jsonDateTime.indexOf('.');
            if (indexOfDot >= 0)
            {
                int indexOfE = jsonDateTime.indexOf('E', indexOfDot + 1);
                if (indexOfE < 0)
                {
                    indexOfE = jsonDateTime.indexOf('e', indexOfDot + 1);
                }
                if (indexOfE > 0)
                {
                    quint64 res = 0;
                    int i = 0;
                    for (; i < indexOfDot; i++)
                    {
                        QChar ch = jsonDateTime.at(i);
                        if (ch.isDigit())
                        {
                            res *= 10;
                            res += (ch.unicode() - '0');
                        }
                        else
                        {
                            //invalid format
                            qDebug() << "Invalid date time format";
                            return QDateTime();
                        }
                    }
                    int tenths = 0;
                    bool ok = 0;
                    tenths = jsonDateTime.right(jsonDateTime.length() - indexOfE - 1).toInt(&ok);
                    if (ok)
                    {
                        i++;
                        for (; /*tenths >0 &&*/ i < indexOfE; i++, tenths--)
                        {
                            QChar ch = jsonDateTime.at(i);
                            if (ch.isDigit())
                            {
                                res *= 10;
                                res += (ch.unicode() - '0');
                            }
                            else
                            {
                                //invalid format
                                qDebug() << "Invalid date time format";
                                return QDateTime();
                            }
                        }
                        int d = (indexOfE - indexOfDot);
                        if ((d > 0) && (d < 13))
                        {
                            d = 13 - d;
                            for ( ; d > 0; d--)
                            {
                                res *= 10;
                            }
                        }
                        QDateTime result;
                        result.setMSecsSinceEpoch(res);
                        return result;
                    }
                }
            }
        }
        return QDateTime();
    }
    
    void char2hex( QChar dec, QString &str )
    {
        char dig1 = (dec.toLatin1() & 0xF0)>>4;
        char dig2 = (dec.toLatin1() & 0x0F);
        
        if ( 0<= dig1 && dig1<= 9) dig1 += '0';    //0,48inascii
        if (10<= dig1 && dig1<=15) dig1 += ('A' - 10); //a,97inascii
        if ( 0<= dig2 && dig2<= 9) dig2 += '0';
        if (10<= dig2 && dig2<=15) dig2 += ('A' - 10);
        
        str.append( dig1);
        str.append( dig2);
    }
    
    QString urlEncode(const QByteArray &c)
    {
        
        QString escaped="";
        int max = c.length();
        for(int i=0; i<max; i++)
        {
            if ( (48 <= c[i] && c[i] <= 57) ||//0-9
                (65 <= c[i] && c[i] <= 90) ||//abc...xyz
                (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
                (c[i]=='~' || c[i]=='(' || c[i]==')')
            )
            {
                escaped.append(c.at(i));
            }
            else if (c[i] == 0x20)
            {
                escaped.append('+');
            }
            else
            {
                escaped.append("%");
                char2hex(c.at(i), escaped);//converts char 255 to string "ff"
            }
        }
        return escaped;
    }
    
    QString urlEncode(const QString &c)
    {
        
        QString escaped="";
        int max = c.length();
        for(int i=0; i<max; i++)
        {
            if ( (48 <= c[i] && c[i] <= 57) ||//0-9
                (65 <= c[i] && c[i] <= 90) ||//abc...xyz
                (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
                (c[i]=='~' || c[i]=='(' || c[i]==')')
            )
            {
                escaped.append(c.at(i).toLatin1());
            }
            else if (c[i] == 0x20)
            {
                escaped.append('+');
            }
            else
            {
                escaped.append("%");
                char2hex(c.at(i), escaped);//converts char 255 to string "ff"
            }
        }
        return escaped;
    }
    
    QString formatStorage(qlonglong storageSpace)
    {
        QString result;
        if (storageSpace >= (1024 * 1024 * 1024))
        {
            double gigaStorage = (float)storageSpace / (1024.0f * 1024.0f * 1024.0f);
            result = QString("%1 GB").arg(gigaStorage, 0, 'f', 1);
        }
        else if (storageSpace >= (1024 * 1024))
        {
            double megaStorage = (float)storageSpace / (1024.0f * 1024.0f);
            result = QString("%1 MB").arg(megaStorage, 0, 'f', 1);
        }
        else if (storageSpace >= 1024)
        {
            double kiloStorage = (float)storageSpace / (1024.0f);
            result = QString("%1 KB").arg(kiloStorage, 0, 'f', 1);
        }
        else
        {
            result = QString("%1").arg(storageSpace);
        }
        return result;
    }
    
    //Returns the size of physical memory (RAM) in bytes.
    size_t getMemorySize()
    {
        #if defined(_WIN32) && (defined(__CYGWIN__) || defined(__CYGWIN32__))
        //Cygwin under Windows
        MEMORYSTATUS status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatus(&status);
        return (size_t)status.dwTotalPhys;
        
        #elif defined(_WIN32)
        //Windows
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return (size_t)status.ullTotalPhys;
        
        #elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
        // UNIX variants.   
        #if defined(CTL_HW) && (defined(HW_MEMSIZE) || defined(HW_PHYSMEM64))
        int mib[2];
        mib[0] = CTL_HW;
        #if defined(HW_MEMSIZE)
        mib[1] = HW_MEMSIZE;            // OSX. 
        #elif defined(HW_PHYSMEM64)
        mib[1] = HW_PHYSMEM64;          // NetBSD, OpenBSD. 
        #endif
        int64_t size = 0;               // 64-bit 
        size_t len = sizeof(size);
        if(sysctl(mib, 2, &size, &len, NULL, 0) == 0)
        {
            return (size_t)size;
        }
        return 0L;  //failed
        
        #elif defined(_SC_AIX_REALMEM)
        // AIX. 
        return (size_t)sysconf(_SC_AIX_REALMEM) * (size_t)1024L;
        
        #elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
        // FreeBSD, Linux, OpenBSD, and Solaris
        return (size_t)sysconf(_SC_PHYS_PAGES) *
        (size_t)sysconf(_SC_PAGESIZE);
        
        #elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGE_SIZE)
        // Legacy.
        return (size_t)sysconf(_SC_PHYS_PAGES) *
        (size_t)sysconf(_SC_PAGE_SIZE);
        
        #elif defined(CTL_HW) && (defined(HW_PHYSMEM) || defined(HW_REALMEM))
        // DragonFly BSD, FreeBSD, NetBSD, OpenBSD, and OSX.
        int mib[2];
        mib[0] = CTL_HW;
        #if defined(HW_REALMEM)
        mib[1] = HW_REALMEM;        // FreeBSD
        #elif defined(HW_PYSMEM)
        mib[1] = HW_PHYSMEM;        // Others
        #endif
        unsigned int size = 0;      // 32-bit 
        size_t len = sizeof( size );
        if (sysctl(mib, 2, &size, &len, NULL, 0) == 0)
        {
            return (size_t)size;
        }
        return 0L; //failed
        #endif //sysctl and sysconf variants 
        
        #else
        return 0L; //Unknown OS. 
        #endif
    }
}