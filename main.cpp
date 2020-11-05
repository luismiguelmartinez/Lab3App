#include <iostream>
#include <fstream>

using namespace std;


void system ();
void administrator ();
void client (string);
bool validateUser (string, string);
string encrypData (string);
void binaryToText (string, ofstream *);
void loadDataBase ();
string extractUserData (ifstream *, string, unsigned short);
void createNewUser ();
void checkMoney (string);
void cashOut (string);
void eraseTempFiles ();

int main()
{
    system();

    return 0;
}

void system () {

    eraseTempFiles();

    string id;

    cout<<endl;
    cout<<"|||||||||||||||||||||||||||||||||||||||||||"<<endl;
    cout<<"|||                                     |||"<<endl;
    cout<<"|||            BIENVENIDO AL            |||"<<endl;
    cout<<"|||          CAJERO AUTOMATICO          |||"<<endl;
    cout<<"|||                                     |||"<<endl;
    cout<<"|||           Salir presione *          |||"<<endl;
    cout<<"|||                                     |||"<<endl;
    cout<<"|||||||||||||||||||||||||||||||||||||||||||"<<endl;
    cout<<endl;
    cout<<"Identificacion: ";

    fflush(stdin);
    getline(cin, id);

    if (id.compare("*"))  {

        loadDataBase();

        if (id == "admin")
            administrator();
        else
            client(id);
    }
}

void administrator () {

    string password;
    cout << endl << "Ingrese contrasena: ";
    getline(cin, password);

    if (validateUser("admin", password)) {

        char reply;

        cout << endl << "Desea registrar un nuevo usuario y/n: ";
        cin >> reply;

        cout << endl << endl;

        if (reply == 'y')
            createNewUser();
        else if (reply == 'n')
            system();
        else
            cout << ">>>  Opcion no valida  <<<" << endl;

    } else {
        cout << ">>> Contrasena invalida <<<" << endl;
        system();
    }

}

bool validateUser (string id, string password) {

    bool flag = false;

    if (id == "admin") {

        string passEntered = encrypData(password), encrypPass;

        try {
            ifstream fin;
            fin.open("sudo.txt");
            if (!fin.is_open())
                throw '1';

            fin.seekg(0);
            while (fin.good())
                encrypPass += fin.get();

            encrypPass.erase(encrypPass.length() - 1);

            fin.close();

        }  catch (char c) {
            if (c == '1')
                cout << "validateUser 1: Error archivo de lectura temp" << endl;
            else
                cout << "validateUser 1: Error inesperado" << endl;
        }

        flag = passEntered == encrypPass;

    } else {

        try {
            ifstream fin;
            fin.open("temp.txt");
            if (!fin.is_open())
                throw '1';

            if (id != extractUserData(&fin, id, 0)) {

                cout << " >>>  El usuario no existe  <<< " << endl << endl;
                system();
            }

            flag = password == extractUserData(&fin, id, 1);

            fin.close();

        }  catch (char c) {
            if (c == '1')
                cout << "validateUser 2: Error archivo de lectura temp" << endl;
            else
                cout << "validateUser 2: Error inesperado" << endl;
        }
    }

    return flag;
}

string encrypData (string data) {

    char binary[9];
    short temp;
    string buffer, aux, encodeData;

    for (unsigned int i = 0; i < data.length(); i++) {

        temp = data[i];

        for (short j = 7; j >= 0; j--) {
            binary[j] = (temp % 2) + 48;
            temp /= 2;
        }

        buffer += binary;

        while (buffer.length() >= unsigned(4)) {

            aux = buffer.substr(3, 1);
            aux += buffer.substr(0, 3);

            encodeData += aux;
            aux.clear();
            buffer = buffer.substr(4);
        }
    }

    return encodeData;
}

