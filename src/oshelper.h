#ifndef OSHELPER_H
#define OSHELPER_H

#include "./official/struct.h"
#include "./official/struct.h"
#include "./official/ballinfo.h"
#include "./official/fishaction.h"
#include "./official/fishinfo.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const CPoint& point);

std::ostream& operator<<(std::ostream& os, const CFishInfo& fish);

std::ostream& operator<<(std::ostream& os, const CFishAction& action);

std::ostream& operator<<(std::ostream& os, const CBallInfo &ball);

std::ostream& operator<<(std::ostream& os, const CHANNEL& channel);

#endif /*OSHELPER_H*/