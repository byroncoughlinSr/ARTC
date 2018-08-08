#ifndef PERSON_H
#define PERSON_H
#include <string>
#include <QDate>
using namespace std;

class Person
{
public:
    Person();

public:
    struct Individual {
    int id;
    QString name;
    QString firstName;
    QString middleName;
    QString lastName;
    QString alias;
    char sex;
    QDate birthdate;
    int motherId;
    int fatherId;
    };

    struct PedigreeConstant {
        int personId;
        QString pedigreeConstanName;
        QChar pedigreeConstantParent;
        QString pedigreeConstantGrandparent;
        int pedigreeGeneration;
        int pedigreeSequence;
        int pedigreeRootId;
    };

public:
Individual individual;
PedigreeConstant pedigreeConstant;
QString getFirstName();
void setFirstName(QString);
QString getLastName();
void setLastName(QString);
QString getName();
void setName(QString);
QString getAlias();
void setAlias(QString);
void setSex(char s);
char getSex();
void setMother(int id);
int getMotherId();
void setPersonId(int id);
void setPedigreeConstantName(QString name);
void setPedigreeConstantParent(QChar parent);
void setPedigreeConstantGrandparent(QString grandparent);
void setPedigreeGeneration(int generation);
void setPedigreeSequence(int sequence);
void setPedigreeRoot(int root);
};

#endif // PERSON_H
