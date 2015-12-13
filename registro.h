#ifndef REGISTRO_H
#define REGISTRO_H
#include "campo.h"
#include "index.h"
#include "estructura.h"
#include "btree.h"
#include <QWidget>
#include <vector>
#include <string>
#include <QDialog>

namespace Ui {
class registro;
}

class registro : public QDialog
{
    Q_OBJECT

public:
    explicit registro(QWidget *parent = 0,vector<campo*>* = 0,Tree* = 0 , int = 0,string="");
    ~registro();
    void agregar_registro_al_Archivo();
    void modificar_registro_de_archivo();
    void eliminar_registro();
    vector<string>split(string, char);
    void validar_campos_en_tabla();
    void cargar_avail_list();
private slots:
    void on_buscar_llave_clicked();


    void on_agregar_nuevo_registro_clicked();

    void on_borrar_registro_clicked();

    void on_modificacion_registro_clicked();

private:
    Ui::registro *ui;
    Tree* my_tree;
    Index indice_temporal;
    string nombre_del_archivo;
    int tipo_operacion;
    vector<campo*>* campos;
    int size_Registro=0;
    vector<int>avail_list;
};

#endif // REGISTRO_H
