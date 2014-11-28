/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef QJPEGIMAGEREADER_H
#define QJPEGIMAGEREADER_H

#include <QImageIOHandler>

class QByteArray;
class QJpegHandlerPrivate;

//This class is an adaptation of an old Qt4 plugin
//to use jpeg-turbo to deal with JPEG images
//The original source is in https://github.com/special/qjpeg-turbo
//It has been adapted to be used as source files and to work on Qt5

//TODO Figure out how to add the license from qjpeg-turbo to this header
//TODO move to ext folder as it is an external piece of code
class QJpegImageReader : public QImageIOHandler
{

public:

    QJpegImageReader();
    ~QJpegImageReader();

    bool canRead() const;
    bool read(QImage *image);
    bool write(const QImage &image);

    QByteArray name() const;

    static bool canRead(QIODevice *device);

    QVariant option(ImageOption option) const;
    void setOption(ImageOption option, const QVariant &value);
    bool supportsOption(ImageOption option) const;

private:

    QJpegHandlerPrivate *d;

};

#endif // QJPEGIMAGEREADER_H
