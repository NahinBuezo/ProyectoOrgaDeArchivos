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
#include <fstream>
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
    cout<<"vamos a campos"<<endl;
    this->size_Registro=0;
    for(int i=0; i<this->campos->size();i++){
        campo* camp=this->campos->at(i);
        size_Registro+=sizeof(camp);
    }
    cout<<"pasamos de calcular el size registro"<<endl;
    if(this->tipo_operacion==1)
        ui->label_operacion->setText("SOLO AGREGAR REGISTROS");
    if(this->tipo_operacion==2)
        ui->label_operacion->setText("SOLO EDICION DE REGISTROS");
    if(this->tipo_operacion==3)
        ui->label_operacion->setText("SOLO ELIMINAR REGISTROS");
    if(this->tipo_operacion==4)
        ui->label_operacion->setText("SOLO BUSQUEDA DE REGISTROS");
    if(this->tipo_operacion==5)
        ui->label_operacion->setText("SOLO LISTAR REGISTROS");
    cout<<"pasamos de la etiqueta"<<endl;
    validar_campos_en_tabla();
    cargar_avail_list();
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
        datos_completos=validacion_datos();
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
            for(int i=0; i<ui->tabla_estructura->columnCount(); i++){
                try{
                    QString dato=ui->tabla_estructura->item(0,i)->text();
                    campo* validador=campos->at(i);
                    if(validador->getField_Type()==CHAR){
                        regis<<dato.toStdString()<<",";
                    }
                    if(validador->getField_Type()==INTF){
                        long int numero=atoi(dato.toStdString().c_str());
                        regis<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str()<<",";
                    }
                    if(validador->getField_Type()==DEC){
                        long double numero=atof(dato.toStdString().c_str());
                        regis<<","<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str()<<",";
                    }

                }catch(...){
                    QMessageBox::information(this,"Problema","Error en el proceso de registro, verifique los tipos de datos ingresados");
                }
            }
            cout<<regis.str()<<endl;
            //cout<<"el nuevo size es: "<<size_Registro<<endl;
            if(this->avail_list.empty()){
                ofstream escritor(this->nombre_del_archivo.c_str(),ios::out|ios::app);
                escritor<<left<<setw(100)<<regis.str().c_str();
                escritor.flush();
                escritor.close();
                //QString linea_edit=ui->tabla_estructura->itemAt(0,campo_llave)->text();
                //string my_llave_texto=linea_edit.toStdString();
                //int my_key=atoi(my_llave_texto.c_str());
                //Index nuevo_indice(my_key,0);
                //this->my_tree->Agregar(nuevo_indice);
                cout<<"Registro guardado"<<endl;
            }else{
                //modificar_Avail_list(avail_list);
                ofstream escritor(this->nombre_del_archivo.c_str());
                int offset=sizeof(100001)+((avail_list.at(this->avail_list.size()))*100);
                escritor.seekp(offset);
                escritor.write(regis.str().c_str(),100);
                escritor.flush();
                escritor.close();
                //QString linea_edit=ui->tabla_estructura->itemAt(0,campo_llave)->text();
                //string my_llave_texto=linea_edit.toStdString();
                //int my_key=atoi(my_llave_texto.c_str());
                // Index nuevo_indice(my_key,avail_list.at(avail_list.size()-1));
                //this->my_tree->Agregar(nuevo_indice);
                cout<<"se guardo el registro"<<endl;
            }
        }
    }catch(...){

    }


}