void client (string user) {

    string password;
    cout << endl << "Ingrese contrasena: ";
    getline(cin, password);

    if (validateUser(user, password)) {

        char reply;

        cout << endl << "1. Consultar saldo (Costo: 1000)";
        cout << endl << "2. Retirar saldo (Costo: 1000)";
        cout << endl << "*  Para salir oprima otra tecla" << endl;
        cout << endl << "Movimiento 1/2: ";
        cin >> reply;

        cout << endl << endl;

        if (reply == '1')
            checkMoney(user);
        else if (reply == '2')
            cashOut(user);
        else
            system();

    } else {
        cout << ">>> clave invalida <<<" << endl;
        system();
    }
}

void loadDataBase () {

    try {
        ifstream fin;
        fin.open("database.txt");
        if (!fin.is_open())
            throw '1';

        ofstream fout;
        fout.open("temp.txt");
        if (!fout.is_open())
            throw '2';

        string aux, buffer;


        for (string line; getline(fin, line); ) {

            for (unsigned short i = 0; i < line.length(); i++) {

                aux += line[i];

                if (aux.length() == 4) {

                    buffer += aux.substr(1);
                    buffer += aux.substr(0, 1);

                    aux.clear();

                    while (buffer.length() >= 8) {
                        binaryToText(buffer, &fout);
                        buffer = buffer.substr(8);
                    }
                }
            }

            fout << endl;
        }

        fin.close();
        fout.close();

    }  catch (char c) {
        if (c == '1')
            cout << "loadDataBase: Error archivo de lectura" << endl;
        else if (c == '2')
            cout << "loadDataBase: Error archivo de escritura" << endl;
        else
            cout << "loadDataBase: Error inesperado" << endl;
    }
}

void binaryToText (string byte, ofstream *fout) {
    short aux1 = 128;
    short aux2 = 0;

    for (short i = 0; i < 8; i++) {
        aux2 += aux1 * (byte[i] - 48);
        aux1 /= 2;
    }

    *fout << char(aux2);
}

string extractUserData (ifstream *fin, string user, unsigned short option) {

    string aux, temp;
    unsigned short i = 0;
    bool flag = false;

    aux = "null";

    fin->seekg(0);
    for (string line; getline(*fin, line); ) {

        while (line[i] != ';') {

            temp += line[i++];
        }

        if (temp == user) {

            if (option == 0)
                return user;
            else if (option == 1 || option == 2) {
                aux.clear();
                temp = line.substr(++i);
                flag = !flag;
                i = 0;
                break;
            }
        }

        temp.clear();
        i = 0;
    }

    if (flag) {

        while (temp[i] != ';') {

            aux += temp[i++];
        }

        if (option == 1)
            return aux;

        return temp.substr(++i);
    }

    return aux;
}

void createNewUser () {

    try {
        ofstream fout;
        ifstream fin;

        if (!fout.is_open() && !fin.is_open()) {

            fout.open("database.txt", fstream::in | fstream::out | fstream::app);
            fin.open("temp.txt");
        }

        if (!fout.is_open() && !fin.is_open())
            throw '1';


        string id, pass, balance;

        cout << "Ingrese cedula: ";
        fflush(stdin);
        getline(cin, id);

        if (id == extractUserData(&fin, id, 0)) {

            cout << " >>>  El usuario ya existe  <<< " << endl << endl;

            char reply;
            cout << endl << "Salir y/n: ";
            cin >> reply;

            if (reply == 'y')
                system();
            else {
                cout << endl;
                createNewUser();
            }
        }

        cout << "Ingrese clave: ";
        getline(cin, pass);

        cout << "Ingrese saldo: ";
        getline(cin, balance);

        char reply;
        cout << endl << "Los datos ingresados son correctos y/n: ";
        cin >> reply;

        if (reply == 'y') {

            fout << encrypData(id + ';' + pass + ';' + balance) << endl;
            cout << ">>> Usuario creado con exito <<<" << endl;
        } else if (reply == 'n')

            createNewUser();
        else {

            cout << "Opcion no valida" << endl;
            system();
        }

        cout << endl << endl << "Desea crear otro usuario y/n: ";
        cin >> reply;

        if (reply == 'y') {

            createNewUser();
        } else if (reply == 'n')

            system();
        else {

            cout << "Opcion no valida" << endl;
            system();
        }

        fin.close();
        fout.close();

    }  catch (char c) {
        if (c == '1')
            cout << "createNewUser: Error archivo de lectura temp" << endl;
        else
            cout << "createNewUser: Error inesperado" << endl;
    }
}

