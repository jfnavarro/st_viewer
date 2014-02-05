/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef WIDGETBACKGROUNDANIMATION_H
#define WIDGETBACKGROUNDANIMATION_H

#include <QObject>
#include <QString>

class QWidget;
class QMovie;
class QPaintEvent;

// Decorator class adding an animated image to a Qt widget.
class WidgetBackgroundAnimation : public QObject
{
    Q_OBJECT

public:

    WidgetBackgroundAnimation(QWidget *displayWidget = 0);
    virtual ~WidgetBackgroundAnimation();

    void paintAnimation(QPaintEvent *event);

    bool enabled() const;
    bool visible() const;

public slots:

    void setAnimation(const QString &fileName);
    void setVisible(const bool visible);
    void setEnabled(const bool enabled);

protected:

    QWidget *m_displayWidget;
    QMovie *m_movie;
    bool m_visible;
    bool m_enable;
};

#endif // WIDGETBACKGROUNDANIMATION_H //
