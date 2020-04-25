#include "LineEdit.h"
#include <QIntValidator>

LineEdit::LineEdit(bool value)
{
    this->setObjectName("LineEdit");
    if(value)
    {
        QIntValidator *validator = new QIntValidator(0, 999);
        this->setValidator(validator);
    }
}
