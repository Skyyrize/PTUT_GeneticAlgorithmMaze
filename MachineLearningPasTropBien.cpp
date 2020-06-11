#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include <time.h>
#include <queue>
#include <algorithm>

#include "CEntity.h"

//https://github.com/daancode/a-star 
#include "AStar.hpp"


AStar::Generator generator;

sf::Clock WaveTime;
sf::Clock MoveTime;
unsigned Generation = 0;
std::vector<CEntity*> EntList;
const std::vector<char> PossibleMove{ 'Z', 'S', 'Q', 'D' };

/*
https://en.wikipedia.org/wiki/Genetic_algorithm
Crée la population de départ
Selection
Croissement
Mutation
*/

//GA config
unsigned GA_Steps = 0; //Varie en fonction de la distance entre les point d'entrer et de sortie (distance_between +2)
unsigned GA_Population = 100;
unsigned GA_MutationChance = 5;
float GA_RefreshTime = 0.5f;

//Maze config
sf::Vector2f Entrance(5, 1);
sf::Vector2f Sortance(1, 3);
std::vector<std::string> LevelBase{
        "0000000",
        "0111010",
        "0111110",
        "0111010",
        "0001110",
        "0111110",
        "0000000",
};

void DrawPixelRectangle(sf::Vector2f p, sf::Image& img, sf::Color col) {
    p.x = p.x * 50;
    p.y = p.y * 50;
    for (unsigned i = 0; i < 50; i += 1) {
        for (unsigned y = 0; y < 50; y += 1) {
            img.setPixel(p.x + i, p.y + y, col);
        }
    }
}

sf::Image CreateLevel() {
    sf::Image LevelImage;
    LevelImage.create(LevelBase.size() * 50, LevelBase.size() * 50);

    for (unsigned y = 0; y < LevelBase.size(); y += 1) {
        for (unsigned x = 0; x < LevelBase[y].size(); x += 1) {
            switch (LevelBase[x][y]) {
            case 'S': { DrawPixelRectangle(sf::Vector2f(y, x), LevelImage, sf::Color::Blue); } break;
            case 'E': { DrawPixelRectangle(sf::Vector2f(y, x), LevelImage, sf::Color::Green); } break;
            case '0': { DrawPixelRectangle(sf::Vector2f(y, x), LevelImage, sf::Color::Red); generator.addCollision({ (int)y, (int)x }); } break;
            case '1': { DrawPixelRectangle(sf::Vector2f(y, x), LevelImage, sf::Color::Cyan); } break;
            default: break;
            }
        }
    }
    return LevelImage;
}


/* GA */
void CreateFirstPopulation(std::vector<char> m) {
    for (unsigned i = 0; i < GA_Population; i += 1) {
        for (unsigned i = 0; i < GA_Steps; i++) {
            m[i] = PossibleMove[rand() % PossibleMove.size()];
        }
        CEntity* ent = new CEntity(m, Entrance);
        EntList.push_back(ent);
    }
}

std::vector<char> Crossover(std::vector<char> m1, std::vector<char> m2) {

    //Two-point and k-point crossover
    //https://en.wikipedia.org/wiki/Crossover_(genetic_algorithm)

    std::vector<unsigned> SortedListPt;
    SortedListPt.push_back(rand() % m1.size());
    SortedListPt.push_back(rand() % m2.size());
    sort(SortedListPt.begin(), SortedListPt.end());

    std::vector<char> nMoveList = m1;

    for (unsigned i = SortedListPt[0]; i < SortedListPt[1]; i++) {
        nMoveList[i] = m2[i];
    }

    return nMoveList;
}

void Mutation(CEntity* m) {
    std::vector<char> MoveList = m->GetMoveList();
    if (rand() % 100 <= GA_MutationChance) {
        //x (entre 1 et 2 au pif) de la liste de move qui change
        unsigned lucky_num = rand() % 2;
        for (unsigned i = 0; i < lucky_num; i++) {
            MoveList[rand() % MoveList.size()] = PossibleMove[rand() % PossibleMove.size()];
        }
        m->ChangeMoveList(MoveList);
    }
}

void CreatePopulation(CEntity* m1, CEntity* m2) {

    std::vector<char> m1_moves = m1->GetMoveList();
    std::vector<char> m2_moves = m2->GetMoveList();

    for (CEntity* EntInf : EntList) {
        EntInf->~CEntity();
    }
    EntList.clear();
    
    for (unsigned i = 0; i < GA_Population; i += 1) {
        CEntity* ent = new CEntity(Crossover(m1_moves, m2_moves), Entrance);
        Mutation(ent);
        EntList.push_back(ent);
    }

}

// utility comparator function to pass to the sort() module
bool sortByVal(const std::pair<CEntity*, unsigned>& a, const std::pair<CEntity*, unsigned>& b) {
    return (a.second < b.second);
}

