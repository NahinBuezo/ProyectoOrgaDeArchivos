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
nueva_estructura::nueva_estructura(QWidget *parent,vector<campo*>* campos, int tipo) :
    QDialog(parent),
    ui(new Ui::nueva_estructura)
{
    //this->archivo_nuevo=nuevo_archivo;
    this->tipo_operacion=tipo;

    ui->setupUi(this);
    this->campos=campos;
    ui->nombre_campo->setMaxLength(30);
    QValidator *validator = new QIntValidator();

    ui->size_campo->setValidator(validator);
    ui->size_dec->setValidator(validator);
    ui->nueva_tabla_estructura->insertRow(0);
    for(int i=0; i<this->campos->size();i++){
        campo*tempo=campos->at(i);
        this->size_registro+=sizeof(tempo);
    }

    cargar_datos_estructura();


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
        campo*tempo=campos->at(campo_a_modificar);
        if(tempo->getLlave()==true){
            QMessageBox::information(this,"Alerta","No puede modificar o eliminar un campo llave");
        }else{
            cout<<"esta columna debe morir: "<<this->campo_a_modificar;
            ui->nueva_tabla_estructura->removeColumn(campo_a_modificar);
            cout<<"se elimino de la tabla, vamos a sobre cargar si se debe"<<endl;
            if(tipo_operacion==2){
                sobre_cargar_estructura_eliminando_campos();
            }
        }
    }
}


