#include "Physics.h"
#include <math.h>
#include <iostream>
Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto& aTransform = a->getComponent<CTransform>();
    auto& bTransform = b->getComponent<CTransform>();
    auto& aBoundingBox = a->getComponent<CBoundingBox>();
    auto& bBoundingBox = b->getComponent<CBoundingBox>();

    Vec2 delta(std::abs((aTransform.pos.x + aBoundingBox.halfsize.x) - (bTransform.pos.x + bBoundingBox.halfsize.x)),
               std::abs((aTransform.pos.y + aBoundingBox.halfsize.y) - (bTransform.pos.y + bBoundingBox.halfsize.y)));

    Vec2 overlap;

    overlap.x = (aBoundingBox.halfsize.x) + (bBoundingBox.halfsize.x) - delta.x;
    overlap.y = (aBoundingBox.halfsize.y) + (bBoundingBox.halfsize.y) - delta.y;

    return overlap;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    auto& aTransform = a->getComponent<CTransform>();
    auto& bTransform = b->getComponent<CTransform>();
    auto& aBoundingBox = a->getComponent<CBoundingBox>();
    auto& bBoundingBox = b->getComponent<CBoundingBox>();

    Vec2 delta(std::abs((aTransform.prevPos.x + aBoundingBox.halfsize.x) - (bTransform.prevPos.x + bBoundingBox.halfsize.x)),
               std::abs((aTransform.prevPos.y + aBoundingBox.halfsize.y) - (bTransform.prevPos.y + bBoundingBox.halfsize.y)));

    Vec2 overlap;

    overlap.x = (aBoundingBox.halfsize.x) + (bBoundingBox.halfsize.x) - delta.x;
    overlap.y = (aBoundingBox.halfsize.y) + (bBoundingBox.halfsize.y) - delta.y;

    return overlap;
}
