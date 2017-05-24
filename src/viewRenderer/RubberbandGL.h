#ifndef RUBBERBANDGL_H
#define RUBBERBANDGL_H

#include "GraphicItemGL.h"

class QRectF;

// RubberbandGL is a graphical item that visualizes a rubberband that is
// shown while the user is selecting genes.
class RubberbandGL : public GraphicItemGL
{
    Q_OBJECT

public:
    explicit RubberbandGL(QObject *parent = 0);
    virtual ~RubberbandGL();

    void setRubberbandRect(const QRectF &rect);
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

protected:
    const QRectF boundingRect() const override;

private:

    QRectF m_rubberbandRect;

    Q_DISABLE_COPY(RubberbandGL)
};

#endif // RUBBERBANDGL_H //
