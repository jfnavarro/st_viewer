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

    explicit SelectionEvent(const QPainterPath &path, const SelectionMode mode = NewSelection)
        : QEvent(TYPE),
          m_path(path),
          m_mode(mode)
    {
    }

    QPainterPath path() const { return m_path; }
    SelectionMode mode() const { return m_mode; }

    static  SelectionMode modeFromKeyboardModifiers(Qt::KeyboardModifiers modifiers)
    {
#if defined Q_OS_MAC
        return
                (modifiers.testFlag(Qt::ShiftModifier) ?
                     (modifiers.testFlag(Qt::MetaModifier) ?
                          SelectionEvent::ExcludeSelection :
                          SelectionEvent::IncludeSelection
                          ) :
                     SelectionEvent::NewSelection
                     );
#else
        return
                (modifiers.testFlag(Qt::ShiftModifier) ?
                     (modifiers.testFlag(Qt::ControlModifier) ?
                          SelectionEvent::ExcludeSelection :
                          SelectionEvent::IncludeSelection
                          ) :
                     SelectionEvent::NewSelection
                     );
#endif // Q_OS_MAC //
    }

private:

    static const QEvent::Type TYPE = static_cast<QEvent::Type>(QEvent::User + 42);

    const QPainterPath m_path;
    const SelectionMode m_mode;
};

#endif // SELECTIONEVENT_H //