void registro::modificar_registro_de_archivo(){
    stringstream regis;
    bool datos_completos=validacion_datos();

    if(!datos_completos){
        QMessageBox::information(this,"Error de datos","verifique que se ingresaron datos correctos y completos");
    }else{
        for(int i=0; i<ui->tabla_estructura->columnCount(); i++){
            try{
                QString dato=ui->tabla_estructura->item(0,i)->text();
                campo* validador=campos->at(i);
                if(validador->getField_Type()==CHAR){
                    regis<<dato.toStdString()<<",";
                }
                if(validador->getField_Type()==INTF){
                    long int numero=atoi(dato.toStdString().c_str());
                    regis<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str()<<",";
                }
                if(validador->getField_Type()==DEC){
                    long double numero=atof(dato.toStdString().c_str());
                    regis<<static_cast<std::ostringstream*>(&(std::ostringstream() << numero))->str()<<",";
                }

            }catch(...){
                QMessageBox::information(this,"Problema","Error en el proceso de registro, verifique los tipos de datos ingresados");
            }
        }
        //se re-escribe el registro
        fstream escritor;
        escritor.open(this->nombre_del_archivo.c_str(),ios::out|ios::in);
        int offset=10000+((this->indice_temporal.getTreeRRN())*100);
        escritor.seekp(offset);
        //escritor.seekg(1);
        //escritor<<endl;
        //listo<<left<<setw(size_Registro)<<regis.str().c_str();
        escritor<<left<<setw(100)<<regis.str().c_str();
        escritor.flush();
        escritor.close();
        QMessageBox::information(this,"Atencion","Registro modificado exitosamente");
    }

}
void registro::validar_campos_en_tabla(){
    QMessageBox::information(this,"eso","Entramos a validar tablas");
    ui->tabla_estructura->insertRow(ui->tabla_estructura->rowCount());
    cout<<"insertamos la fiela";
    for(int i=0; i<campos->size(); i++){
        cout<<"campo: "<<i<<endl;
        campo* temp=campos->at(i);
        ui->tabla_estructura->insertColumn(ui->tabla_estructura->columnCount());
        ui->tabla_estructura->setHorizontalHeaderItem((ui->tabla_estructura->columnCount()-1),new QTableWidgetItem(temp->getNombre()));

    }
}
void registro::cargar_avail_list(){
    cout<<"vamos a cargar availist"<<endl;
    cout<<"archivo"<<this->nombre_del_archivo<<endl;
    ifstream lector(this->nombre_del_archivo.c_str());
    char header[10000];
    lector.read(header,10000);
    lector.seekg(0, lector.beg);
    stringstream linea;
    linea<<header;
    cout<<"partimos elheader"<<endl;
    vector<string>datos_header=split(linea.str(),',');
    cout<<"esto es lo que hay en datos header: "<<datos_header.at(0)<<endl;

    cout<<"Veremos qie pedo"<<endl;
    QString dataa=QString::fromStdString(datos_header.at(0));
    if(dataa.contains("-1")==false){
        stringstream numero;
        unsigned int num;
        numero<<datos_header.at(0);
        num=atoi(numero.str().c_str());
        this->avail_list.push_back(num);
        cout<<"entro siempre"<<endl;
        while(num!=-1){
            int offset=10000+(num*100);
            lector.seekg(offset);
            char* registro=new char[100];
            lector.read(registro,100);
            lector.seekg(0,lector.beg);
            stringstream interprete_registro;
            interprete_registro<<registro;
            vector<string>partes_registro=split(interprete_registro.str(),',');
            string pieza=partes_registro.at(1);
            if(pieza=="-1")
                break;
            num=atoi(pieza.c_str());
            this->avail_list.push_back(num);

        }
    }
    QMessageBox::information(this,"Cargado","EL avail list se cargo exitosamente");
    lector.close();

}
void registro::eliminar_registro(){

    if(avail_list.empty()){
        //obtenemos el header
        char* header=new char[10001];
        char* registro_a_borrar=new char[100];
        ifstream lectura(nombre_del_archivo.c_str());
        lectura.read(header,10001);
        lectura.seekg(0,lectura.beg);
        int offset=10000+((this->indice_temporal.getTreeRRN())*100);
        lectura.seekg(offset);
        lectura.read(registro_a_borrar,100);
        lectura.close();
        //ahora re-escribiremos el header y el registro
        //primero el header
        stringstream nuevo_header;
        nuevo_header<<header;
        stringstream registro_eliminado;
        registro_eliminado<<registro_a_borrar;
        vector<string>partes_header=split(nuevo_header.str(),',');
        vector<string>partes_registro=split(registro_eliminado.str(),',');
        partes_registro.pop_back();
        partes_header.pop_back();
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
        cout<<header_actualizado.str()<<endl;
        cout<<registro_muerto.str()<<endl;
        //se procede a actualizar el archivo
        fstream escritor;
        escritor.open(this->nombre_del_archivo.c_str(),ios::out|ios::in);
        if(escritor.is_open()){
            escritor<<left<<setw(10000)<<header_actualizado.str().c_str();
            escritor.flush();
            escritor.close();
        }
        escritor.open(this->nombre_del_archivo.c_str(),ios::out|ios::in);
        if(escritor.is_open()){
            int offset=10000+((this->indice_temporal.getTreeRRN())*100);
            escritor.seekp(offset);
            escritor<<left<<setw(100)<<registro_muerto.str().c_str();
            escritor.flush();
            escritor.close();
        }
        avail_list.push_back(indice_temporal.getTreeRRN());
        cout<<"********************LA LLAVE DEL INDICE ES: "<<indice_temporal.getKeys()<<"****************8"<<endl;
        this->my_tree->Eliminar(indice_temporal);
    }else{
        //debemos leer el registro completo debido a que no sabemos la cantidad de digitos a escribir
        ifstream lectura(this->nombre_del_archivo.c_str());
        //calculamos el offset
        int offset=10000+((this->indice_temporal.getTreeRRN())*100);
        char registro_a_borrar[101];
        lectura.seekg(offset);
        lectura.read(registro_a_borrar,101);
        stringstream ultimo_registro_borrado;
        ultimo_registro_borrado<<registro_a_borrar;
        //ahora leeremos el ultimo registro borrado actualemnte
        lectura.seekg(0, lectura.beg);
        //calculamos de nuevo el offset esta vez correspondiente al ultimo registro borrado
        offset=10000+(avail_list.at(avail_list.size()-1)*100);
        lectura.seekg(offset);
        char* penultimo_registro_borrado=new char[101];
        lectura.read(penultimo_registro_borrado,101);
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
        offset=10000+((this->indice_temporal.getTreeRRN())*100);
        escritor<<endl;
        escritor<<left<<setw(100)<<nuevo_registro_borrado.str().c_str();
        escritor.seekp(0, escritor.beg);
        offset=10000+(avail_list.at(avail_list.size()-1)*100);
        escritor<<endl;
        escritor<<left<<setw(100)<<ultimo_registro_borrado2.str().c_str();
        escritor.flush();
        escritor.close();
        //se procede a agregar el nuevo espacio dispobile al avail list
        avail_list.push_back(indice_temporal.getTreeRRN());
        cout<<"********************LA LLAVE DEL INDICE ES: "<<indice_temporal.getKeys()<<"****************8"<<endl;
        this->my_tree->Eliminar(indice_temporal);
    }


}


