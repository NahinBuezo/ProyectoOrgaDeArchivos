#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nueva_estructura.h"
#include "registro.h"
#include "cruces.h"
#include <QMessageBox>
#include <fstream>
#include <vector>
#include "campo.h"
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
#include <cstring>

using namespace std;
//vector<string>split(string,char);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->campos=new vector<campo*>();
    this->arbol_creado=false;
}

MainWindow::~MainWindow()
{
    delete ui;

}


void MainWindow::on_actionNuevo_triggered()
{
    // QMessageBox::information(this,"Saludo","Hola como estas");
    nueva_estructura nuevo(NULL,campos,true);
    //nuevo.setVisible(
    //this->hide();
    nuevo.setModal(true);
    nuevo.exec();
}

void MainWindow::on_actionAbrir_triggered()
{
    try{
        this->nombre_archivo_abierto=QFileDialog::getOpenFileName(this,tr("Open File"),"./","All files (*.*);;Text File (*.txt)");
        if(!nombre_archivo_abierto.isEmpty()){
            while(campos->size()>0){
                campos->erase(campos->begin());
            }
            while(ui->estructura_abierta->columnCount()>0){
                ui->estructura_abierta->removeColumn(0);
            }
            vector<string> partes=split(nombre_archivo_abierto.toStdString(),'/');
            nombre_archivo_abierto=QString::fromStdString(partes.at(partes.size()-1));
            cout<<"archivo "<<nombre_archivo_abierto.toStdString();
            ifstream lector(nombre_archivo_abierto.toStdString().c_str());
            char* linea=new char[sizeof(estructura)+1];
            if(lector.is_open()){
                lector.read(linea,sizeof(estructura)+1);
                lector.close();
            }
            cout<<"se leyo el archivo"<<endl;
            //se procede a recuperar parte por parte los campos de la estructura
            stringstream finger;

            finger<<linea;
            vector<string>partes_de_estructura=split(finger.str(),',');
            //string nombre_estructura=partes_de_estructura.at(0);
            partes_de_estructura.erase(partes_de_estructura.begin());
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
            size_registro=0;
            for(int i=0; i<campos->size(); i++){
                campo* referencia=campos->at(i);
                this->size_registro+=sizeof(referencia);
            }
            delete[]linea;
            //cargar_indice_desde_archivo();
        }
    }catch(...){

    }
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
    if(this->nombre_archivo_abierto.isEmpty()||this->nombre_archivo_abierto.isNull()){
        QMessageBox::information(this,"Atencion","No se encuentra ningun archivo abierto");
    }else{
        nueva_estructura temp(NULL,campos,2);
        temp.nombre_archivo_a_modificar=this->nombre_archivo_abierto.toStdString();
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
        bool existe_campo_llave=false;
        for(int i=0; i<campos->size();i++){
            campo*tempo=campos->at(i);
            if(tempo->getLlave()==true)
                existe_campo_llave=true;
        }
        if(existe_campo_llave=false){
            QMessageBox::information(this,"Alerta","No existe ningun campo llave, debe crear uno");
        }else{
            QString archivo=QFileDialog::getSaveFileName();
            vector<string>url=split(archivo.toStdString(),'/');
            stringstream nombre_real;

            nombre_real<<url.at(url.size()-1)<<".txt";
            this->nombre_archivo_abierto=QString::fromStdString(nombre_real.str());
            ofstream escritor(nombre_real.str().c_str());
            stringstream detalles;

            detalles<<"-1,";
            for(int i=0; i<campos->size();i++){
                campo* temp=campos->at(i);
                string size_campo=static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size))->str();
                string size_dec=static_cast<std::ostringstream*>(&(std::ostringstream() << temp->size_dec))->str();
                string llave=static_cast<std::ostringstream*>(&(std::ostringstream() << temp->key))->str();
                if(i>0)
                    detalles<<temp->name<<","<<temp->tipo<<","<<size_campo<<","<<size_dec<<","<<llave<<",";
                if(i==0)
                    detalles<<temp->name<<","<<temp->tipo<<","<<size_campo<<","<<size_dec<<","<<llave<<",";
            }
            // estructura nueva_base_de_datos(detalles.str().c_str());
            QString paja;
            paja=QString::fromStdString(detalles.str());
            QMessageBox::information(this,"Alerta",paja);
            escritor<< left <<setw(10000)<<detalles.str().c_str();
            escritor.flush();
            escritor.close();
            QMessageBox::information(this,"Guardado","Archivo guardado exitosamente");
        }


    }
}
void MainWindow::cargar_indice_desde_archivo(){
    stringstream nombre_archivo_indice;
    nombre_archivo_indice<<"indices_"<<nombre_archivo_abierto.toStdString();
    ifstream lectura(nombre_archivo_indice.str().c_str());
    if(lectura.is_open()){
        cout<<"se abrio el archivo indice"<<endl;
        while(!lectura.eof()){
            char mi_indice[51];
            lectura.read(mi_indice,51);
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
    cout<<"Se creara lel indice"<<endl;
    int posicion_llave;
    for(int i=0; i<campos->size(); i++){
        campo* campo_temporal=campos->at(i);
        if(campo_temporal->getLlave()==true)
            posicion_llave=i;
    }
    if(posicion_llave<0){
        QMessageBox::information(this,"Alerta","No se encontro ningun campo llave, debe crear uno y re-indexar");
    }else{
        cout<<"el cmapo lave es: "<< posicion_llave <<endl;

        //una ves obtenido la posicion se procede a leer el archivo leer cada registro
        //asi obtener el campo que se necesita
        fstream lectura;
        lectura.open(nombre_archivo_abierto.toStdString().c_str(),ios::in|ios::out);
        int mi_rrn=0;
        if(lectura.is_open()){
            cout<<"se leera el archivo"<<endl;
            lectura.seekg(10000);
            while(!lectura.eof()){
                cout<<"vamos..."<<mi_rrn<<endl;
                char* regis=new char[101];
                lectura.read(regis,101);
                cout<<"se leer el registro: "<<regis<<endl;

                stringstream interprete_registro;
                interprete_registro<<regis;
                vector<string>partes_registro=split(interprete_registro.str(),',');
                cout<<"NUMERO DE PARTES DE REGISTRO: "<<partes_registro.size()<<endl;
                cout<<"tenemos las partes"<<endl;
                if(partes_registro.empty()==false){
                    partes_registro.pop_back();
                    string campo_llave=partes_registro.at(posicion_llave);
                    cout<<"tenemos el campo llave en string"<<endl;
                    long int mega_llave=atoi(campo_llave.c_str());
                    cout<<"El campo llave es: "<<mega_llave<<endl;
                    cout<<"crearemis el idnice"<<endl;
                    Index indice_nuevo(mega_llave,mi_rrn);
                    cout<<"indice creado"<<endl;
                    arbol_b.Agregar(indice_nuevo);

                    cout<<"vamos a crear elarbol"<<endl;
                    mi_rrn++;
                }
                delete[]regis;
            }
            lectura.close();
            QMessageBox::information(this,"Arbol","El arbol se creo y cargo correctamente");
        }
    }

}


void MainWindow::on_actionExportar_Excel_triggered(){
    int num=0;
    if(num==0){
        ifstream file("personasfinalHEADER.txt");
        string header;
        string secondary_name = "ToExcel.txt";
        getline(file, header);
        ofstream outfile(secondary_name.c_str(), ios::app);
        outfile << header;
        outfile.close();
        file.close();
    }
    ifstream file("personasfinalHEADER.txt");
    string header;
    string name = "ToCSV.csv";
    getline(file, header);
    vector<string> lineas;
    string line = "";

    while(!file.eof()){
        string tmp_line;
        getline(file,tmp_line);
        line+=tmp_line;
        line+='\n';
    }

    string linea_final = "";

    for(int i = 0; i < line.length();i++){
        if(line[i]=='_'){
            linea_final+=' ';
        }else{
            linea_final+=line[i];
        }
    }

    ofstream outfile(name.c_str(), ios::app);
    outfile << linea_final;

    outfile.close();
    file.close();
}

void MainWindow::on_actionSalir_triggered(){
    exit(0);
}

void MainWindow::on_actionIntroducir_registros_triggered()
{
    cout<<"nombre de archivo en mainwindow: "<<nombre_archivo_abierto.toStdString()<<endl;
    if(this->nombre_archivo_abierto.isEmpty()||this->nombre_archivo_abierto.isNull()){
        QMessageBox::information(this,"alerta","Debe abrir un archivo");
    }else{
        registro regis(NULL,campos,&arbol_b,1,this->nombre_archivo_abierto.toStdString());
        regis.setModal(true);
        regis.exec();

    }



}

void MainWindow::on_actionModificar_registros_triggered()
{
    cout<<"nombre de archivo en mainwindow: "<<nombre_archivo_abierto.toStdString()<<endl;
    if(this->nombre_archivo_abierto.isEmpty()||this->nombre_archivo_abierto.isNull()){
        QMessageBox::information(this,"alerta","Debe abrir un archivo");
    }else{
        if(this->arbol_creado==false){
            QMessageBox::information(this,"Alerta","Debe crear el indice o cargarlo a memoria, seleccione reindexar");
        }else{
            registro regis(NULL,campos,&arbol_b,2,this->nombre_archivo_abierto.toStdString());
            regis.setModal(true);
            regis.exec();
        }
    }
}

void MainWindow::on_actionBuscar_registros_triggered()
{
    cout<<"nombre de archivo en mainwindow: "<<nombre_archivo_abierto.toStdString()<<endl;
    if(this->nombre_archivo_abierto.isEmpty()||this->nombre_archivo_abierto.isNull()){
        QMessageBox::information(this,"alerta","Debe abrir un archivo");
    }else{
        if(this->arbol_creado==false){
            QMessageBox::information(this,"Alerta","Debe crear el indice o cargarlo a memoria, seleccione reindexar");
        }else{
            registro regis(NULL,campos,&arbol_b,4,this->nombre_archivo_abierto.toStdString());
            regis.setModal(true);
            regis.exec();
        }
    }
}

void MainWindow::on_actionBorrar_registros_triggered()
{
    cout<<"nombre de archivo en mainwindow: "<<nombre_archivo_abierto.toStdString()<<endl;
    if(this->nombre_archivo_abierto.isEmpty()||this->nombre_archivo_abierto.isNull()){
        QMessageBox::information(this,"alerta","Debe abrir un archivo");
    }else{
        if(this->arbol_creado==false){
            QMessageBox::information(this,"Alerta","Debe crear el indice o cargarlo a memoria, seleccione reindexar");
        }else{
            registro regis(NULL,campos,&arbol_b,3,this->nombre_archivo_abierto.toStdString());
            regis.setModal(true);
            regis.exec();
        }
    }
}

void MainWindow::on_actionListar_registros_triggered()
{
    cout<<"nombre de archivo en mainwindow: "<<nombre_archivo_abierto.toStdString()<<endl;
    if(this->nombre_archivo_abierto.isEmpty()||this->nombre_archivo_abierto.isNull()){
        QMessageBox::information(this,"alerta","Debe abrir un archivo");
    }else{
        while(ui->estructura_abierta->rowCount()>0){
            ui->estructura_abierta->removeRow(0);
        }
        cout<<"borramos las filas"<<endl;
        vector<string>registros;
        fstream lectura;
        lectura.open(nombre_archivo_abierto.toStdString().c_str(),ios::in|ios::out);
        if(lectura.is_open()){
            string line;
            getline(lectura,line);
            while(!lectura.eof()){
                char* registro=new char[100];
                lectura.read(registro,100);
                stringstream datos;
                datos<<registro;
                registros.push_back(datos.str());
            }
            lectura.close();
        }
        cout<<"leimos los registros"<<endl;
        cout<<"escribiremos los registros"<<endl;
        registros.pop_back();
        for(int i=0; i<registros.size(); i++){
            vector<string>partes_registro=split(registros.at(i),',');
            cout<<i<<endl;
            int row=ui->estructura_abierta->rowCount();
            ui->estructura_abierta->insertRow(ui->estructura_abierta->rowCount());
            for(int j=0; j<ui->estructura_abierta->columnCount(); j++){
                QString informacion=QString::fromStdString(partes_registro.at(j));
                ui->estructura_abierta->setItem(row,j,new QTableWidgetItem(informacion));
            }
            if(i==45)
                break;
        }
    }
}



void MainWindow::on_actionExportar_JSON_triggered()
{

    ifstream file("personasfinalHEADER.txt");
    string header;
    string secondary_name = "ToJSON.txt";
    getline(file, header);
    ofstream outfile(secondary_name.c_str(), ios::app);
    outfile << header;
    outfile.close();
    file.close();
    QMessageBox::information(this,"Exportacion a JSON","La exportacion a JSON completa");


}

void MainWindow::on_actionCrear_indices_triggered()
{
    if(arbol_b.getRoot()==-1||this->nombre_archivo_abierto.isEmpty()){
        QMessageBox::information(this,"Alerta","Debe existir un archivo abierto e indices en el arbol");
    }else{
        recorrer_arbol();
        QMessageBox::information(this,"Recorrido","El archivo fue creado con exito");
    }
}
void MainWindow::recorrer_arbol(){
    stringstream nombre_indice;
    nombre_indice<<"indices_"<<this->nombre_archivo_abierto.toStdString();
    ofstream escritor(nombre_indice.str().c_str());
    for(int i=0; i<arbol_b.arbol.size(); i++){
        Node* mi_nodo=arbol_b.arbol.at(i);
        for(int j=0; j<mi_nodo->getKeys().size(); j++){
            Index mi_indice=mi_nodo->getKeys().at(j);
            stringstream indice_temp;
            indice_temp<<mi_indice.getKeys()<<","<<mi_indice.getTreeRRN();
            escritor<<left<<setw(50)<<indice_temp.str().c_str();
            escritor<<endl;
        }
    }
    escritor.flush();
    escritor.close();
}

void MainWindow::on_actionExportar_XML_con_Schema_triggered()
{
    ifstream get_txt("personasfinalHEADER.txt");
    const string FILE_HEAD = "<?xml version=\"1.0\"?>";

    //creamos variables y llamamos los primeros atributos del archivo
    int campos, registros;
    string name, name_infile;
    get_txt >> name;
    get_txt >> campos;
    get_txt >> registros;

    //vector q se usara
    vector<string> campo_nombre;

    //para infile
    name_infile = name;

    //buscamos los campos
    for(int i = 0; i < campos; i++){
        string temp_name;
        string temp_type;
        int temp_size;
        string is_key;
        int decimals;
        get_txt >> temp_name;
        get_txt >> temp_size;
        get_txt >> temp_type;
        get_txt >> is_key;
        get_txt >> decimals;
        campo_nombre.push_back(temp_name);
    }

    //a xml-schema, se busca el nombre
    ofstream outdata;
    name+=".xml";

    outdata.open(name.c_str(), ios::app);

    outdata << FILE_HEAD << endl;
    outdata << '<' << name_infile << '>' << endl;

    for(int i = 0; i < registros;i++){
        string fout[campos];
        for(int j = 0; j < campos; j++){
            get_txt >> fout[j];
            cout << campo_nombre.at(j) << " " << fout[j] << " ";
            outdata << '\t' << '<' << campo_nombre.at(j) << ">" << fout[j] << "</" << campo_nombre.at(j) << '>' << endl;
        }

        cout << endl;

    }//for i

    outdata << "</" << name_infile << '>';

    get_txt.close();
    outdata.close();


}

void MainWindow::on_actionCruzar_Archivos_triggered()
{
    cruces nueva_cruzada;
    nueva_cruzada.ejecutar();
    QMessageBox::information(this,"Listo","Nueva cruzada con exito, se guardo el archivo nuevo");
}

void MainWindow::on_actionRe_indexar_archivo_triggered()
{
    if(nombre_archivo_abierto.isNull()||nombre_archivo_abierto.isEmpty()){
        QMessageBox::information(this,"Alerta","Debe abrir un archivo");
    }else{
        Tree arbol(16);
        arbol_b=arbol;
        cargar_indice_desde_archivo();
        this->arbol_creado=true;
    }

}

void MainWindow::on_generador_clicked()
{
    vector<string>partes_persona;
    vector<string>partes_lugar;
    fstream lectura;
    // lectura.open("personasfinalHEAD.txt",ios::in|ios::out);
    /* if(lectura.is_open()){
        char* header=new char[58];
        lectura.read(header,58);
        while(!lectura.eof()){
            char* linea=new char[34];
            lectura.read(linea,linea);
            partes_persona.push_back(linea);
        }
        lectura.close();
    }
    */
    lectura.open("Lugares.txt",ios::in|ios::out);
    if(lectura.is_open()){
        cout<<"HELLLOOOOOOOOOOOOOOOOOO"<<endl;
        while(!lectura.eof()){
            //cout<<"vamos"<<endl;
            string linea;
            getline(lectura,linea);
            //cout<<linea<<endl;
            stringstream d;
            d<<linea;
            //cout<<d.str()<<endl;
            partes_lugar.push_back(d.str());
        }
        cout<<"SAlimos"<<endl;
        lectura.close();
    }
    //partes_lugar.erase(partes_lugar.begin());
    vector<string>global;
    //partes_lugar.erase(partes_lugar.begin());
    for(int i=0; i<partes_lugar.size(); i++){
        string cadena=partes_lugar.at(i);
        //cout<<cadena<<endl;
        vector<string>mis_partes=split(cadena,',');
        mis_partes.pop_back();
        stringstream generado;
        for(int j=0; j<mis_partes.size(); j++){
            generado<<mis_partes.at(j)<<",";
        }
        cout<<"GENERADO: "<<generado.str()<<endl;
        global.push_back(generado.str());
        cout<<"se agrego"<<endl;
        if(i==45)
            break;
    }
    cout<<"Salimos de l cagada"<<endl;
    for(int i=0; i<global.size(); i++){
        cout<<global.at(i)<<endl;
    }
    ofstream escritor("places.txt");
    escritor<<left<<setw(10000)<<global.at(0).c_str();
    escritor.flush();
    escritor.close();

    escritor.open("places.txt",ios::out|ios::app);
    escritor<<endl;
    for(int i=1; i<global.size(); i++){
        escritor<<left<<setw(100)<<global.at(i).c_str()<<endl;
    }
    escritor.flush();
    escritor.close();

    /*escritor.open("people.txt",ios::out|ios::in);
    for(int i=0; i<partes_persona.size(); i++){
        if(i==0){
            escritor<<left<<setw(10000)<<partes_persona.at(i).c_str();
        }else{
            escritor<<left<<setw(100)<<partes_persona.at(i).c_str();
        }
    }
    escritor.flush();
    escritor.close();
    */
    QMessageBox::information(this,"listo","se cargaron");
}
