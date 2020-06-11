#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class CEntity {
public:
    unsigned movePenality = 0;
    CEntity(std::vector<char> m, sf::Vector2f);
    ~CEntity();

    void Draw(sf::RenderWindow&) const;

    void SetPos(sf::Vector2f);
    sf::Vector2f GetPos() const;
    sf::Vector2f GetPosScreen() const;

    void MovePos();
    std::vector<char> GetMoveList() const;
    void ChangeMoveList(std::vector<char>);
private:
    unsigned moveIndex = 0;
    std::vector<char> MoveList;
    sf::Vector2f Pos;
    sf::CircleShape shape;
};
