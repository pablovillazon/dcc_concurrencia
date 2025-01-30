#include <thread>
#include <iostream>

class Transaccion
{
    public:
        int id;
        double monto;
        std::string moneda;
};

class PasarelaDePagos
{
    public:
        void procesarTransaccion(Transaccion transaccion)
        {
            std::thread hilo([this, transaccion]{
                //Procesar la Transaccion
                std::cout<<"Procesando la transaccion: "<<transaccion.id<<std::endl;
            });

            hilo.join();
        }
};

int main()
{
    PasarelaDePagos pasarela;
    Transaccion transaccion{1, 10.99, "USD"};
    pasarela.procesarTransaccion(transaccion);
    return 0;

}