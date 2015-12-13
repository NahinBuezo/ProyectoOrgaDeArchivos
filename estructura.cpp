#include "nueva_estructura.h"
#include "ui_nueva_estructura.h"
#include "campo.h"
#include "index.h"
#include "estructura.h"
#include <QMessageBox>
#include <QString>
#include <vector>
#include <fstream>
#include <QInputDialog>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
using namespace std;
//vector<string>split(string,char);
//enum FieldType {CHAR, INTF, DEC};
nueva_estructura::nueva_estructura(QWidget *parent,vector<campo*>* campos,bool nuevo_archivo, int tipo) :
    QDialog(parent),
    ui(new Ui::nueva_estructura)
{
    this->archivo_nuevo=nuevo_archivo;
    this->tipo_operacion=tipo;
    ui->setupUi(this);
    this->campos=campos;
    ui->nombre_campo->setMaxLength(30);
    QValidator *validator = new QIntValidator();
    if(this->archivo_nuevo==true){
        ui->size_campo->setValidator(validator);
        ui->size_dec->setValidator(validator);
        ui->nueva_tabla_estructura->insertRow(0);
    }else{
        cargar_datos_estructura();
    }

}

nueva_estructura::~nueva_estructura()
{
    delete ui;
}

void nueva_estructura::on_crear_nuevo_campo_clicked()
{
    agregar_campo();
}

void nueva_estructura::on_eliminar_campo_clicked()
{
    if(campo_a_modificar>=0&&campo_a_modificar<ui->nueva_tabla_estructura->columnCount()){
        if(tipo_operacion==1){
            ui->nueva_tabla_estructura->removeColumn(campo_a_modificar);
            campos->erase(campos->begin()+campo_a_modificar);
            campo_a_modificar=-1;
        }else{
            sobre_cargar_estructura_eliminando_campos();
            campo_a_modificar=-1;
        }//fin else
    }
}


void nueva_estructura::cargar_datos_estructura(){
    QString datos;
    datos=campos->size();
    QMessageBox::information(this,"eso",datos);
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        ui->nueva_tabla_estructura->insertColumn(ui->nueva_tabla_estructura->columnCount());
        ui->nueva_tabla_estructura->setHorizontalHeaderItem((ui->nueva_tabla_estructura->columnCount()-1),new QTableWidgetItem(temp->name));
        ui->nueva_tabla_estructura->setCellWidget(0,(ui->nueva_tabla_estructura->columnCount()-1),new QCheckBox(0));

    }
}
void nueva_estructura::setOperacion(int tipo){
    this->tipo_operacion=tipo;
}


void nueva_estructura::agregar_campo(){
    if(ui->nombre_campo->text().isEmpty()||ui->size_campo->text().isEmpty()||ui->size_dec->text().isEmpty()){
        QMessageBox::information(this,"Datos","Debe llenar todos los campos");
    }else{
        if(ui->nombre_campo->text().length()>30){
            QMessageBox::information(this,"Datos","El nombre del campo no debe exceder los 30 caracteres");
        }else{
            int size,size_decimales;
            bool key;
            int opcion_tipo_dato;
            FieldType tipo;
            QString nombre=ui->nombre_campo->text();
            try{
                size=ui->size_campo->text().toInt();
                size_decimales=ui->size_dec->text().toInt();
                opcion_tipo_dato=ui->tipo_dato_campo->currentIndex();
                if(opcion_tipo_dato==0){
                    tipo=CHAR;
                }
                if(opcion_tipo_dato==1){
                    tipo=INTF;
                }
                if(opcion_tipo_dato==2){
                    tipo=DEC;
                }
                key=ui->campo_llave->isChecked();
                //campo nuevo_campo(nombre,tipo,size,size_decimales,key);
                this->campos->push_back(new campo(nombre.toStdString().c_str(),tipo,size,size_decimales,key));
                QMessageBox::information(this,"Exito","se procedera agregar a la tabla");
                ui->nueva_tabla_estructura->insertColumn(ui->nueva_tabla_estructura->columnCount());
                ui->nueva_tabla_estructura->setHorizontalHeaderItem((ui->nueva_tabla_estructura->columnCount()-1),new QTableWidgetItem(nombre));
                if(tipo_operacion==1){
                    ui->nueva_tabla_estructura->setCellWidget(0,(ui->nueva_tabla_estructura->columnCount()-1),new QCheckBox(0));
                }
            }catch(...){
                QMessageBox::information(this,"Datos","Verifique que los campos tengan el tipo de valor correcto");
            }
        }
    }
}

