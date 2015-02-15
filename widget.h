#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QList>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
private:
    QString filename; // имя открываемого файла
    QList<double> coord_x; // список координат x
    QList<double> coord_y; // список координат y
    bool painted; // флаг, отвечающий за то, в каких осях рисовать график

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_open_file_clicked();

    void on_Changeaxes_clicked();

    void on_Save_clicked();

private:
    Ui::Widget *ui;
    void paintGraph(const QList<double>& vx,const QList<double>& vy); // функция рисования графика
    void openfile(); // открытие файла
    void errorfunc(const QString& str); // вывод сообщения об ошибке
    bool readfrombin(); // чтение bin файла
    bool readfromtxt(); // чтение txt файла
};

#endif // WIDGET_H
