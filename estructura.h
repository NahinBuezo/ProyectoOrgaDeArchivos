#ifndef NUEVA_ESTRUCTURA_H
#define NUEVA_ESTRUCTURA_H
#include <vector>
#include "campo.h"
#include <QDialog>
#include "btree.h"
using namespace std;
namespace Ui {
class nueva_estructura;
}
class nueva_estructura : public QDialog
{
    Q_OBJECT
public:
    bool archivo_nuevo;
    int tipo_operacion=-1;
    string nombre_archivo_a_modificar;
public:
    explicit nueva_estructura(QWidget *parent = 0,vector<campo*>* = 0,bool = 0, int=0);
    ~nueva_estructura();
    void cargar_datos_estructura();
    void setOperacion(int);
    void agregar_campo();
    void sobre_cargar_estructura_modificando_campos();
    void sobre_cargar_estructura_eliminando_campos();
    void sobre_cargar_estructura_agregango_campos();
    vector<string>split(string,char);
private slots:
    void on_crear_nuevo_campo_clicked();

    void on_eliminar_campo_clicked();

    void on_editar_campo_clicked();


    void on_nueva_tabla_estructura_cellClicked(int row, int column);

private:
    Ui::nueva_estructura *ui;
    vector<campo*>*campos;
    vector<campo*>campos_modificados;
    vector<int>posicion_campos_modificados;
    bool estructura_modificada;
    int campo_a_modificar;
    int size_registro;

};

#endif // NUEVA_ESTRUCTURA_H
