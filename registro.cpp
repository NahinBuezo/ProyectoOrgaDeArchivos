#include "registro.h"
#include "ui_registro.h"
#include <QMessageBox>
#include <string>
#include <sstream>
#include <vector>
#include "index.h"
#include "btree.h"
#include <fstream>
#include "estructura.h"
#include <iomanip>
#include <cstdlib>
#include <iostream>
using namespace std;
registro::registro(QWidget *parent ,vector<campo*>*campos ,Tree*my_arbol ,int operacion, string nombre_del_archivo) :
    QDialog(parent),
    ui(new Ui::registro)
{
    ui->setupUi(this);
    QMessageBox::information(this,"entramos","entramos");
    this->nombre_del_archivo=nombre_del_archivo;
    this->my_tree=my_arbol;
    this->campos=campos;
    this->tipo_operacion=operacion;
    for(int i=0; i<this->campos->size();i++){
        campo* camp=this->campos->at(i);
        size_Registro+=sizeof(camp);
    }
    //validar_campos_en_tabla();
}

registro::~registro()
{
    delete ui;
}

vector<string>registro::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}
void registro::agregar_registro_al_Archivo(){
    bool datos_completos=true;
    try{
        cout<<"Entro al agregar"<<endl;
        for(int i=0; i<ui->tabla_estructura->columnCount(); i++){
           QString dato=ui->tabla_estructura->item(0,i);
            if(dato.isEmpty()||dato.isNull())
                datos_completos=false;
        }
        cout<<"pasamos jajja"<<endl;

        if(datos_completos==false){
            QMessageBox::information(this,"Datos","Los datos estan incompletos");
        }else{
            //creamos el indice
            cout<<"vamos a crear indice"<<endl;
            int campo_llave;
            for(int i=0; i<campos->size();i++){
                campo* camp=campos->at(i);
                if(camp->getLlave()==true)
                    campo_llave=i;
            }
            stringstream regis;
            cout<<"vamos a formar el registro"<<endl;
            bool registro_comoleto=true;
            stringstream detalles;
            for(int i=0; i<ui->tabla_estructura->columnCount(); i++){
                try{
                    QString dato=ui->tabla_estructura->item(0,i);
                    campo* validador=campos->at(i);
                    if(validador->getField_Type()==CHAR){
                        regis<<dato.toStdString()<<",";
                    }
                    if(validador->getField_Type()==INTF){
                        long int numero=atoi(dato.toStdString().c_str());
                        regis<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str();
                    }
                    if(validador->getField_Type()==DEC){
                        long double numero=atof(dato.toStdString().c_str());
                        regis<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str();
                    }

                }catch(...){
                    QMessageBox::information(this,"Problema","Error en el proceso de registro, verifique los tipos de datos ingresados");
                }
            }
            if(this->avail_list.at(this->avail_list.size()-1)==-1){
                ofstream escritor(this->nombre_del_archivo.c_str(),ios::out | ios::app);
                escritor.write(regis.str().c_str(),size_Registro);
                escritor.flush();
                escritor.close();

            }else{
                //modificar_Avail_list(avail_list);
                ofstream escritor(this->nombre_del_archivo.c_str());
                int offset=sizeof(estructura)+((avail_list.at(this->avail_list.size()-1))*size_Registro);
                escritor.seekp(offset);
                escritor<<left<<setw(size_Registro)<<regis.str().c_str();
                escritor.flush();
                escritor.close();
                QString linea_edit=ui->tabla_estructura->itemAt(0,campo_llave);
                string my_llave_texto=linea_edit.toStdString();
                int my_key=atoi(my_llave_texto.c_str());
                Index nuevo_indice(my_key,avail_list.at(avail_list.size()-1));
                this->my_tree->Agregar(nuevo_indice);
            }
        }
    }catch(...){

    }


}

