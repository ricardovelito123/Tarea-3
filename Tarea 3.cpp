#include <iostream>
#include <list>
#include <map>
#include <functional>
#include <string>
using namespace std;

class Entity {
    private:
    int vida, energia, nivel, recursos;
    string nombre;
    int x, y;

    public:
    Entity(string nombre, int x, int y, int vida, int energia, int nivel, int recursos) : nombre(nombre), x(x), y(y), vida(vida), energia(energia), nivel(nivel), recursos(recursos) {}

    int getVida()    const {
        return vida;
    }
    string getNombre() const {
        return nombre;
    }

    void mover(int ab, int ac) {
        x = ab;
        y = ac;
    }
    void curar(int n) {
        vida = vida + n;
    }
    void damage(int n) {
        vida = vida - n;
    }
    void mostrarS() const {
        cout << "Nombre: " << nombre << " | Vida: " << vida << " | Pos(" << x << "," << y << ")\n";
    }
};

using Command = function<void ( const std :: list < std :: string >&) >;

class CommandCenter {
private:
    Entity& entity;
    list<string> histo_ejecucion;
    map<string, Command> comandos;
public:
    CommandCenter(Entity& entity) : entity(entity) {}

    void registrarComando(const string& nombree,  Command comando) {
        comandos[nombree] = comando;
    }
    void ejecutarComando(const string& nombree, const list<string>& a) {
        map<string, Command>::iterator b = comandos.find(nombree);
        if (b != comandos.end()) {
            string entrada = nombree + " | vida antes: " + to_string(entity.getVida());
            b->second(a);

            entrada += "- > vida despues: " + to_string(entity.getVida());
            histo_ejecucion.push_back(entrada);
        } else {
            cout << "Comando no encontrado:  " << nombree << endl;
        }
    }

    void eliminarComando(const string& nombree) {
        map<string, Command>::iterator b = comandos.find(nombree);
        if (b != comandos.end()) {
            comandos.erase(b);
            cout << "Comando eliminado: " << nombree << "\n";
        } else {
            cout << "Comando no existe: " << nombree << "\n";
        }
    }

    void mostrarHistorial() {
        list <string>:: iterator a = histo_ejecucion.begin();
        for (; a != histo_ejecucion.end(); ++a) {
            cout << *a << "\n";
        }
    }
};

//funcion libre
void cmdStatus(Entity& e, const list<string>& a) {
    e.mostrarS();
}

//functor: clase propia
class DamageCommand {
    Entity& e;
    int contador;
public:
    DamageCommand(Entity& ent) : e(ent), contador(0) {}

    void operator()(const list<string>& arg) {
        if (arg.size() != 1)
            return;
        int n = stoi(*arg.begin());
        e.damage(n);
        contador++;
        cout << "Veces usado: " << contador << endl;
    }
};

//func helper para validar numeros.
bool esNum(const string& a) {
    try {
        stoi(a);
        return true;
    } catch (...) {
        return false;
    }
}

int main() {
    Entity e("Nombre", 0, 0, 100, 50, 1, 200);
    CommandCenter center(e);

    //lambda
    auto cmdCurar = [&e](const list<string>& arg) {
        if (arg.empty()) {
            cout << "error: falta valor\n";
            return;
        }
        if (!esNum(arg.front())) {
            cout << "error: debe ser un numero\n";
            return;
        }
        e.curar(stoi(arg.front()));
    };

    //lambda move
    auto cmdMove = [&e](const list<string>& arg) {
        if (arg.size() < 2) {
            cout << "error: move requiere x y\n";
            return;
        }
        auto a = arg.begin();
        int x = stoi(*a++);
        int y = stoi(*a);
        e.mover(x, y);
    };

    // este es el functor
    DamageCommand dmg(e);

    // registrar
    center.registrarComando("heal",   cmdCurar);
    center.registrarComando("move",   cmdMove);
    center.registrarComando("damage", dmg);
    center.registrarComando("status", [&e](const list<string>& a) {
        cmdStatus(e, a);
    });

    // ejecucion de validos
    list<string> args;
    args.push_back("20");
    center.ejecutarComando("heal", args);

    args.clear();
    args.push_back("5"); args.push_back("10");
    center.ejecutarComando("move", args);

    // invalidos
    args.clear();
    center.ejecutarComando("heal", args);

    // historial
    center.mostrarHistorial();
}