void nueva_estructura::on_editar_campo_clicked()
{

    if(this->campo_a_modificar==-1){
        QMessageBox::information(this,"Atencion","Debe seleccionar un campo y modificar sus valores\n"
                                                 "lo que puede implicar perdida de datos");
    }else{
        if(ui->nombre_campo->text().isEmpty()||ui->size_campo->text().isEmpty()||ui->size_dec->text().isEmpty()){
            QMessageBox::information(this,"Atencion","No puede modificar este campo si no se ha completado los datos");
        }else{
            campo* temporal=campos->at(this->campo_a_modificar);
            if(temporal->getLlave()){
                QMessageBox::information(this,"Alerta","Este es un campo llave el cual no puede modificarse");
            }else{
                // QMessageBox::StandardButton reply;
                //reply = QMessageBox::question(this, "Modificar", "Esta seguro que desea modifica? esto puede causar perdida d edatos",
                //                            QMessageBox::Yes|QMessageBox::No);
                int size,size_decimales;
                bool key;
                int opcion_tipo_dato;
                FieldType tipo;
                QString nombre=ui->nombre_campo->text();
                try{
                    size=ui->size_campo->text().toInt();
                    size_decimales=ui->size_dec->text().toInt();
                    opcion_tipo_dato=ui->tipo_dato_campo->currentIndex();
                    if(opcion_tipo_dato==0){
                        tipo=CHAR;
                    }
                    if(opcion_tipo_dato==1){
                        tipo=INTF;
                    }
                    if(opcion_tipo_dato==2){
                        tipo=DEC;
                    }
                    key=ui->campo_llave->isChecked();
                    //campo nuevo_campo(nombre.toStdString().c_str(),tipo,size,size_decimales,key);
                    if((tipo==INTF&&temporal->getField_Type()==CHAR)||(tipo==DEC&&temporal->getField_Type()==CHAR)){
                        QMessageBox::information(this,"ATENCION","No puede cambiar el tipo CHAR por tipo INT o FLOAT");
                    }else{
                        temporal->setNombre(nombre.toStdString().c_str());
                        temporal->setField_Type(tipo);
                        temporal->setSize_campo(size);
                        temporal->setSize_dec(size_decimales);
                        temporal->setLLave(key);
                        campo_a_modificar=-1;
                        if(tipo_operacion==2){
                            QMessageBox::information(this,"Atencion","se ha modificado el campo, se procedera a re-escribir el archivo");
                            sobre_cargar_estructura_modificando_campos();
                        }else{
                            QMessageBox::information(this,"Modificar","Estructura modificada exitosamente");
                        }

                    }
                }catch(...){
                    QMessageBox::information(this,"Datos","Verifique que los campos tengan el tipo de valor correcto");
                }
            }//fin del tercer else
        }//fin del segudno else
    }//fin del primer else

}


