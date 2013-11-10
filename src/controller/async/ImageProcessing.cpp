/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include <utils/QGLExtended.h>

#include <QIODevice>
#include <QImageReader>

#include "ImageProcessing.h"

namespace async
{

ImageRequest *ImageProcess::createOpenGLImage(QIODevice *device)
{
    DEBUG_FUNC_NAME

    ImageRequest *request = new ImageRequest();

    QFutureWatcher<TransformedImage> *watcher = new QFutureWatcher<TransformedImage>(request);
    QObject::connect(watcher, SIGNAL(finished()), request, SLOT(signalImageFinished()));

    QFuture<TransformedImage> future;
    future = QtConcurrent::run(&ImageProcess::convertToGLFormat, device);
    watcher->setFuture(future);

    return request;
}

ImageProcess::TransformedImage ImageProcess::convertToGLFormat(QIODevice *device)
{
    DEBUG_FUNC_NAME

    QImageReader reader(device);
    QImage image = reader.read();

    //deallocate device
    device->deleteLater();

    // early out
    if (image.isNull())
    {
        return TransformedImage(QImage(), QTransform());
    }

    QImage openglImage;
    QTransform openglTransform;
    // try to convert image (using downsampling 8 times if needed)
    for (int i=0; (openglImage.isNull() && (i<8)); ++i)
    {
        // scale if needed
        if (i != 0)
        {
            qDebug() << QString("Downsampling image by factor of %1!").arg(i+1);
            image = image.scaled(0.5 * image.size());          // scale down by half
            openglTransform = openglTransform.scale(2.0, 2.0); // apply inverse, ie. scale up by 2
        }
        // create opengl image
        openglImage = QtExt::convertToGLFormat(image);
    }

    return TransformedImage(openglImage, openglTransform);
}

ImageRequest::ImageRequest(QObject *parent) : QObject(parent)
{

}
ImageRequest::~ImageRequest()
{

}

void ImageRequest::signalImageFinished()
{
    QFutureWatcher<ImageProcess::TransformedImage> *watcher =
            dynamic_cast<QFutureWatcher<ImageProcess::TransformedImage>*>(sender());
    m_image = watcher->result().first;
    m_transform = watcher->result().second;
    emit signalFinished();
}




} // namespace async //
