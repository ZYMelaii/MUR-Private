/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

/**
 * @name:          多水中机器人控制无色标平台
 * @description:   版权所有，授权使用
 * @version:       1.2.0.0
 * @copyright:     国际水中机器人联盟
 * @author:        ZhangX,XiongML
 * @date:          14-04-04 16:55
 */

#ifndef MUR_FISHACTION_H
#define MUR_FISHACTION_H

#include "../official/struct.h"

/**
 * @brief 机器鱼动作操作类
 */
class CFishAction : public FISHACTION {
public:
    /**
     * @brief 默认构造
     * @note 由init()完成
     * @author zymelaii
     */
    CFishAction();

    /**
     * @brief 从动作参数拷贝构造
     * @note 由setAction(const SetFishAction&)完成
     * @author zymelaii
     */
    CFishAction(const FISHACTION &action);

    /**
     * @brief 初始化动作参数
     * @author zymelaii
     */
    void init();

    /**
     * @brief 拷贝动作参数
     * @author zymelaii
     */
    void setAction(const FISHACTION &fishaction);

    /**
     * @brief 动作参数赋值
     * @author zymelaii
     */
    CFishAction& operator=(const CFishAction &other);
    
    /**
     * @brief 动作参数判等
     * @author zymelaii
     */
    bool operator==(const CFishAction &other) const;
};

#endif /*MUR_FISHACTION_H*/