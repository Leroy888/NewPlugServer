#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QWidget>

namespace Ui {
class ComboBox;
}

class ComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit ComboBox(QWidget *parent = 0);
    ~ComboBox();

    void setEditable(const bool value);
    void setCurrentText(QString text);
    QString currentText();
    void clear();
    void addItems(const QStringList &items);
    int currentIndex() const;

private slots:
    void on_cmbBox_currentTextChanged(const QString &arg1);


private:
    Ui::ComboBox *ui;

    QString m_text;
};

#endif // COMBOBOX_H
