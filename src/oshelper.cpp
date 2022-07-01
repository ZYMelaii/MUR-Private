#include "./oshelper.h"

std::ostream& operator<<(std::ostream& os, const CPoint& point) {
    os << "(" << point.x << ", " << point.y << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CFishInfo& fish) {
    os <<
        "fish[" << fish.fishId() << "] {\n" <<
        "    teamId: " << fish.teamId() << "\n" <<
        "    teamName: " << fish.teamName() << "\n" <<
        "    lastCenterPos: " << fish.lastCenterPos() << "\n" <<
        "    centerPos: " << fish.centerPos() << "\n" <<
        "    headerPos: " << fish.headerPos() << "\n" <<
        "    velocity: " << fish.velocity() << "cm/s\n" <<
        "    angularVelocity: " << fish.angularVelocity() << "rad/s\n" <<
        "    targetPos: " << fish.targetPos() << "\n" <<
        "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CFishAction& action) {
    os <<
        "action[" << action.id << "] {\n" <<
        "    speed: " << action.speed << "gear\n" <<
        "    direction: " << action.direction << "gear\n" <<
        "    mode: " << static_cast<int>(action.mode) << "\n" <<
        "    state: " << action.state << "\n" <<
        "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CBallInfo &ball) {
    os <<
        "ball {\n" <<
        "    center: " << ball.getCenter() << "\n" <<
        "    lastCenter: " << ball.getPrevCenter() << "\n" <<
        "    radius: " << ball.getRadius() << "cm\n" <<
        "    velocity: " << ball.getSpeed() << "cm/s\n" <<
        "    direction: " << ball.getDirection() << "rad\n" <<
        "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CHANNEL& channel) {
    os <<
        "channel {\n" <<
        "    center: " << channel.center << "\n" <<
        "    width: " << channel.width << "\n" <<
        "    angle: " << channel.angle << "\n" <<
        "}";
    return os;
}