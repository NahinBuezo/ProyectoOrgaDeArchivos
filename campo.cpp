#include "campo.h"
#include <string>
#include <sstream>
#include <QString>
using namespace std;
campo::campo(const char* nombre,FieldType tipo,int size,int size_dec,bool key){
    strcpy(this->name,nombre);
    this->tipo=tipo;
    this->size=size;
    this->size_dec=size_dec;
    this->key=key;
}

QString campo::getNombre(){
    stringstream data;
    data<<name;
    QString nombre=QString::fromStdString(data.str());
    return nombre;
}
FieldType campo::getField_Type(){
    return tipo;
}

int campo::getSize_campo(){
    return this->size;
}

int campo::getSize_dec(){
   return this->size_dec;
}

bool campo::getLlave(){
    return this->key;
}
void campo::setNombre(const char* nombre){
    /*string titulo=nombre.toStdString();
    for(int i=0; i<30;i++){
        name[i]=' ';
    }
    for(int i=0; i<titulo.length();i++){
        name[i]=titulo[i];
    }*/
     //strcpy(this->name,nombre.toStdString().c_str());
}

void campo::setSize_campo(int size){
    this->size=size;
}

void campo::setSize_dec(int size_dec){
    this->size_dec=size_dec;
}

void campo::setLLave(bool key){
    this->key=key;
}

void campo::setField_Type(FieldType tipo){
    this->tipo=tipo;
}
