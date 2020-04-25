#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QObject>
#include <QLineEdit>

class LineEdit : public QLineEdit
{
public:
    LineEdit(bool value = true);
};

#endif // LINEEDIT_H
