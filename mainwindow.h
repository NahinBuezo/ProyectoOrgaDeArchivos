#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "campo.h"
#include "nueva_estructura.h"
#include "estructura.h"
#include "index.h"
#include "btree.h"
#include <vector>
#include <QMainWindow>
#include <QString>
//enum FieldType {CHAR, INTF, DEC};
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
     bool archivo_abierto;
     bool estructura_modificada;
     int size_registro;
     vector<int>campos_eliminados;
     //vector<int>campos_modificados;
     vector<string>split(string,char);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void abrir_creacion_estructura();
    void cargar_indice_desde_archivo();
    void crear_archivo_index();

private slots:


    void on_actionNuevo_triggered();

    void on_actionAbrir_triggered();

    void on_actionCrear_campos_triggered();

    void on_actionListar_campos_triggered();

    void on_actionModificar_campos_triggered();

    void on_actionBorrar_campos_triggered();

    void on_actionSalvar_triggered();

    void on_actionExportar_Excel_triggered();

    void on_actionSalir_triggered();

    void on_actionIntroducir_registros_triggered();

private:
    Ui::MainWindow *ui;
    vector <campo*>* campos;
    vector<Index*>*indices;
    QString nombre_archivo_abierto;
    Tree arbol_b;
};
   #endif // MAINWINDOW_H
