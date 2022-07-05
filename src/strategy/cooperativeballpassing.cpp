#include "cooperativeballpassing.h"
#include <algorithm>
#include <bitset>

CooperativeBallPassing::CooperativeBallPassing(int width, int height) :
    initialized_(false), width_(width), height_(height) {
    /**
     * @note �����Ϣ��ʼ�Ա��趨Ϊ���ж�����Ч�����������
     *  ������Ч���ж���
     *  fish_[*].centerPos() == CPoint(CFishInfo::nil, CFishInfo::nil)
     *  fish_[*].centerPos() == CPoint(-1, -1)
     *  ball_.getCenter() == CPoint(-1, -1)
     *  door_center_[*] == CPoint(0, 0)
     */
}

void CooperativeBallPassing::stableHoverInstruct(CFishInfo &fish, CFishAction &action) {
    if (!initialized_) return;
    // TODO
}

CooperativeBallPassing::Region CooperativeBallPassing::regionPredict(const CPoint &pos) const {
    int flag = Region::Empty;
    flag |= pos.y <= height_ / 2 ? Region::Upper : Region::Lower;
    flag |= pos.x <= width_ / 3 ? Region::Left : (pos.x >= width_ * 2 / 3 ? Region::Right : Region::Middle);
    return static_cast<Region>(flag);
}

CooperativeBallPassing::Phase CooperativeBallPassing::phasePredict() const {
    const auto reg_1 = regionPredict(fish_[0].centerPos());
    const auto reg_2 = regionPredict(fish_[1].centerPos());

    std::cout << "[INFO] A����������bin(" << std::bitset<8>(reg_1) << ")\n";
    std::cout << "[INFO] B����������bin(" << std::bitset<8>(reg_2) << ")\n";
    std::cout << std::flush;

    if ((reg_1 & Region::Left) && reg_2 == Region::LL) {
        return Phase::Stage_1st;
    } else if (reg_1 == Region::UR && !(reg_2 & Region::Left)) {
        return Phase::Stage_3rd;
    } else {
        return Phase::Stage_2nd;
    }
}

void CooperativeBallPassing::StageInstruct_1st(RefArray<CFishAction> aAction) {
    puts("[INFO] ����ִ�е�һ�׶β���");

    const auto threshold = getDistance(door_center_[0],
        CPoint(width_ / 7, height_ * 2 / 6));
    const auto distance = getDistance(fish_[0].headerPos(), door_center_[0]);

    /**
     * @note A�������о����Ƚӽ�һ�����Ϸ�����ӽ�һ���ţ�����Ź�����������
     */
    if ((regionPredict(fish_[0].headerPos()) & Region::Lower) || distance < threshold * 0.3) {
        auto goal = CPoint(door_center_[0].x, door_center_[0].y + height_ / 4);
        aAction[0].speed = 7;
        spinP2PMove(goal, fish_[0], aAction[0]);
    } else if (distance < threshold * 0.6) {
        aAction[0].speed = 6;
        spinP2PMove(door_center_[0], fish_[0], aAction[0]);
    } else {
        aAction[0].speed = 5;
        auto goal = CPoint(door_center_[0].x - width_ / 7, door_center_[0].y - height_ / 6);
        spinP2PMove(goal, fish_[0], aAction[0]);
    }

    /**
     * @note B����ͣ����ˮ�����½���
     * @note һ���ٶ���������λ�нϺõ��ȶ���
     */
    aAction[1].speed = 1;
    aAction[1].direction = 12;
    // spinP2PMove(CPoint(0, height_), fish_[1], aAction[1]);
}

void CooperativeBallPassing::StageInstruct_2nd(RefArray<CFishAction> aAction) {
    puts("[INFO] ����ִ�еڶ��׶β���");
}

void CooperativeBallPassing::StageInstruct_3rd(RefArray<CFishAction> aAction) {
    puts("[INFO] ����ִ�е����׶β���");
}

bool CooperativeBallPassing::Strategy(
    RefArray<CFishAction> aAction,
    RefArray<CFishInfo>   aFish,
    RefArray<CBallInfo>   aBallinfo,
    RefArray<OBSTAINFO>   aObstacle,
    RefArray<CHANNEL>     aChannel) {
    /**
     * @note δ��ʼ����ɣ��ȴ����ű궨��
     */
    if (!init(aChannel)) return false;

    /**
     * @note ���궨��ʧʱ�����������Ӿ�ʶ����Ϣ
     * @note ģ�������Ϊ��ʵ��
     */
    trackAndUpdate(aAction, aFish, aBallinfo);

    std::cout << fish_[0] << "\n" << aAction[0] << "\n";
    std::cout << fish_[1] << "\n" << aAction[1] << "\n";
    std::cout << ball_ << "\n";
    std::cout << std::flush;

    switch (phasePredict()) {
        case Phase::Stage_1st:
            StageInstruct_1st(aAction);
        break;
        case Phase::Stage_2nd:
            StageInstruct_2nd(aAction);
        break;
        case Phase::Stage_3rd:
            StageInstruct_3rd(aAction);
        break;
    }

    return true;
}