void checkMoney (string id) {

    try {
        ifstream fin;
        fin.open("temp.txt");
        if (!fin.is_open())
            throw '1';

        ofstream fout;
        fout.open("database.txt");
        if (!fout.is_open())
            throw '2';

        bool flag = true;
        string aux, buffer;
        string pass = extractUserData(&fin, id, 1);
        unsigned int balance = atoi(extractUserData(&fin, id, 2).c_str());


        fin.seekg(0);
        for (string line; getline(fin, line); ) {

            if (flag) {

                if ((id == line.substr(0, id.length())) && (line.length() > id.length()) && (line[id.length()] == ';')) {

                    if (balance >= 1000) {

                        cout << endl << "Saldo actual: " << balance;
                        balance -= 1000;
                        cout << endl << "Saldo posterior a la operacion: " << balance << endl;

                        fout << encrypData(id + ';' + pass + ';' + to_string(balance)) << endl;
                        flag = !flag;

                    } else {

                        cout << " >>  Saldo insuficiente, no se puede realizar la operacion  <<< " << endl;
                        fout << encrypData(line) << endl;
                        flag = !flag;
                    }

                } else
                    fout << encrypData(line) << endl;
            } else
                fout << encrypData(line) << endl;
        }

        fin.close();
        fout.close();

        system();

    }  catch (char c) {
        if (c == '1')
            cout << "checkMoney: Error archivo de lectura" << endl;
        else if (c == '2')
            cout << "checkMoney: Error archivo de escritura" << endl;
        else
            cout << "checkMoney: Error inesperado" << endl;
    }
}

void cashOut (string id) {

    try {
        ifstream fin;
        fin.open("temp.txt");
        if (!fin.is_open())
            throw '1';

        bool flag = true;
        string aux, buffer;
        string pass = extractUserData(&fin, id, 1);
        unsigned int balance = atoi(extractUserData(&fin, id, 2).c_str()), cash;

        cout << "Ingrese el valor a retirar: ";
        cin >> cash;

        ofstream fout;
        fout.open("database.txt");
        if (!fout.is_open())
            throw '2';

        fin.seekg(0);

        for (string line; getline(fin, line); ) {

            if (flag) {

                if ((id == line.substr(0, id.length())) && (line.length() > id.length()) && (line[id.length()] == ';')) {

                    if (balance >= (1000 + cash)) {

                        cout << endl << "Saldo actual: " << balance;
                        balance -= (1000 + cash);
                        cout << endl << "Saldo posterior a la operacion: " << balance << endl;

                        fout << encrypData(id + ';' + pass + ';' + to_string(balance)) << endl;
                        flag = !flag;

                    } else {

                        cout << " >>  Saldo insuficiente, no se puede realizar la operacion  <<< " << endl;
                        fout << encrypData(line) << endl;
                        flag = !flag;
                    }

                } else
                    fout << encrypData(line) << endl;
            } else
                fout << encrypData(line) << endl;
        }

        fin.close();
        fout.close();

        system();

    }  catch (char c) {
        if (c == '1')
            cout << "cashOut: Error archivo de lectura" << endl;
        else if (c == '2')
            cout << "cashOut: Error archivo de escritura" << endl;
        else
            cout << "cashOut: Error inesperado" << endl;
    }
}

void eraseTempFiles () {

    try {
        ofstream fout;
        fout.open("temp.txt");
        if (!fout.is_open())
            throw '1';

        fout.close();

    }  catch (char c) {
        if (c == '1')
            cout << "eraseTempFiles: Error archivo de lectura" << endl;
        else
            cout << "eraseTempFiles: Error inesperado" << endl;
    }
}
