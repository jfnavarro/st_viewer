#include "JSONError.h"

#include <QApplication>

static const char *LOC_CONTEXT = "JSONError";

JSONError::JSONError(QObject *parent)
    : Error(parent)
{
    init(QJsonParseError::NoError);
}

JSONError::JSONError(QJsonParseError::ParseError error, QObject *parent)
    : Error(parent)
{
    init(error);
}

JSONError::~JSONError()
{
}

void JSONError::init(QJsonParseError::ParseError error)
{
    QString name;
    QString description;

    switch (error) {
    case QJsonParseError::NoError:
        name = QApplication::translate(LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(LOC_CONTEXT, "NoError:Description");
        break;
    case QJsonParseError::UnterminatedObject:
        name = QApplication::translate(LOC_CONTEXT, "UnterminatedObject:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnterminatedObject:Description");
        break;
    case QJsonParseError::MissingNameSeparator:
        name = QApplication::translate(LOC_CONTEXT, "MissingNameSeparator:Name");
        description = QApplication::translate(LOC_CONTEXT, "MissingNameSeparator:Description");
        break;
    case QJsonParseError::UnterminatedArray:
        name = QApplication::translate(LOC_CONTEXT, "UnterminatedArray:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnterminatedArray:Description");
        break;
    case QJsonParseError::MissingValueSeparator:
        name = QApplication::translate(LOC_CONTEXT, "MissingValueSeparator:Name");
        description = QApplication::translate(LOC_CONTEXT, "MissingValueSeparator:Description");
        break;
    case QJsonParseError::IllegalValue:
        name = QApplication::translate(LOC_CONTEXT, "IllegalValue:Name");
        description = QApplication::translate(LOC_CONTEXT, "IllegalValue:Description");
        break;
    case QJsonParseError::TerminationByNumber:
        name = QApplication::translate(LOC_CONTEXT, "TerminationByNumber:Name");
        description = QApplication::translate(LOC_CONTEXT, "TerminationByNumber:Description");
        break;
    case QJsonParseError::IllegalNumber:
        name = QApplication::translate(LOC_CONTEXT, "IllegalNumber:Name");
        description = QApplication::translate(LOC_CONTEXT, "IllegalNumber:Description");
        break;
    case QJsonParseError::IllegalEscapeSequence:
        name = QApplication::translate(LOC_CONTEXT, "IllegalEscapeSequence:Name");
        description = QApplication::translate(LOC_CONTEXT, "IllegalEscapeSequence:Description");
        break;
    case QJsonParseError::IllegalUTF8String:
        name = QApplication::translate(LOC_CONTEXT, "IllegalUTF8String:Name");
        description = QApplication::translate(LOC_CONTEXT, "IllegalUTF8String:Description");
        break;
    case QJsonParseError::UnterminatedString:
        name = QApplication::translate(LOC_CONTEXT, "UnterminatedString:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnterminatedString:Description");
        break;
    case QJsonParseError::MissingObject:
        name = QApplication::translate(LOC_CONTEXT, "MissingObject:Name");
        description = QApplication::translate(LOC_CONTEXT, "MissingObject:Description");
        break;
    case QJsonParseError::DeepNesting:
        name = QApplication::translate(LOC_CONTEXT, "DeepNesting:Name");
        description = QApplication::translate(LOC_CONTEXT, "DeepNesting:Description");
        break;
    case QJsonParseError::DocumentTooLarge:
        name = QApplication::translate(LOC_CONTEXT, "DocumentTooLarge:Name");
        description = QApplication::translate(LOC_CONTEXT, "DocumentTooLarge:Description");
        break;
    default:
        name = QApplication::translate(LOC_CONTEXT, "UnknownError:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnknownError:Description").arg(error);
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}