void registro::on_agregar_nuevo_registro_clicked()
{
    if(this->tipo_operacion==1){
        agregar_registro_al_Archivo();
    }else{
        QMessageBox::information(this,"Operaciones","Actualmente esta operacion no esta permitida para evitar problemas con los datos");
    }

}

void registro::on_borrar_registro_clicked()
{
    if(this->tipo_operacion==3){
        eliminar_registro();
    }else{
        QMessageBox::information(this,"Operaciones","Actualmente esta operacion no esta permitida para evitar problemas con los datos");
    }
}

void registro::on_modificacion_registro_clicked()
{
    if(this->tipo_operacion==2){
        modificar_registro_de_archivo();
    }else{
        QMessageBox::information(this,"Operaciones","Actualmente esta operacion no esta permitida para evitar problemas con los datos");
    }
}
bool registro::validacion_datos(){
    bool correcto=true;
    try{
        for(int i=0; i<campos->size(); i++){
            QString dato=ui->tabla_estructura->item(0,i)->text();
            if(dato.isEmpty()||dato.isNull())
                correcto=false;
        }
        if(correcto==true){
            for(int i=0; i<campos->size(); i++){
                QString dato=ui->tabla_estructura->item(0,i)->text();
                campo* validador=campos->at(i);

                if(dato.length()>validador->getSize_campo())
                    correcto=false;

                if(validador->getField_Type()==INTF){
                    long int numero=atoi(dato.toStdString().c_str());

                }
                if(validador->getField_Type()==DEC){
                    long double numero=atof(dato.toStdString().c_str());
                }

            }

        }


    }catch(...){
        QMessageBox::information(this,"Problema","Error en el proceso de registro, verifique los tipos de datos ingresados");
        correcto=false;
    }
    return correcto;
}
void registro::busque_por_llave(){
    string my_llave=ui->llave_primaria->text().toStdString();
    int the_key=atoi(my_llave.c_str());
    cout<<"veamos la llave: "<<the_key<<endl;
    cout<<"veremos el arbol "<<the_key<<endl;
    int RRN=this->my_tree->Buscar(the_key);

    if(RRN==-1){
        QMessageBox::information(this,"Busqueda","No se encontro ninguna llave");
    }else{
        cout<<"RRN es: "<<RRN<<endl;
        Index indice_encontrado(the_key,RRN);
        this->indice_temporal=indice_encontrado;
        if(this->tipo_operacion==2||this->tipo_operacion==3||this->tipo_operacion==4){
            //cargaremos el archivo que queremos eliminar o editar
            int offset=10000+(RRN*100);
            ifstream lectura(this->nombre_del_archivo.c_str());
            char mi_registro[100];
            lectura.seekg(offset);
            lectura.read(mi_registro,100);
            lectura.close();
            stringstream interprete;
            interprete<<mi_registro;
            vector<string>partes_registro=split(interprete.str(),',');
            if(!partes_registro.empty()){
                partes_registro.erase(partes_registro.begin()+(partes_registro.size()-1));
                for(int i=0; i<partes_registro.size(); i++){
                    campo* campo_temporal=campos->at(i);
                    if(campo_temporal->getField_Type()==CHAR){
                        QString dato=QString::fromStdString(partes_registro.at(i));
                        ui->tabla_estructura->setItem(0,i,new QTableWidgetItem(dato));
                    }
                    if(campo_temporal->getField_Type()==INTF||campo_temporal->getField_Type()==DEC){
                        QString dato=QString::fromStdString(partes_registro.at(i));
                        ui->tabla_estructura->setItem(0,i,new QTableWidgetItem(dato));
                    }
                }
            }//fin validacion si el vector esta vacio
        }
    }
}

void registro::on_boton_busqueda_indice_clicked()
{
    busque_por_llave();
}


