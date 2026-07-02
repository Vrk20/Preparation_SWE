#include<iostream>

class Button{
    public:
        virtual void show() = 0;
};

class WindowButton :public Button{
    public:
        void show() override{
            std::cout<<"this is windows Button\n";
        }
};

class LinuxButton : public Button{
    public:
        void show() override{
            std::cout<<"This is Linux Button\n";
        }
};

class Dialog{
    protected:
        virtual Button* createButton() = 0;
    public:
        void render(){
            Button* but = createButton();
            but->show();
            delete but;
        };
};

class WindowsDialog:public Dialog{
    protected:
        Button* createButton()override{
        return new WindowButton();
        }
};

class LinuxDialog: public Dialog{
    protected:
        Button* createButton() override{
            return new LinuxButton();
        }
};


int main(){
    Dialog* logistics = new LinuxDialog();
    logistics->render();
    delete logistics;
    return 0;
}