void nueva_estructura::cargar_datos_estructura(){
    QString datos;
    datos=campos->size();
    //QMessageBox::information(this,"eso",datos);
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
                bool validador_campo_llave=false;
                key=ui->campo_llave->isChecked();
                for(int i=0; i<campos->size();i++){
                    campo*auxiliar=campos->at(i);
                    if(auxiliar->getField_Type()==true&&key==true){
                        validador_campo_llave=true;
                    }
                }
                if(validador_campo_llave==true){
                    QMessageBox::information(this,"Alerta","No puede existir mas de un campo llave, esto puede producir colisiones");
                }else{
                    //campo nuevo_campo(nombre,tipo,size,size_decimales,key);
                    this->campos->push_back(new campo(nombre.toStdString().c_str(),tipo,size,size_decimales,key));
                    QMessageBox::information(this,"Exito","se procedera agregar a la tabla");
                    ui->nueva_tabla_estructura->insertColumn(ui->nueva_tabla_estructura->columnCount());
                    ui->nueva_tabla_estructura->setHorizontalHeaderItem((ui->nueva_tabla_estructura->columnCount()-1),new QTableWidgetItem(nombre));
                    ui->nueva_tabla_estructura->setCellWidget(0,(ui->nueva_tabla_estructura->columnCount()-1),new QCheckBox(0));
                    if(tipo_operacion==2){
                        sobre_cargar_estructura_agregango_campos();
                    }
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
                    if(key==true){
                        QMessageBox::information(this,"Alerta","No puede teneer mas d eun campo llave");
                    }else{
                        if((tipo==INTF&&temporal->getField_Type()==CHAR)||(tipo==DEC&&temporal->getField_Type()==CHAR)){
                            QMessageBox::information(this,"ATENCION","No puede cambiar el tipo CHAR por tipo INT o FLOAT");
                        }else{
                            temporal->setNombre(nombre.toStdString().c_str());
                            temporal->setField_Type(tipo);
                            temporal->setSize_campo(size);
                            temporal->setSize_dec(size_decimales);
                            temporal->setLLave(key);
                            // campo_a_modificar=-1;
                            if(tipo_operacion==2){
                                QMessageBox::information(this,"Atencion","se ha modificado el campo, se procedera a re-escribir el archivo");
                                sobre_cargar_estructura_modificando_campos();
                            }else{
                                QMessageBox::information(this,"Modificar","Estructura modificada exitosamente");
                            }

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
    detalles_de_campo();
}
void nueva_estructura::sobre_cargar_estructura_eliminando_campos(){
    cout<<"entramos al sobre carga eliminar"<<endl;

    if(campo_a_modificar==0){
        campos->erase(campos->begin());
    }else{
        campos->erase(campos->begin()+campo_a_modificar);
    }

    cout<<"MUY BIEN EL TAMA;O DE CAMPOS ES AHORA: "<<campos->size()<<endl;
    //ui->nueva_tabla_estructura->removeColumn(campo_a_modificar);
    vector<string>registros_viejos;
    vector<string>registros_nuevos;
    cout<<"se procede a leer el header"<<endl;
    ifstream lector(this->nombre_archivo_a_modificar.c_str());
    char* antiguo_header=new char[10000];
    lector.read(antiguo_header,10000);
    cout<<"se leyo la cabeza"<<endl;
    //se toma en cuenta los registro-borrados ya que vemas a sobre-escribir todo
    cout<<"se leerla los registros"<<endl;
    while(!lector.eof()){
        char* registro=new char[101];
        lector.read(registro,100);

        stringstream interprete;
        interprete<<registro;
        vector<string>mejorando=split(interprete.str(),',');
        mejorando.pop_back();
        stringstream antiguo;
        for(int i=0; i<mejorando.size(); i++){
            antiguo<<mejorando.at(i)<<",";
        }
        cout<<antiguo.str()<<endl;
        registros_viejos.push_back(antiguo.str());
    }
    lector.close();
    registros_viejos.pop_back();
    //ahora vamos a crear los nuevos registros con la estructura modificada

    cout<<"si ha registros crearemos nuevo"<<endl;
    if(!registros_viejos.empty()){
        cout<<"parece que si hay registros jajaja"<<endl;
        for(int i=0; i<registros_viejos.size(); i++){
            string registro_temporal=registros_viejos.at(i);
            //cout<<registro_temporal<<endl;
            vector<string>partes_del_registro=split(registro_temporal,',');
            string referencia=partes_del_registro.at(0);
            if(referencia=="*"){
                int verdadera_posicion_campo=campo_a_modificar+2;
                partes_del_registro.erase(partes_del_registro.begin()+verdadera_posicion_campo);
            }else{
                if(campo_a_modificar==0)
                    partes_del_registro.erase(partes_del_registro.begin());
                else
                    partes_del_registro.erase(partes_del_registro.begin()+campo_a_modificar);
            }

            stringstream receptor;
            for(int k=0; k<partes_del_registro.size(); k++){
                receptor<<partes_del_registro.at(k)<<",";
            }
            //cout<<receptor.str()<<endl;
            registros_nuevos.push_back(receptor.str());
        }//fin del for superior
    }//fin del else
    cout<<"ahora calcularemos de nuevo el size de registro"<<endl;
    stringstream informacion_vieja;
    informacion_vieja<<antiguo_header;
    vector<string>partes_header=split(informacion_vieja.str(),',');
    cout<<"partimos el header"<<endl;
    stringstream informacion;
    informacion<<partes_header.at(0)<<",";
    cout<<"ahora vamos a crear el headr con nuevos campos"<<endl;
    for(int i=0; i<campos->size(); i++){
        cout<<"vamos bien:  "<<i<<endl;
        campo* temp=campos->at(i);
        informacion<<temp->name<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->tipo))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size_dec))->str();
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str()<<",";
    }
    ofstream escritor(nombre_archivo_a_modificar.c_str());
    escritor<<left<<setw(10000)<<informacion.str().c_str();
    //se procede a escribir cada uno de los registros
    cout<<"ya se escribio el header, veamos lo de registros jaja"<<endl;
    escritor.flush();
    escritor.close();
    escritor.open(nombre_archivo_a_modificar.c_str(),ios::out|ios::app);
    if(!registros_nuevos.empty()){
        for(int j=0; j<registros_nuevos.size(); j++){
            string reg=registros_nuevos.at(j);

            escritor<<left<<setw(100)<<reg.c_str()<<endl;
        }
    }
    cout<<"se supone que terminamos"<<endl;
    campo_a_modificar=-1;
    escritor.flush();
    escritor.close();
    QMessageBox::information(this,"Exito","la modificacion fue exitosa");
}
void nueva_estructura::sobre_cargar_estructura_modificando_campos(){
    //recalculamos el size de refistro
    vector<string>registros_viejos;
    //vector<string>registros_nuevos;
    ifstream lector(this->nombre_archivo_a_modificar.c_str());
    char* antiguo_header=new char[10000];
    lector.read(antiguo_header,10000);
    //se toma en cuenta los registro-borrados ya que vemas a sobre-escribir todo

    while(!lector.eof()){
        char* registro=new char[101];
        lector.read(registro,100);

        stringstream interprete;
        interprete<<registro;
        vector<string>mejorando=split(interprete.str(),',');
        mejorando.pop_back();
        stringstream antiguo;
        for(int i=0; i<mejorando.size(); i++){
            antiguo<<mejorando.at(i)<<",";
        }
        registros_viejos.push_back(antiguo.str());
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
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str()<<",";
    }
    strcpy(header,informacion.str().c_str());
    ofstream escritor(nombre_archivo_a_modificar.c_str());
    escritor<<left<<setw(10000)<<header;
    escritor.flush();
    escritor.close();
    //ahora trabajamos con los registros
    vector<string>nuevos_registros;
    for(int i=0; i<registros_viejos.size(); i++){
        string registro_temporal=registros_viejos.at(i);
        vector<string>partes_del_registro=split(registro_temporal,',');
        string referencia=partes_del_registro.at(0);

        if(referencia=="*"){
            int verdadera_posicion_campo=campo_a_modificar+2;
            string parte_original_del_registro=partes_del_registro.at(verdadera_posicion_campo);
            campo*temp=campos->at(campo_a_modificar);
            char* datos_nuevos=new char[temp->size];
            strncpy(datos_nuevos,parte_original_del_registro.c_str(),temp->size);
            stringstream nuevo_regis;
            for(int j=0; j<partes_del_registro.size();j++){
                string partes=partes_del_registro.at(j);
                if(j==verdadera_posicion_campo){
                    nuevo_regis<<datos_nuevos<<",";
                }else{
                    nuevo_regis<<partes<<",";
                }
            }
            nuevos_registros.push_back(nuevo_regis.str());
        }else{
            string parte_original_del_registro=partes_del_registro.at(campo_a_modificar);
            campo*temp=campos->at(campo_a_modificar);
            char* datos_nuevos=new char[temp->getSize_campo()];
            strncpy(datos_nuevos,parte_original_del_registro.c_str(),temp->getSize_campo());
            stringstream nuevo_regis;
            for(int j=0; j<partes_del_registro.size();j++){
                string partes=partes_del_registro.at(j);
                if(j==campo_a_modificar){
                    nuevo_regis<<datos_nuevos<<",";
                }else{
                    nuevo_regis<<partes<<",";
                }
            }
            nuevos_registros.push_back(nuevo_regis.str());
        }
    }//fin del for superior
    //ahora escribiremos registros
    escritor.open(this->nombre_archivo_a_modificar.c_str(),ios::out|ios::app);
    for(int i=0; i<nuevos_registros.size(); i++){
        escritor<<left<<setw(100)<<nuevos_registros.at(i).c_str()<<endl;
    }
    escritor.flush();
    escritor.close();
    QMessageBox::information(this,"Operacion","El archivo se actualizo");
}