void nueva_estructura::on_nueva_tabla_estructura_cellClicked(int row, int column)
{
    this->campo_a_modificar=column;
}
void nueva_estructura::sobre_cargar_estructura_eliminando_campos(){
    campos->erase(campos->begin()+campo_a_modificar);
    ui->nueva_tabla_estructura->removeColumn(campo_a_modificar);
    //se debe re-calcular el size del registro
    this->size_registro=0;
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        this->size_registro+=sizeof(temp);
    }
    vector<string>registros_viejos;
    vector<string>registros_nuevos;
    ifstream lector(this->nombre_archivo_a_modificar.c_str());
    char antiguo_header[10000];
    lector.read(antiguo_header,10000);

    //se toma en cuenta los registro-borrados ya que vemas a sobre-escribir todo

    while(!lector.eof()){
        char* registro=new char[size_registro];
        lector.read(registro,size_registro);

        stringstream interprete;
        interprete<<registro;
        registros_viejos.push_back(interprete.str());
    }
    lector.close();
    //ahora vamos a crear los nuevos registros con la estructura modificada
    //ofstream escritor(nombre_archivo_a_modificar.toStdString().c_str());
    for(int i=0; i<registros_viejos.size(); i++){
        string registro_temporal=registros_viejos.at(i);
        vector<string>partes_del_registro=split(registro_temporal,',');
        string referencia=partes_del_registro.at(0);
        if(referencia=="*"){
            int verdadera_posicion_campo=campo_a_modificar+2;
            partes_del_registro.erase(partes_del_registro.begin()+verdadera_posicion_campo);
        }else{
            partes_del_registro.erase(partes_del_registro.begin()+campo_a_modificar);
        }

        stringstream receptor;
        for(int k=0; k<partes_del_registro.size(); k++){
            receptor<<partes_del_registro.at(k)<<",";
        }
        registros_nuevos.push_back(receptor.str());
    }//fin del for superior
    //ahora procedemos a escribir los nuevos archivos
    //antes debemos actualizar el header
    char header[10000];
    stringstream informacion_vieja;
    informacion_vieja<<antiguo_header;
    vector<string>partes_header=split(informacion_vieja.str(),',');
    stringstream informacion;
    informacion<<partes_header.at(0)<<",";
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        informacion<<temp->name<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->tipo))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size_dec))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str();
    }
    strcpy(header,informacion.str().c_str());
    ofstream escritor(nombre_archivo_a_modificar.c_str());
    escritor<<left<<setw(10000)<<header;
    //se procede a escribir cada uno de los registros

    for(int j=0; j<registros_nuevos.size(); j++){
        string reg=registros_nuevos.at(j);
        escritor<<endl;
        escritor<<left<<setw(size_registro)<<reg.c_str();
    }
    escritor.flush();
    escritor.close();
    QMessageBox::information(this,"Exito","la modificacion fue exitosa");
}
void nueva_estructura::sobre_cargar_estructura_modificando_campos(){
    //recalculamos el size de refistro
    size_registro=0;
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        size_registro+=sizeof(temp);
    }
    vector<string>registros_viejos;
    //vector<string>registros_nuevos;
    ifstream lector(this->nombre_archivo_a_modificar.c_str());
    char* antiguo_header=new char[10000];
    lector.read(antiguo_header,10000);
    //se toma en cuenta los registro-borrados ya que vemas a sobre-escribir todo

    while(!lector.eof()){
        char* registro=new char[size_registro];
        lector.read(registro,size_registro);

        stringstream interprete;
        interprete<<registro;
        registros_viejos.push_back(interprete.str());
    }
    lector.close();
    //creamos un nuevo header
    char header[10000];
    stringstream informacion_vieja;
    informacion_vieja<<antiguo_header;
    vector<string>partes_header=split(informacion_vieja.str(),',');
    stringstream informacion;
    informacion<<partes_header.at(0)<<",";
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        informacion<<temp->name<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->tipo))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size_dec))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str();
    }
    strcpy(header,informacion.str().c_str());
    ofstream escritor(nombre_archivo_a_modificar.c_str());
    escritor<<left<<setw(10000)<<header;

    //ahora trabajamos con los registros

    for(int i=0; i<registros_viejos.size(); i++){
        string registro_temporal=registros_viejos.at(i);
        vector<string>partes_del_registro=split(registro_temporal,',');
        string referencia=partes_del_registro.at(0);

        if(referencia=="*"){
            int verdadera_posicion_campo=campo_a_modificar+2;
            string parte_original_del_registro=partes_del_registro.at(verdadera_posicion_campo);
            campo*temp=campos->at(campo_a_modificar);
            char* datos_nuevos=new char[temp->size];
            strncpy(datos_nuevos,parte_original_del_registro.c_str(),sizeof(datos_nuevos));
            stringstream nuevo_regis;
            for(int j=0; j<partes_del_registro.size();j++){
                string partes=partes_del_registro.at(j);
                if(j==verdadera_posicion_campo){
                    nuevo_regis<<datos_nuevos<<",";
                }else{
                    nuevo_regis<<partes<<",";
                }
            }
            escritor<<endl;
            escritor<<left<<setw(size_registro)<<nuevo_regis.str().c_str();
        }else{
            string parte_original_del_registro=partes_del_registro.at(campo_a_modificar);
            campo*temp=campos->at(campo_a_modificar);
            char* datos_nuevos=new char[temp->size];
            strncpy(datos_nuevos,parte_original_del_registro.c_str(),sizeof(datos_nuevos));
            stringstream nuevo_regis;
            for(int j=0; j<partes_del_registro.size();j++){
                string partes=partes_del_registro.at(j);
                if(j==campo_a_modificar){
                    nuevo_regis<<datos_nuevos<<",";
                }else{
                    nuevo_regis<<partes<<",";
                }
            }
            escritor<<endl;
            escritor<<left<<setw(size_registro)<<nuevo_regis.str().c_str();
        }
    }//fin del for superior
    escritor.flush();
    escritor.close();
}

void nueva_estructura::sobre_cargar_estructura_agregango_campos(){
    //recalculamos el size de refistro
    size_registro=0;
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        size_registro+=sizeof(temp);
    }
    vector<string>registros_viejos;
    //vector<string>registros_nuevos;
    ifstream lector(this->nombre_archivo_a_modificar.c_str());
    char antiguo_header[10000];
    lector.read(antiguo_header,10000);
    //se toma en cuenta los registro-borrados ya que vemas a sobre-escribir todo

    while(!lector.eof()){
        char* registro=new char[size_registro];
        lector.read(registro,size_registro);

        stringstream interprete;
        interprete<<registro;
        registros_viejos.push_back(interprete.str());
    }
    lector.close();
    //creamos un nuevo header
    char header[10000];
    stringstream informacion_vieja;
    informacion_vieja<<antiguo_header;
    vector<string>partes_header=split(informacion_vieja.str(),',');
    stringstream informacion;
    informacion<<partes_header.at(0)<<",";
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        informacion<<temp->name<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->tipo))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size_dec))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str();
    }
    strcpy(header,informacion.str().c_str());
    ofstream escritor(nombre_archivo_a_modificar.c_str());
    escritor<<left<<setw(10000)<<header;

    //ahora vamos hacer magia con los registros
    for(int i=0; i<registros_viejos.size(); i++){
        escritor<<endl;
        escritor<<left<<setw(size_registro)<<registros_viejos.at(i)<<",";
    }//fin del for superior
    escritor.flush();
    escritor.close();
}

vector<string> nueva_estructura::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}
