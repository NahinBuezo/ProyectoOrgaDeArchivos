#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nueva_estructura.h"
#include "registro.h"
#include <QMessageBox>
#include <fstream>
#include <vector>
#include "campo.h"
#include "excel.h"
#include "btree.h"
#include "estructura.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <QFileSystemModel>
#include <QFileDialog>
#include <iomanip>


using namespace std;
//vector<string>split(string,char);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->campos=new vector<campo*>();
    Tree arbol(16);
    arbol_b=arbol;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNuevo_triggered()
{
    QMessageBox::information(this,"Saludo","Hola como estas");
    nueva_estructura nuevo(NULL,campos,true);
    //nuevo.setVisible(true);
    nuevo.setModal(true);
    nuevo.exec();
}

void MainWindow::on_actionAbrir_triggered()
{
    this->nombre_archivo_abierto=QFileDialog::getOpenFileName(this,tr("Open File"),"./","All files (*.*);;Text File (*.txt)");
    ifstream lector(nombre_archivo_abierto.toStdString().c_str());
    char linea[sizeof(estructura)];
    if(lector.is_open()){
        lector.read(linea,sizeof(estructura));
        lector.close();
    }
    //se procede a recuperar parte por parte los campos de la estructura
    stringstream finger;

    finger<<linea;
    vector<string>partes_de_estructura=split(finger.str(),',');
    //string nombre_estructura=partes_de_estructura.at(0);
    partes_de_estructura.erase(partes_de_estructura.begin()+0);
    //se calcula el numero de recorridos del vector ya que los campos y sis atributos estan mezclados
    int recorridos=(partes_de_estructura.size())/5;
    int a=0;//posicion del nombre del campo
    int b=1;//posicion de TypeField
    int c=2;//posicion de sizeo del campo
    int d=3;//posicion de size dec
    int e=4;//posicion de la llave
    QMessageBox::information(this,"Advertencia","Se cargaran los campos");
    for(int i=0; i<recorridos;i++){
        string name=partes_de_estructura.at(a+(i*5));
        string tipo=partes_de_estructura.at(b+(i*5));
        string size_c=partes_de_estructura.at(c+(i*5));
        string size_d=partes_de_estructura.at(d+(i*5));
        string logico=partes_de_estructura.at(e+(i*5));
        int tipe_field=atoi(tipo.c_str());
        int size_campo=atoi(size_c.c_str());
        int size_dec=atoi(size_d.c_str());
        bool llave=atoi(logico.c_str());
        FieldType tipo_campo;
        if(tipe_field==0){
          tipo_campo=CHAR;
        }
        if(tipe_field==1){
          tipo_campo=INTF;
        }
        if(tipe_field==2){
          tipo_campo=DEC;
        }

        //campo nuevo_campo(name.c_str(),tipo_campo,size_campo,size_dec,llave);
        campos->push_back(new campo(name.c_str(),tipo_campo,size_campo,size_dec,llave));
        this->archivo_abierto=true;
    }
    for(int i=0; i<campos->size();i++){
        campo* temp=campos->at(i);
        ui->estructura_abierta->insertColumn(ui->estructura_abierta->columnCount());
        ui->estructura_abierta->setHorizontalHeaderItem((ui->estructura_abierta->columnCount()-1),new QTableWidgetItem(temp->name));
    }
    QMessageBox::information(this,"Exito","Estructura cargada exitosamente");
   // ui->nueva_tabla_estructura->insertColumn(ui->nueva_tabla_estructura->columnCount());
    //ui->nueva_tabla_estructura->setHorizontalHeaderItem((ui->nueva_tabla_estructura->columnCount()-1),new QTableWidgetItem(nombre));

}

vector<string> MainWindow::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

void MainWindow::on_actionCrear_campos_triggered()
{
    abrir_creacion_estructura();
}
void MainWindow::abrir_creacion_estructura(){
    if(this->archivo_abierto==false){
        QMessageBox::information(this,"Atencion","No se encuentra ningun archivo abierto");
    }else{
        nueva_estructura temp(NULL,campos,false,2);
        temp.setModal(true);
        temp.exec();
    }

}

void MainWindow::on_actionListar_campos_triggered()
{
    abrir_creacion_estructura();
}

