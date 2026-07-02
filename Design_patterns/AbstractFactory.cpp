#include<iostream>

class Button{
    public:
     virtual void render() = 0;
     virtual ~Button() = default;
};

class Checkbox{
    public:
        virtual void render() = 0;
        virtual ~Checkbox() = default;
};

class WinButton : public Button{
    public:
        void render()override{
            std::cout<<"This is windows Button\n";
        }
};

class Wincheckbox : public Checkbox{
    public:
        void render()override{
            std::cout<<"This is windows Checkbox\n";
        }
};

class LinuxButton : public Button{
    public: 
        void render()override{
            std::cout<<"This is Linux Button\n";
        }
};
class LinuxCheckbox: public Checkbox{
    public:
        void render()override{
            std::cout<<"This is Linux Checkbox\n";
        }
};
class GUIFactory{
    public:
        virtual  Button* createButton() = 0;
        virtual Checkbox* createCheckbox() = 0;
        virtual  ~GUIFactory() = default;

};

class WinFactory : public GUIFactory{
    public:
     Button* createButton() override{
        return new WinButton();
     }
     Checkbox* createCheckbox() override{
        return new Wincheckbox();
     }
};

class LinuxFactory : public GUIFactory{
    public:
        Button* createButton()override{
            return new LinuxButton();
        }
        Checkbox* createCheckbox() override{
            return new LinuxCheckbox();
        }
};


void renderUI(GUIFactory* factory){
    Button* btn = factory->createButton();
    Checkbox* chk = factory->createCheckbox();

    btn->render();
    chk->render();
    
    delete btn;
    delete chk;
}

int main(){
    return 0;
}