bool CooperativeBallPassing::init(const RefArray<CHANNEL> &aChannel) {
    /**
     * @note ��ʼ���Ѿ����
     */
    if (initialized_) return true;

    /**
     * @note δָ���������ŵ����꣬�ȴ��궨��
     *  ���궨λ�ô�����ʱ���ٶ����ڴ���궨�������ȴ���
     */
    if (aChannel.size() != 3) return false;

    /**
     * @note door_center_��������ֱ��Ӧ�����������ģ�
     *  �������Ӧ���ǵ����ģ�Ϊ�˷�ֹ�궨ʱ�Ĵ��󣬴�
     *  �˴��������������
     * @note �ο����꣺(220,236)��(474,240)��(600,280)
     */

    int door_id[3] = {0, 1, 2};
    CPoint refs[3] = {
        aChannel[door_id[0]].center,
        aChannel[door_id[1]].center,
        aChannel[door_id[2]].center,
    };

    if (refs[door_id[0]].x > refs[door_id[1]].x) {
        std::swap(door_id[0], door_id[1]);
    }

    if (refs[door_id[0]].x > refs[door_id[2]].x) {
        std::swap(door_id[0], door_id[2]);
    }

    if (refs[door_id[1]].x > refs[door_id[2]].x) {
        std::swap(door_id[1], door_id[2]);
    }

    door_center_[0] = refs[door_id[0]];
    door_center_[1] = refs[door_id[1]];
    door_center_[2] = refs[door_id[2]];

    return (initialized_ = true);
}

bool CooperativeBallPassing::trackAndUpdate(
    const RefArray<CFishAction> &aAction,
    const RefArray<CFishInfo>   &aFish,
    const RefArray<CBallInfo>   &aBallinfo) {
    /**
     * @brief A��B��ı����ˮ��ı��
     * @note Э������Ǿ�������Ŀ������Զ���Ϊ���ӡ����м���
     *  ��������Ϣ������{0...7}������Ϊi������Ϊi+1����Ķ�
     *  ���������Ϣͬ����һһ��Ӧ��
     * @note ���Ϊi��ˮ���Ӧ��ˮ����Ϣ�б�����Ϊi-1��λ�á�
     */

    const int fish_A_id = 1, fish_B_id = 2;
    const int ball_id = 1;
    const auto nilFishPos = CPoint(CFishInfo::nil, CFishInfo::nil);
    const auto nilBallPos = CPoint(-1, -1);

    int fish_index[2] = {-1, -1};
    int ball_index = -1;

    /**
     * @note У���ˮ�������׷���Ƿ���Ч
     */

    if (aFish.size() > (fish_A_id - 1) &&
        aFish[fish_A_id - 1].centerPos() != nilFishPos) {
        fish_index[0] = fish_A_id - 1;
    }

    if (aFish.size() > (fish_B_id - 1) &&
        aFish[fish_B_id - 1].centerPos() != nilFishPos) {
        fish_index[1] = fish_B_id - 1;
    }

    if (aBallinfo.size() > (ball_id - 1) &&
        aBallinfo[ball_id - 1].getCenter() != nilBallPos) {
        ball_index = ball_id - 1;
    }

    /**
     * @note ���������Ϣ
     * @note ����Ϣ���ڳ�ʼ���׶�δ����Ч��ȡ�����趨ΪĬ��ֵ
     */

    if (fish_index[0] != -1) {
        fish_[0] = aFish[fish_index[0]];
    } else if (fish_[0].centerPos() == nilFishPos) {
        fish_[0].centerPos() = CPoint(width_ / 7, height_ * 2 / 6);
    } else {
        // TODO
    }

    if (fish_index[1] != -1) {
        fish_[1] = aFish[fish_index[1]];
    } else if (fish_[1].centerPos() == nilFishPos) {
        fish_[1].centerPos() = CPoint(width_ / 7, height_ * 4 / 6);
    } else {
        // TODO
    }

    if (ball_index != -1) {
        ball_ = aBallinfo[ball_index];
    } else if (ball_.getCenter() == nilBallPos) {
        ball_.setProperty(CPoint(width_ * 2 / 7, height_ * 4 / 6), 128, 13);
    } else {
        // TODO
    }

    return
        fish_index[0] == -1 ||
        fish_index[1] == -1 ||
        ball_index == -1;
}