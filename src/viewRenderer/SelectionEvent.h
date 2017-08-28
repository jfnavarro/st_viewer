#ifndef SELECTIONEVENT_H
#define SELECTIONEVENT_H

#include <QEvent>
#include <QPainterPath>

// Selection event used to propagate selection data to view items.
class SelectionEvent : public QEvent
{

public:
    enum SelectionMode { NewSelection, IncludeSelection, ExcludeSelection };

    SelectionEvent()
        : QEvent(TYPE)
        , m_path()
        , m_mode(NewSelection)
    {
    }

    SelectionEvent(const QPainterPath &path, const SelectionMode mode = NewSelection)
        : QEvent(TYPE)
        , m_path(path)
        , m_mode(mode)
    {
    }

    QPainterPath path() const { return m_path; }
    SelectionMode mode() const { return m_mode; }

    static SelectionMode modeFromKeyboardModifiers(Qt::KeyboardModifiers modifiers)
    {
        return (modifiers.testFlag(Qt::ShiftModifier)
                    ? (modifiers.testFlag(Qt::ControlModifier) ? SelectionEvent::ExcludeSelection
                                                               : SelectionEvent::IncludeSelection)
                    : SelectionEvent::NewSelection);
    }

private:
    static const QEvent::Type TYPE = static_cast<QEvent::Type>(QEvent::User + 42);

    const QPainterPath m_path;
    const SelectionMode m_mode;
};

#endif // SELECTIONEVENT_H //
