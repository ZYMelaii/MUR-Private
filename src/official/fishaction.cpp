/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

#include "../official/fishaction.h"

CFishAction::CFishAction() {
    init();
}

CFishAction::CFishAction(const FISHACTION &action) {
    setAction(action);
}

void CFishAction::init() {
    id = -1;
    speed = 0;
    direction = 7;
    state = 0;
    mode = 0;
    display = 0;
};

void CFishAction::setAction(const FISHACTION &action) {
    id = action.id;
    speed = action.speed;
    direction = action.direction;
    mode = action.mode;
    state = action.state;
    display = action.display;
};

CFishAction& CFishAction::operator=(const CFishAction &other) {
    setAction(*static_cast<const FISHACTION*>(&other));
    return *this;
};

bool CFishAction::operator==(const CFishAction &other) const {
    return mode == other.mode
        && speed == other.speed
        && direction==other.direction;
};