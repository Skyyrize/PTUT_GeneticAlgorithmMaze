#include "CEntity.h"
#include <iostream>

CEntity::CEntity(std::vector<char> m, sf::Vector2f start) {
    shape = sf::CircleShape(5.0f);
    shape.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
    
    this->MoveList = m;
    this->SetPos(start);
}

CEntity::~CEntity() {}

void CEntity::Draw(sf::RenderWindow& w) const {
    w.draw(this->shape);
}
void CEntity::SetPos(sf::Vector2f p) {
    this->Pos = p;
    this->shape.setPosition(sf::Vector2f(p.x * 50+ rand() % 50, p.y * 50 + rand() % 50));
}
sf::Vector2f CEntity::GetPos() const {
    return this->Pos;
}
sf::Vector2f CEntity::GetPosScreen() const {
    return this->shape.getPosition();
}

void CEntity::MovePos() {
    sf::Vector2f NPos = this->GetPos();
    
    char dir = this->MoveList[moveIndex++];

    float MoveDist = 1;
    switch (dir) {
    case 'Z': {
        NPos.y -= MoveDist;
    } break;
    case 'S': {
        NPos.y += MoveDist;
    } break;
    case 'Q': {
        NPos.x -= MoveDist;
    } break;
    case 'D': {
        NPos.x += MoveDist;
    } break;
    default: break;
    }
    this->SetPos(NPos);
}

std::vector<char> CEntity::GetMoveList() const {
    return this->MoveList;
}

void CEntity::ChangeMoveList(std::vector<char> a) {
    this->MoveList = a;
}