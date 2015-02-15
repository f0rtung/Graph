#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QIODevice>
#include <algorithm>
#include <QDataStream>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}
Widget::~Widget()
{
    delete ui;
}
void Widget::on_open_file_clicked()
{
    openfile();             // диалог выбора файла
    if(filename.isEmpty()){ // проверяем, был ли выбран файл
        errorfunc("You have not selected a file. Please select a file."); // если нет - выводим сообщение
        return;
    }
    coord_x.clear(); // очищаем списки координат для ввода новых
    coord_y.clear();
    bool wasread = false; // переменная для хранения успешности открытия файла
    if(filename.at(filename.lastIndexOf(".") + 1) == 'b'){ // если расширение файла начинается на b - считаем, что это bin-файл (без расширения файлы не читаем)
        wasread = readfrombin(); //читаем из бинарного файла
    }
    else
    {
        wasread = readfromtxt(); //читаем из txt файла
    }
    if(wasread) // если чтение было успешным
    {
        painted = true; // устанавливаем флаг, что координаты отрисованы XOY
        paintGraph(coord_x,coord_y); // рисуем график
    }
    else{ //если чтение было неуспешным
        ui->Changeaxes->setEnabled(false); // делаем неактивными кнопки смены координат
        ui->Save->setEnabled(false); // и сохранения в файл
    }
}

void Widget::paintGraph(const QList<double>& vx,const QList<double>& vy){
        ui->Changeaxes->setEnabled(true); // если ввод координат был успешен - делаем кнопки изменения координат
        ui->Save->setEnabled(true);       // и сохранения графика доступными
        double min_x = *(std::min_element(vx.begin(),vx.end())); // находим минимальные и максимальные элементы
        double max_x = *(std::max_element(vx.begin(),vx.end())); // для установки диапазона графика. было бы лучше использовать std::minmax, чтобы 2 раза не запускать поиск,
        double min_y = *(std::min_element(vy.begin(),vy.end())); // но использовать класс pair в Qt у меня так и не получилось
        double max_y = *(std::max_element(vy.begin(),vy.end()));
        ui->widget->addGraph();
        ui->widget->graph(0)->setData(vx.toVector(), vy.toVector()); // передаем координаты точек для построения
        ui->widget->graph(0)->setPen(QColor(50, 50, 50, 255)); // цвет
        ui->widget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar, 6)); // тип маркера на графике
        ui->widget->xAxis->setLabel("X"); // названия осей
        ui->widget->yAxis->setLabel("Y");
        ui->widget->xAxis->setRange(min_x,max_x); // устанавливаем диапазоны отображения по x и y
        ui->widget->yAxis->setRange(min_y,max_y);
        ui->widget->replot();
}

void Widget::on_Changeaxes_clicked()
{
    if(painted){ // если true, значит нарисовано XOY, меняем местами координаты
        painted = false;
        paintGraph(coord_y,coord_x);
    }
    else {  // если флаг в false, значит нарисовано YOX, меняем местами координаты
        painted = true;
        paintGraph(coord_x,coord_y);
    }
}

void Widget::openfile(){
    filename = QFileDialog::getOpenFileName(
                this,                                    // родитель окна
                "Open File",                             // заголовок окна
                ".",                                     // директория для открытия (по умолчанию текущая директория)
                "Text File (*.txt);; Bin File (*.bin)"   // фильтр выбора файлов
                );
}

void Widget::errorfunc(const QString &str){
    QMessageBox::critical(this,"Error",str); // вывод бокса с текстом ошибки
}

void Widget::on_Save_clicked()
{
    filename = QFileDialog::getSaveFileName(  // открываем файл для сохранения
                this,
                "Save File",
                ".",
                "Bin File (*.bin)"); // только в бинарный файл
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    auto it_x = coord_x.begin();
    auto it_y = coord_y.begin();
    while(it_x != coord_x.end()){  // сохраняем данные в файле, считаем, что кол-во координат х = у
        out << *it_x;
        out << *it_y;
        ++it_x;
        ++it_y;
    }
}

bool Widget::readfrombin(){
    QFile file(filename);
    file.open(QIODevice::ReadOnly);  // предполагается, что бинарный файл не может содержать ошибок,в отличие от txt файла
    QDataStream in(&file); // в противном случае можно предусмотреть,например, проверку на равенство двух контейнеров, т.к. кол-во координат x д.б. = y
    double x, y;
    while(!in.atEnd()){ // считываем и сохраняем координаты
        in >> x >> y;
        coord_x << x;
        coord_y << y;
    }
    return true;
}

bool Widget::readfromtxt(){
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly |QIODevice::Text)){ // проверяем успешность открытия файла
        while(!file.atEnd()){
            QString str = file.readLine();      // считываем строку
            QStringList lst = str.split(";");   // создаем список строк, которые получаются из исходной при помощи разделителя
            if(lst.count() != 2){ // проверяем, содержится ли в списке 2 координаты. если нет, то выводим ошибку
                errorfunc("The file "+ filename+ " contains errors. Please open another file.");
                return false;
            }
            str = lst.at(0);
            int pos = str.indexOf(",");         // ищем в подстроке заяптую, если она есть - меняем на точку
            if(pos != -1) str.replace(pos,1,".");
            bool test;
            double coord = str.toDouble(&test); // проверяем успешность приведения, если успешно, то
            if (test) coord_x << coord;         // добавляем координаты в соответствующий вектор
            else{
                errorfunc("The file " + filename + " contains errors. Please open another file."); // иначе выводим сообщение об ошибке
                return false;
            }
            str = lst.at(1); // тоже самое для координат Y
            pos = str.indexOf(",");
            if(pos != -1) str.replace(pos,1,".");
            coord = str.toDouble(&test);
            if (test) coord_y << coord;
            else{
                errorfunc("The file "+ filename+ " contains errors. Please open another file.");
                return false;
            }
        }
        return true; // возвращаем признак успешного ввода
    }
    else{ // если открыть файл не удалось - выводим сообщение
        errorfunc("Can't open a file! Try again!");
        return false;
    }
}
