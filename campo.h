#ifndef CAMPO_H
#define CAMPO_H
#include <QString>

enum FieldType {CHAR, INTF, DEC};
class campo
{
public:
    char name[30];
    FieldType tipo;
    int size;
    int size_dec;
    bool key;

public:
    campo(const char*,FieldType,int,int,bool);
    QString getNombre();
    int getSize_campo();
    int getSize_dec();
    bool getLlave();
    FieldType getField_Type();
    void setNombre(const char*);
    void setSize_campo(int);
    void setSize_dec(int);
    void setLLave(bool);
    void setField_Type(FieldType);

    //signals:

    //public slots:
};

#endif // CAMPO_H