void Selection() {
    if (EntList.size() <= 2) {
        CreateFirstPopulation(std::vector<char>(GA_Steps));
        Generation = 0;
        std::cout << "EXTERMINATION : Generation " << 0 << std::endl;
        return;
    }
    std::cout << "SELECTION : Generation " << Generation++ << std::endl;

    static CEntity* SelectedFirst;
    static CEntity* SelectedSeconde;
    
    //https://www.educative.io/edpresso/how-to-sort-a-map-by-value-in-cpps
    std::vector<std::pair<CEntity*, unsigned>> BestEntities;
    for (CEntity* EntInf : EntList) {
        BestEntities.push_back( std::make_pair(EntInf, EntInf->movePenality));
    }
    sort(BestEntities.begin(), BestEntities.end(), sortByVal);
    SelectedFirst = BestEntities[0].first;
    SelectedSeconde = BestEntities[1].first;

    if (SelectedFirst != nullptr && SelectedSeconde != nullptr) {
        CreatePopulation(SelectedFirst, SelectedSeconde);
    }


    /*

    unsigned min_penality = 999999;
    for (CEntity* EntInf : EntList) {
        if (EntInf->movePenality < min_penality) {
            SelectedFirst = EntInf;
            min_penality = EntInf->movePenality;
        }
    }

    unsigned min_dist = 9999;
    for (CEntity* EntInf : EntList) {

        sf::Vector2f entpos = EntInf->GetPos();
        auto path = generator.findPath({ (int)entpos.x, (int)entpos.y }, { (int)Sortance.x, (int)Sortance.y });
        if (path.size() < min_dist) {
            SelectedFirst = EntInf;
            min_dist = path.size();
        }

        
        sf::Vector2f entpos = EntInf->GetPos();
        unsigned distance = abs(entpos.x - Sortance.x) + abs(entpos.y - Sortance.y);
        if (distance < min_dist) {
            SelectedFirst = EntInf;
            min_dist = distance;
        }
    }
    */

}

int steps = 0;
void GameLogic() {
    for (unsigned i = 0; i < EntList.size(); i++) {
        EntList[i]->MovePos();
        sf::Vector2f nPos = EntList[i]->GetPos();
        
        if (nPos.x >= LevelBase.size() || nPos.y >= LevelBase.size()) {
            EntList.erase(EntList.begin() + i);
            continue;
        }
        if (nPos.x < 0 || nPos.y < 0) {
            EntList.erase(EntList.begin() + i);
            continue;
        }
        switch (LevelBase[nPos.y][nPos.x]) {
            case 'S': {
                std::cout << "SOLUTION TROUVE" << std::endl;
            } break;
            case '0': {
                EntList.erase(EntList.begin() + i);
            }break;
        default: break;
        }

        //A chaque move, la penalité de l'entité augment par rapport a la distance qu'il a gagné par rapport au tour qu'il a jouer.
        //Donc ca penalise ceux qui vont a l'oposé ou qui font des tour sur eux même

        auto path = generator.findPath({ (int)nPos.x, (int)nPos.y }, { (int)Sortance.x, (int)Sortance.y });
        EntList[i]->movePenality += path.size();
    }

    
    steps++;
    if (steps >= GA_Steps) {
        steps = 0;
        Selection();
    }
}

int main() {

    unsigned cinPopulation;
    float cinRefreshTime;
    unsigned cinMutationChance;
    std::cout << "Taille de la population initial: ";
    std::cin >> cinPopulation;
    std::cout << "Chance de mutation (Entre 1 et 5 cest bien): ";
    std::cin >> cinMutationChance;
    std::cout << "Temps de rafraichissement (par etape): ";
    std::cin >> cinRefreshTime;

    int Ex, Ey, Sx, Sy;
    std::cout << "Position X, Y (Entree): ";
    std::cin >> Ex >> Ey;

    std::cout << "Position X, Y (Sortie): ";
    std::cin >> Sx >> Sy;

    GA_MutationChance = cinMutationChance;
    GA_RefreshTime = cinRefreshTime;
    GA_Population = cinPopulation;

    Entrance = sf::Vector2f(Ex, Ey);
    Sortance = sf::Vector2f(Sx, Sy);

    LevelBase[Entrance.y][Entrance.x] = 'E';
    LevelBase[Sortance.y][Sortance.y] = 'S';



    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(LevelBase.size()*50, LevelBase.size() * 50), "SFML works!");

    std::cout << "Appuyez sur n'importe quel touche pour continuer";
    char a; std::cin >> a;

    generator.setWorldSize({ (int)LevelBase.size(), (int)LevelBase.size() });
    generator.setHeuristic(AStar::Heuristic::manhattan);
    generator.setDiagonalMovement(false);

    //Maze
    sf::Sprite WindowSprite;
    sf::Texture WindowTexture;
    sf::Image LevelBuild = CreateLevel();
    WindowTexture.loadFromImage(LevelBuild);
    WindowSprite.setTexture(WindowTexture);

    auto path = generator.findPath({ (int)Entrance.x, (int)Entrance.y }, { (int)Sortance.x, (int)Sortance.y });
    GA_Steps = path.size() + 2;


    CreateFirstPopulation(std::vector<char>(GA_Steps));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (MoveTime.getElapsedTime().asSeconds() >= GA_RefreshTime) {
            GameLogic();
            MoveTime.restart();
        }

        window.clear();
        window.draw(WindowSprite);

        for (CEntity* InfEntity : EntList) {
            InfEntity->Draw(window);
        }
        
        window.display();
    }
    return 0;
}