#pragma once

class Bomba {
    private:
        bool estado;
        int  pinControl;


    public:
        // Constructor
        Bomba(int pinControl);
        void iniciar();
        void ActivarBomba();
        void ApagarBomba();
        bool estaEncendida();
};