void MainWindow::on_actionModificar_campos_triggered()
{
    abrir_creacion_estructura();
}

void MainWindow::on_actionBorrar_campos_triggered()
{
    abrir_creacion_estructura();
}

void MainWindow::on_actionSalvar_triggered()
{
    if(campos->empty()){
        QMessageBox::information(this,"Alerta","No existe ningun campo creado para esta estructura");
    }else{
        QString archivo=QFileDialog::getSaveFileName();
        ofstream escritor(archivo.toStdString().c_str());
        stringstream detalles;
        detalles<<"-1,";
        for(int i=0; i<campos->size();i++){
            campo* temp=campos->at(i);
            string size_campo=static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size))->str();
            string size_dec=static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size_dec))->str();
            string llave=static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str();
            if(i>0)
                detalles<<","<<temp->name<<","<<temp->tipo<<","<<size_campo<<","<<size_dec<<","<<llave;
            if(i==0)
                detalles<<temp->name<<","<<temp->tipo<<","<<size_campo<<","<<size_dec<<","<<llave;
        }
        estructura nueva_base_de_datos(detalles.str().c_str());
        QString paja;
        paja=QString::fromStdString(detalles.str());
        QMessageBox::information(this,"Alerta",paja);
        escritor<< left <<setw(10000)<<nueva_base_de_datos.descripcion_campos;
        escritor.flush();
        escritor.close();
        QMessageBox::information(this,"Guardado","Archivo guardado exitosamente");
    }
}
void MainWindow::cargar_indice_desde_archivo(){
    stringstream nombre_archivo_indice;
    nombre_archivo_indice<<"indices_"<<nombre_archivo_abierto.toStdString();
    ifstream lectura(nombre_archivo_indice.str().c_str());
    if(lectura.is_open()){
        while(!lectura.eof()){
            char mi_indice[sizeof(Index)];
            lectura.read(mi_indice,sizeof(Index));
            stringstream interprete_indice;
            interprete_indice<<mi_indice;
            vector<string>partes_indice=split(interprete_indice.str(),',');
            string parte_llave=partes_indice.at(0);
            string parte_rrn=partes_indice.at(1);
            int llave_suprema=atoi(parte_llave.c_str());
            int RRn=atoi(parte_rrn.c_str());
            Index nuevo_indice(llave_suprema,RRn);
            this->arbol_b.Agregar(nuevo_indice);
        }
        lectura.close();
        QMessageBox::information(this,"Indices","Arbol cargado con exito");
    }else{
        crear_archivo_index();
    }
}
void MainWindow::crear_archivo_index(){
    //se procede a verificar que campo es el campo llave
    int posicion_llave;
    for(int i=0; i<campos->size(); i++){
        campo* campo_temporal=campos->at(i);
        if(campo_temporal->getLlave()==true)
            posicion_llave=i;
    }

    //una ves obtenido la posicion se procede a leer el archivo leer cada registro
    //asi obtener el campo que se necesita
    ifstream lectura(nombre_archivo_abierto.toStdString().c_str());
    int mi_rrn=0;
    if(lectura.is_open()){
        //int mover=sizeof(estructura);
        lectura.seekg(sizeof(estructura));
        while(!lectura.eof()){
            char regis[size_registro];
            lectura.read(regis,size_registro);
            stringstream interprete_registro;
            interprete_registro<<regis;
            vector<string>partes_registro=split(interprete_registro.str(),',');
            string campo_llave=partes_registro.at(posicion_llave);
            long int mega_llave=atoi(campo_llave.c_str());
            Index indice_nuevo(mega_llave,mi_rrn);
            arbol_b.Agregar(indice_nuevo);
            mi_rrn++;
        }
        lectura.close();
        QMessageBox::information(this,"Arbol","El arbol se creo y cargo correctamente");
    }
}


void MainWindow::on_actionExportar_Excel_triggered(){


}

void MainWindow::on_actionSalir_triggered(){
    exit(0);
}

void MainWindow::on_actionIntroducir_registros_triggered()
{
    registro regis(NULL,campos,&arbol_b,1,this->nombre_archivo_abierto.toStdString());
    regis.setModal(true);
    regis.exec();


}