void registro::modificar_registro_de_archivo(){
    stringstream regis;
    bool datos_completos=true;
    for(int i=0; i<ui->tabla_estructura->columnCount(); i++){
        QString contenedor=ui->tabla_estructura->item(0,i);
        if(contenedor.isEmpty())
            datos_completos=false;
    }
    if(!datos_completos){
        QMessageBox::information(this,"Datos imcompletos","Debe llenar todos los campos");
    }else{
        for(int i=0; i<ui->tabla_estructura->columnCount(); i++){
            try{
                QString dato=ui->tabla_estructura->item(0,i);
                campo* validador=campos->at(i);
                if(validador->getField_Type()==CHAR){
                    regis<<dato.toStdString()<<",";
                }
                if(validador->getField_Type()==INTF){
                    long int numero=atoi(dato.toStdString().c_str());
                    regis<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str();
                }
                if(validador->getField_Type()==DEC){
                    long double numero=atof(dato.toStdString().c_str());
                    regis<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str();
                }

            }catch(...){
                QMessageBox::information(this,"Problema","Error en el proceso de registro, verifique los tipos de datos ingresados");
            }
        }
        //se re-escribe el registro
        ofstream escritor(this->nombre_del_archivo.c_str());
        int offset=sizeof(estructura)+((this->indice_temporal.getTreeRRN())*size_Registro);
        escritor.seekp(offset);
        escritor<<left<<setw(size_Registro)<<regis.str().c_str();
        escritor.flush();
        escritor.close();
        QMessageBox::information(this,"Atencion","Registro modificado exitosamente");
    }

}
void registro::validar_campos_en_tabla(){
    QMessageBox::information(this,"eso","Entramos a validar tablas");
    ui->tabla_estructura->insertRow(ui->tabla_estructura->rowCount());
    for(int i=0; i<campos->size(); i++){
        campo* temp=campos->at(i);
        QLineEdit* linea=new QLineEdit();

        if(temp->getField_Type()==INTF){
            linea->setValidator(new QIntValidator());
        }
        if(temp->getField_Type()==DEC){
           linea->setValidator(new QDoubleValidator());
        }

        linea->setMaxLength(temp->getSize_campo());
        ui->tabla_estructura->insertColumn(ui->tabla_estructura->columnCount());
        ui->tabla_estructura->setHorizontalHeaderItem((ui->tabla_estructura->columnCount()-1),new QTableWidgetItem(temp->getNombre()));
        ui->tabla_estructura->setCellWidget(0,(ui->tabla_estructura->columnCount()-1),linea);
    }
}
void registro::cargar_avail_list(){
    ifstream lector(this->nombre_del_archivo.c_str());
    char header[10000];
    lector.read(header,10000);
    lector.seekg(0, lector.beg);
    stringstream linea;
    linea<<header;
    vector<string>datos_header=split(linea.str(),',');
    string numero=datos_header.at(0);
    int espacio=atoi(numero.c_str());
    this->avail_list.push_back(espacio);
    if(espacio>-1){
        while(espacio!=-1){
            int offset=sizeof(estructura)+(espacio*size_Registro);
            lector.seekg(offset);
            char registro[size_Registro];
            lector.read(registro,size_Registro);
            lector.seekg(0,lector.beg);
            stringstream interprete_registro;
            interprete_registro<<registro;
            vector<string>partes_registro=split(interprete_registro.str(),',');
            string pieza=partes_registro.at(1);
            espacio=atoi(pieza.c_str());
            this->avail_list.push_back(espacio);
        }
    }
    QMessageBox::information(this,"Cargado","EL avail list se cargo exitosamente");
    lector.close();
}
void registro::eliminar_registro(){

    if(avail_list.at(avail_list.size()-1)==-1){
        //obtenemos el header
        char header[sizeof(estructura)];
        char registro_a_borrar[sizeof(size_Registro)];
        ifstream lectura(nombre_del_archivo.c_str());
        lectura.read(header,sizeof(estructura));
        lectura.seekg(0,lectura.beg);
        int offset=sizeof(estructura)+(indice_temporal.getTreeRRN()*size_Registro);
        lectura.seekg(offset);
        lectura.read(registro_a_borrar,size_Registro);
        lectura.close();
        //ahora re-escribiremos el header y el registro
        //primero el header
        stringstream nuevo_header;
        nuevo_header<<header;
        stringstream registro_eliminado;
        registro_eliminado<<registro_a_borrar;
        vector<string>partes_header=split(nuevo_header.str(),',');
        vector<string>partes_registro=split(registro_eliminado.str(),',');
        stringstream header_actualizado;
        stringstream registro_muerto;
        for(int i=0; i<partes_header.size(); i++){
            if(i==0)
                header_actualizado<<indice_temporal.getTreeRRN()<<",";
            else
                header_actualizado<<partes_header.at(i)<<",";
        }
        registro_muerto<<"*,-1,";
        for(int i=0; i<partes_registro.size(); i++){
           registro_muerto<<partes_registro.at(i)<<",";
        }

        //se procede a actualizar el archivo
        ofstream escritor(this->nombre_del_archivo.c_str());
        if(escritor.is_open()){
            escritor<<left<<setw(sizeof(estructura))<<header_actualizado.str().c_str();
            escritor.seekp(0,escritor.beg);
            int offset=sizeof(estructura)+(indice_temporal.getTreeRRN()*size_Registro);
            escritor.seekp(offset);
            escritor<<endl;
            escritor<<left<<setw(size_Registro)<<registro_muerto.str().c_str();
        }
        this->my_tree->Eliminar(indice_temporal);
    }else{
        //debemos leer el registro completo debido a que no sabemos la cantidad de digitos a escribir
        ifstream lectura(this->nombre_del_archivo.c_str());
        //calculamos el offset
        int offset=sizeof(estructura)+(this->indice_temporal.getTreeRRN()*size_Registro);
        char registro_a_borrar[size_Registro];
        lectura.seekg(offset);
        lectura.read(registro_a_borrar,size_Registro);
        stringstream ultimo_registro_borrado;
        ultimo_registro_borrado<<registro_a_borrar;
        //ahora leeremos el ultimo registro borrado actualemnte
        lectura.seekg(0, lectura.beg);
        //calculamos de nuevo el offset esta vez correspondiente al ultimo registro borrado
        offset=sizeof(estructura)+(avail_list.at(avail_list.size()-1)*size_Registro);
        lectura.seekg(offset);
        char penultimo_registro_borrado[size_Registro];
        lectura.read(penultimo_registro_borrado,size_Registro);
        stringstream penultimo_eliminado;
        penultimo_eliminado<<penultimo_registro_borrado;
        lectura.close();

        //ahora procedemos a editar los regisros leidos
        vector<string>partes_regitro_a_borrar=split(ultimo_registro_borrado.str(),',');
        vector<string>partes_ultimo_registro_borrado=split(penultimo_eliminado.str(),',');
        stringstream nuevo_registro_borrado;
        stringstream ultimo_registro_borrado2;
        for(int i=0; i<partes_regitro_a_borrar.size(); i++){
            if(i==0)
                nuevo_registro_borrado<<"*,";
            if(i==1)
                nuevo_registro_borrado<<"-1,";
            if(i!=0&&i!=1)
                nuevo_registro_borrado<<partes_regitro_a_borrar.at(i)<<",";
        }
        for(int i=0; i<partes_ultimo_registro_borrado.size(); i++){
            if(i==0)
                ultimo_registro_borrado2<<"*,";
            if(i==1)
                ultimo_registro_borrado2<<indice_temporal.getTreeRRN()<<",";
            if(i!=0&&i!=1)
                ultimo_registro_borrado2<<partes_ultimo_registro_borrado.at(i)<<",";
        }
        //se procede a re-escribir los registros
        ofstream escritor(this->nombre_del_archivo.c_str());
        offset=sizeof(estructura)+(indice_temporal.getTreeRRN()*size_Registro);
        escritor<<endl;
        escritor<<left<<setw(size_Registro)<<nuevo_registro_borrado.str().c_str();
        escritor.seekp(0, escritor.beg);
        offset=sizeof(estructura)+(avail_list.at(avail_list.size()-1)*size_Registro);
        escritor<<endl;
        escritor<<left<<setw(size_Registro)<<ultimo_registro_borrado2.str().c_str();
        escritor.flush();
        escritor.close();
        //se procede a agregar el nuevo espacio dispobile al avail list
        avail_list.push_back(indice_temporal.getTreeRRN());
        this->my_tree->Eliminar(indice_temporal);
    }


}

