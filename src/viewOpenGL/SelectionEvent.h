/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SELECTIONEVENT_H
#define SELECTIONEVENT_H

#include <QEvent>
#include <QPainterPath>

// Selection event used to propagate selection data to view items.
//TODO move definition to CPP
//TODO extend to allow other type of selections
class SelectionEvent : public QEvent
{

public:

    enum SelectionMode {
        NewSelection,
        IncludeSelection,
        ExcludeSelection
    };

    SelectionEvent() :
        QEvent(TYPE),
        m_path(),
        m_mode(NewSelection)
    {
    }

    SelectionEvent(const QRectF &rect,
                   const SelectionMode mode = NewSelection)
        : QEvent(TYPE),
          m_path(rect),
          m_mode(mode)
    {
    }

    QRectF path() const { return m_path; }
    SelectionMode mode() const { return m_mode; }

    static  SelectionMode modeFromKeyboardModifiers(Qt::KeyboardModifiers modifiers)
    {
        return
                (modifiers.testFlag(Qt::ShiftModifier) ?
                     (modifiers.testFlag(Qt::ControlModifier) ?
                          SelectionEvent::ExcludeSelection :
                          SelectionEvent::IncludeSelection
                          ) :
                     SelectionEvent::NewSelection
                     );
    }

private:

    static const QEvent::Type TYPE = static_cast<QEvent::Type>(QEvent::User + 42);

    const QRectF m_path;
    const SelectionMode m_mode;

    Q_DISABLE_COPY(SelectionEvent)
};

#endif // SELECTIONEVENT_H //
