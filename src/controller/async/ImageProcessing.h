/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QImage>

namespace async
{

class ImageRequest;

// Wrapper class encapsulating asynchronous functions to load and convert an image to an OpenGL friendly format.
// The class relies on an ImageRequest object to provide a link between the caller and the operation.
// NOTE Due to the share size of the image the class will downsample the converted image (if needed) so as to
//      avoid out of exceptions. The applied transformation is stored in the transform member variable.
class ImageProcess
{
public:

    // Load given image from stream and convert it to an OpenGL friendly format.
    // Returns an ImageRequest object which provides a handle to the async request.
    static ImageRequest *createOpenGLImage(QIODevice *device);

    // result is returned as a QImage, QTransform touple
    typedef QPair<QImage, QTransform> TransformedImage;

    static TransformedImage convertToGLFormat(QIODevice *device);
};

class ImageRequest : public QObject
{
    Q_OBJECT

public:

    explicit ImageRequest(QObject *parent = 0);
    virtual ~ImageRequest();

    inline const QImage &image() const { return m_image; }
    inline const QTransform &transform() const {return m_transform; }

private slots:

    void slotImageFinished();

signals:

    void signalFinished();

private:

    //NOTE OpenGL formatted image, may be huge so dont forget to delete ImageRequest
    QImage m_image;         // image data
    QTransform m_transform; // transformation data (scaling)
};

} // namespace async //

#endif // IMAGEPROCESSING_H //
