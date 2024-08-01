#include "Physics.h"
#include <math.h>
#include <iostream>
Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 delta(std::abs((a->getComponent<CTransform>().pos.x + a->getComponent<CBoundingBox>().halfsize.x )-
        (b->getComponent<CTransform>().pos.x + b->getComponent<CBoundingBox>().halfsize.x)),
         std::abs((a->getComponent<CTransform>().pos.y + a->getComponent<CBoundingBox>().halfsize.y) -
            (b->getComponent<CTransform>().pos.y + b->getComponent<CBoundingBox>().halfsize.y)));

    Vec2 overlap;
    overlap.x = (a->getComponent<CBoundingBox>().halfsize.x) + (b->getComponent<CBoundingBox>().halfsize.x) - delta.x;
    overlap.y = (a->getComponent<CBoundingBox>().halfsize.y) + (b->getComponent<CBoundingBox>().halfsize.y) - delta.y;
    return overlap;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 delta(std::abs((a->getComponent<CTransform>().prevPos.x + a->getComponent<CBoundingBox>().halfsize.x) -
        (b->getComponent<CTransform>().prevPos.x + b->getComponent<CBoundingBox>().halfsize.x)),
        std::abs((a->getComponent<CTransform>().prevPos.y + a->getComponent<CBoundingBox>().halfsize.y) -
            (b->getComponent<CTransform>().prevPos.y + b->getComponent<CBoundingBox>().halfsize.y)));

    Vec2 overlap;
    overlap.x = (a->getComponent<CBoundingBox>().halfsize.x) + (b->getComponent<CBoundingBox>().halfsize.x) - delta.x;
    overlap.y = (a->getComponent<CBoundingBox>().halfsize.y) + (b->getComponent<CBoundingBox>().halfsize.y) - delta.y;
    return overlap;
}
