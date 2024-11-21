#include "Utils.h"

Red::WorldPosition Game::ToRed(const glm::vec3& acPosition)
{
    Red::WorldPosition pos;
    pos.x = acPosition.x;
    pos.y = acPosition.y;
    pos.z = acPosition.z;
    return pos;
}

glm::vec3 Game::ToGlm(const Red::WorldPosition& acPosition)
{
    glm::vec3 pos{acPosition.x, acPosition.y, acPosition.z};
    return pos;
}

Red::Quaternion Game::ToRed(const glm::quat& acOrientation)
{
    return {acOrientation.x, acOrientation.y, acOrientation.z, acOrientation.w};
}

glm::quat Game::ToGlm(const Red::Quaternion& acOrientation)
{
    return {acOrientation.r,
        acOrientation.i,
        acOrientation.j,
        acOrientation.k};
}