#include<iostream>
class GamePlayer{
private:
    static const int NumTurns =5 ;
    int scores[NumTurns];
public:
    void print(){
        std::cout << NumTurns << std::endl;
    }
};
int main(){
    GamePlayer g;
    g.print();
    return 0;
}