void registro::on_buscar_llave_clicked(){
    string my_llave=ui->llave_primaria->text().toStdString();
    int the_key=atoi(my_llave.c_str());
    int RRN=this->my_tree->Buscar(the_key);
    if(RRN==-1){
        QMessageBox::information(this,"Busqueda","No se encontro ninguna llave");
    }else{
        Index indice_encontrado(the_key,RRN);
        this->indice_temporal=indice_encontrado;
        if(this->tipo_operacion==2||this->tipo_operacion==3){
            //cargaremos el archivo que queremos eliminar o editar
            int offset=sizeof(estructura)+(RRN*size_Registro);
            ifstream lectura(this->nombre_del_archivo.c_str());
            char mi_registro[size_Registro];
            lectura.seekg(offset);
            lectura.read(mi_registro,size_Registro);
            lectura.close();
            stringstream interprete;
            interprete<<mi_registro;
            vector<string>partes_registro=split(interprete.str(),',');
            for(int i=0; i<partes_registro.size(); i++){
                campo* campo_temporal=campos->at(i);
                if(campo_temporal->getField_Type()==CHAR){
                    QString dato=QString::fromStdString(partes_registro.at(i));
                    ((QLineEdit*)ui->tabla_estructura->itemAt(0,i))->setText(dato);
                }
                if(campo_temporal->getField_Type()==INTF||campo_temporal->getField_Type()==DEC){
                    QString dato=QString::fromStdString(partes_registro.at(i));
                    ((QLineEdit*)ui->tabla_estructura->itemAt(0,i))->setText(dato);
                }
            }
        }
    }
}



void registro::on_agregar_nuevo_registro_clicked()
{
    agregar_registro_al_Archivo();
}

void registro::on_borrar_registro_clicked()
{
    eliminar_registro();
}

void registro::on_modificacion_registro_clicked()
{
    modificar_registro_de_archivo();
}
