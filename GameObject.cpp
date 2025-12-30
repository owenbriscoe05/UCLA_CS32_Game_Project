//
//  GameObject.cpp
//  CS32_Project3v2
//
//  Created by Owen Briscoe on 5/18/24.
//

#include "GameObject.h"

//all accessor/set methods
void GameObject::setCol(int newC) {
    m_col = newC;
}

void GameObject::setRow(int newR) {
    m_row = newR;
}

int GameObject::row() {
    return m_row;
}

int GameObject::col() {
    return m_col;
}

string GameObject::getDescription() {
    return m_description;
}

int Scroll::getImproveAmt() {
    return m_improveAmt;
}
