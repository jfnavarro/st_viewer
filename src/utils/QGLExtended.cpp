/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "QGLExtended.h"

namespace QtExt
{

static void convertToGLFormatHelper(QImage &dst, const QImage &img, GLenum texture_format)
{
    // early out
    if (dst.isNull() || img.isNull()) {
        dst = QImage();
        return;
    }
    Q_ASSERT(dst.depth() == 32);
    Q_ASSERT(img.depth() == 32);

    if (dst.size() != img.size()) {
        int target_width = dst.width();
        int target_height = dst.height();
        qreal sx = target_width / qreal(img.width());
        qreal sy = target_height / qreal(img.height());
        quint32 *dest = (quint32 *) dst.scanLine(0); // NB! avoid detach here
        uchar *srcPixels = (uchar *) img.scanLine(img.height() - 1);
        int sbpl = img.bytesPerLine();
        int dbpl = dst.bytesPerLine();
        int ix = int(0x00010000 / sx);
        int iy = int(0x00010000 / sy);
        quint32 basex = int(0.5 * ix);
        quint32 srcy = int(0.5 * iy);
        // scale, swizzle and mirror in one loop
        while (target_height--) {
            const uint *src = (const quint32 *)(srcPixels - (srcy >> 16) * sbpl);
            int srcx = basex;
            for (int x = 0; x < target_width; ++x) {
                uint src_pixel = src[srcx >> 16];
                if (texture_format == GL_BGRA) {
                    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                        dest[x] = ((src_pixel << 24) & 0xff000000)
                                  | ((src_pixel >> 24) & 0x000000ff)
                                  | ((src_pixel << 8) & 0x00ff0000)
                                  | ((src_pixel >> 8) & 0x0000ff00);
                    } else {
                        dest[x] = src_pixel;
                    }
                } else {  // GL_RGBA
                    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                        dest[x] = (src_pixel << 8) | ((src_pixel >> 24) & 0xff);
                    } else {
                        dest[x] = ((src_pixel << 16) & 0xff0000)
                                  | ((src_pixel >> 16) & 0xff)
                                  | (src_pixel & 0xff00ff00);
                    }
                }
                srcx += ix;
            }
            dest = (quint32 *)(((uchar *) dest) + dbpl);
            srcy += iy;
        }
    } else {
        const int width = img.width();
        const int height = img.height();
        const uint *p = (const uint*) img.scanLine(img.height() - 1);
        uint *q = (uint*) dst.scanLine(0);
        if (texture_format == GL_BGRA) {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                // mirror + swizzle
                for (int i = 0; i < height; ++i) {
                    const uint *end = p + width;
                    while (p < end) {
                        *q = ((*p << 24) & 0xff000000)
                             | ((*p >> 24) & 0x000000ff)
                             | ((*p << 8) & 0x00ff0000)
                             | ((*p >> 8) & 0x0000ff00);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            } else {
                const uint bytesPerLine = img.bytesPerLine();
                for (int i = 0; i < height; ++i) {
                    memcpy(q, p, bytesPerLine);
                    q += width;
                    p -= width;
                }
            }
        } else {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                for (int i = 0; i < height; ++i) {
                    const uint *end = p + width;
                    while (p < end) {
                        *q = (*p << 8) | ((*p >> 24) & 0xff);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            } else {
                for (int i = 0; i < height; ++i) {
                    const uint *end = p + width;
                    while (p < end) {
                        *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            }
        }
    }
}

const QImage convertToGLFormat(const QImage& img)
{
    QImage res(img.size(), QImage::Format_ARGB32);
    if (!res.isNull()) {
        convertToGLFormatHelper(res, img.convertToFormat(QImage::Format_ARGB32), GL_RGBA);
    }
    return res;
}

} // namespace QtExt //
