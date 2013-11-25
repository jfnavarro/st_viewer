/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ImageProcessing.h"

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include <utils/QGLExtended.h>

#include <QIODevice>
#include <QImageReader>

namespace async
{

ImageRequest *ImageProcess::createOpenGLImage(QIODevice *device)
{
    DEBUG_FUNC_NAME
    ImageRequest *request = new ImageRequest();
    //create and connect the future watcher to the slot
    QFutureWatcher<TransformedImage> *watcher = new QFutureWatcher<TransformedImage>(request);
    QObject::connect(watcher, SIGNAL(finished()), request, SLOT(slotImageFinished()));
    //create the future and run the function in a thread
    QFuture<TransformedImage> future;
    future = QtConcurrent::run(&ImageProcess::convertToGLFormat, device);
    watcher->setFuture(future);
    //return the request object
    return request;
}

ImageProcess::TransformedImage ImageProcess::convertToGLFormat(QIODevice *device)
{
    DEBUG_FUNC_NAME
    //read input and create impate
    QImageReader reader(device);
    QImage image = reader.read();
    //deallocate device
    device->deleteLater();
    // early out
    if (image.isNull()) {
        return TransformedImage(QImage(), QTransform());
    }
    //create openGL image and scale down if needed
    QImage openglImage;
    QTransform openglTransform;
    // try to convert image (using downsampling 8 times if needed)
    for (int i = 0; (openglImage.isNull() && (i < 8)); ++i) {
        // scale if needed
        if (i != 0) {
            qDebug() << QString("Downsampling image by factor of %1!").arg(i + 1);
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

void ImageRequest::slotImageFinished()
{
    QFutureWatcher<ImageProcess::TransformedImage> *watcher =
            dynamic_cast<QFutureWatcher<ImageProcess::TransformedImage>*>(sender());
    m_image = watcher->result().first;
    m_transform = watcher->result().second;
    emit signalFinished();
}

} // namespace async //