void nueva_estructura::sobre_cargar_estructura_agregango_campos(){
    //recalculamos el size de refistro

    vector<string>registros_viejos;
    //vector<string>registros_nuevos;
    ifstream lector(this->nombre_archivo_a_modificar.c_str());
    char* antiguo_header=new char[10000];
    lector.read(antiguo_header,10000);
    cout<<"se leyo el header en agregando..."<<endl;
    //se toma en cuenta los registro-borrados ya que vemas a sobre-escribir todo

    while(!lector.eof()){
        char* registro=new char[101];
        lector.read(registro,100);

        stringstream interprete;
        interprete<<registro;
        vector<string>mejorando=split(interprete.str(),',');
        mejorando.pop_back();
        stringstream antiguo;
        for(int i=0; i<mejorando.size(); i++){
            antiguo<<mejorando.at(i)<<",";
        }
        cout<<antiguo.str()<<endl;
        registros_viejos.push_back(antiguo.str());
    }
    lector.close();
    //creamos un nuevo header

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
        informacion<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str()<<",";
    }

    ofstream escritor(nombre_archivo_a_modificar.c_str());
    escritor<<left<<setw(10000)<<informacion.str().c_str();
    escritor.flush();
    escritor.close();
    //ahora vamos hacer magia con los registros
    cout<<"vamos a escribir registros... si lo hay jajaja"<<endl;
    registros_viejos.pop_back();
    if(!registros_viejos.empty()){
        escritor.open(nombre_archivo_a_modificar.c_str(),ios::out|ios::app);
        cout<<"Parece que si hat registros"<<endl;
        for(int i=0; i<registros_viejos.size(); i++){
            escritor<<left<<setw(100)<<registros_viejos.at(i).c_str()<<endl;
        }//fin del for superior
        escritor.flush();
        escritor.close();
        QMessageBox::information(this,"Operaciones","Se actualizo el archivo");
    }


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
void nueva_estructura::detalles_de_campo(){
    campo* campo_tempo=campos->at(this->campo_a_modificar);
    QString size_camp=QString::number(campo_tempo->getSize_campo());
    QString size_decimal=QString::number(campo_tempo->getSize_dec());
    int dato;
    if(campo_tempo->getField_Type()==CHAR)
        dato=0;
    if(campo_tempo->getField_Type()==INTF)
        dato=1;
    if(campo_tempo->getField_Type()==DEC)
        dato=2;
    ui->nombre_campo->setText(campo_tempo->getNombre());
    if(campo_tempo->getLlave()==true)
        ui->es_llave_este_campo->setText("Este campo es llave");
    else
        ui->es_llave_este_campo->setText("Este campo no es llave");
    //ui->size_campo->setText(size_camp);
    ui->size_dec->setText(size_decimal);
    ui->campo_llave->setCheckable(campo_tempo->getLlave());
}
