/*!
 * \file cooperativeballpassingfsm.h
 * \brief MUR 2022 水中协作顶球策略
 * \author zymelaii
 * \date 2022-07-15
 */

#include "../official/strategyhelper.h"
#include <functional>
#include <initializer_list>
#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <utility>


/**
 * @brief 基于状态机运行的水中协作顶球策略类
 * @note 该策略引入了视觉图像信息以便于鱼的调试，同时其将充分利用图像信息，进行场地的定位等操作。
 * 这意味着意义不明的常量将基本不会出现在策略代码中，以便于调试与维护。
 * @attention 务必确保策略对象的生命周期不短于赛程。
 */
class CooperativeBallPassingFSM : public CStrategy {
public:
	/**
	 * @brief 集成鱼对象类
	 */
	struct Fish {
		/**
		 * @brief 重置绑定关系
		 * @param target_fish 待绑定的鱼对象
		 * @param target_action 待绑定的鱼对象对应的动作对象
		 * @return 当前鱼对象
		 */
		Fish& reset(CFishInfo* target_fish, CFishAction* target_action);

		//! \defgroup CooperativeBallPassingFSM鱼类预设动作
		//! @{
		/**
		 * @brief 通用点对点移动
		 * @param goal 目标点
		 * @param speed_gear 速度挡位（-1表示使用原速度）
		 * @note 指令内核为CStrategy基类的spinP2PMove函数，该方法从综合调参的方法得到，
		 * 故而对一些特殊情形，该指令的执行结果可能远低于期望。对于特殊情形，请酌情特殊化处理。
		 */
		void trivalMove(const CPoint& goal, int speed_gear);

		/**
		 * @brief 原地稳定悬停
		 */
		void stableHover();

		/**
		 * @brief 斜角移动
		 * @param goal 目标点
		 * @details 当目标点在视野左侧，顺时针运动向上侧穿越，否则逆时针运动向下侧穿越。
		 * @attention 确保目标点在前方区域
		 * @note 尚未实现
		 */
		void bevelMove(const CPoint& goal);
		//! @}

		/*! @brief 绑定的鱼对象 */
		CFishInfo* fish;

		/*! @brief 绑定鱼对应的动作对象 */
		CFishAction* action;
	};

public:
	/**
	 * @brief 策略入口
	 * @param aAction 鱼动作列表（长度不可信赖）
	 * @param aFish 鱼列表（长度不可信赖）
	 * @param aBallinfo 水球列表（长度不可信赖）
	 * @param aObstacle 障碍物列表（长度不可信赖）
	 * @param aChannel 通道列表
	 * @return 策略是否被成功调用（该值被忽略）
	 * @attention 调用该策略之前，务必先调用updateVisualFrame。
	 */
	virtual bool Strategy(RefArray<CFishAction> aAction, RefArray<CFishInfo> aFish,
						  RefArray<CBallInfo> aBallinfo, RefArray<OBSTAINFO> aObstacle,
						  RefArray<CHANNEL> aChannel) override;

	/**
	 * @details 所有的状态配置都将在此构造函数中完成，除此之外构造函数不做任何读写数据的操作。
	 */
	explicit CooperativeBallPassingFSM() noexcept;

	/**
	 * @brief 区域枚举值
	 */
	enum Region {
		Empty  = 0b00000000,	 /*!< 未知区域 */
		Upper  = 0b00000001,	 /*!< 上半平面 */
		Lower  = 0b00000010,	 /*!< 下半平面 */
		Left   = 0b00000100,	 /*!< 左三分平面 */
		Middle = 0b00001000,	 /*!< 中三分平面 */
		Right  = 0b00010000,	 /*!< 右三分平面 */
		UL	   = Upper | Left,	 /*!< 左上区域 */
		UM	   = Upper | Middle, /*!< 中上区域 */
		UR	   = Upper | Right,	 /*!< 右上区域 */
		LL	   = Lower | Left,	 /*!< 左下区域 */
		LM	   = Lower | Middle, /*!< 中下区域 */
		LR	   = Lower | Right,	 /*!< 右下区域 */
	};

	/**
	 * @brief 策略核，策略运行的基础单元
	 * @details 策略核以当前策略对象为参数，运行返回值由核的具体含义指定。
	 */
	using PolicyKernel = std::function<bool(CooperativeBallPassingFSM*)>;

	/**
	 * @brief 策略约束
	 * @details 策略约束是策略行为的约束，指明了策略的准入与准出情形。
	 * 准入情形指明了策略在何种情形下被启动运行，而准出情形定义了策略执行完毕的标志。
	 * 当准入情形通过时，策略对象应尽可能转向启动该策略。
	 * 当准出情形通过时，策略对象必须将策略流程传递至下一策略。
	 */
	using PolicyConstraint = std::pair<PolicyKernel, PolicyKernel>;

	/**
	 * @brief 策略类型
	 * @details 策略类型是策略行为与策略约束的集合，是独立的模块。
	 */
	using Policy = std::pair<PolicyKernel, PolicyConstraint>;

	/**
	 * @brief 策略阶段枚举值（自动机状态定义）
	 */
	enum class Phase {
		Origin /*!< 初始起点（空状态） */
	};

	/**
	 * @brief 区域判定
	 * @param pos 待判定的点位
	 * @return 所属区域枚举值
	 */
	Region regionPredict(const CPoint& pos) const;

	/**
	 * @brief 阶段预测
	 * @return 当前所处的策略阶段
	 * @attention 具体执行的策略将依据该方法进行分发调用
	 */
	Phase phasePredict(const CPoint& a_pos, const CPoint& b_pos, const CPoint& ball_pos) const;

	/**
	 * @brief 更新视觉帧
	 * @attention 平台传入帧图像的生命周期不定，只能保证在一次策略调用中其未被销毁，
	 * 但是无法保证下一次调用其依旧存活。当访问已被销毁的帧图像时，该行为将导致平台崩溃。
	 * 所以请确保在每一次策略调用前都调用对平台的传参调用该函数。
	 * @return 是否更新成功
	 * @retval true 视觉帧已更新
	 * @retval false 视觉帧所处的内存地址未更新
	 */
	bool updateVisualFrame(IplImage* image);

	/**
	 * @brief 场景刷新
	 * @details 完成相关变量的配置与更新，需要在updateVisualFrame之后，
	 * Strategy之前执行。
	 * @param aAction 鱼动作列表（长度不可信赖）
	 * @param aFish 鱼列表（长度不可信赖）
	 * @param aBallinfo 水球列表（长度不可信赖）
	 * @param aObstacle 障碍物列表（长度不可信赖）
	 * @param aChannel 通道列表
	 * @param wait_for_track 是否等待视觉追踪完全，若不等待将直接返回真；
	 * 否则当前追踪丢失，直接返回假。
	 * @return 是否刷新成功
	 * @note 当通道标定不全时（强制性要求三通道），函数始终返回假。
	 * @attention 刷新失败时，应尽可能取消调用当前轮次策略。
	 */
	bool refreshScene(RefArray<CFishAction> aAction, RefArray<CFishInfo> aFish,
					  RefArray<CBallInfo> aBallinfo, RefArray<OBSTAINFO> aObstacle,
					  RefArray<CHANNEL> aChannel, bool wait_for_track = true);

	/**
	 * @brief 空策略核，始终返回真。
	 */
	static bool null_policy(CooperativeBallPassingFSM*) { return true; }

	/**
	 * @brief 注册策略
	 * @param name 待注册的策略名
	 * @param action 策略行为
	 * @param in_constraint 准入策略约束
	 * @param out_constraint 准出策略约束
	 * @return 已注册的策略
	 * @retval std::nullopt 策略名冲突
	 */
	std::optional<Policy> registerPolicy(std::string name, PolicyKernel action,
										 PolicyKernel in_constraint, PolicyKernel out_constraint);

	/**
	 * @brief 配置策略
	 * @param src_policy 源策略
	 * @param dst_policies 后继策略列表（不存在的策略将被忽略）
	 * @return 配置是否成功
	 * @retval false 源策略不存在
	 */
	bool configurePolicy(std::string src_policy, std::initializer_list<std::string> dst_policies);

public:
	/**
	 * @brief 获取门中心坐标
	 * @param index 门的标号（0/1/2分别顺序对应三个门）
	 * @return 指定标号门的中心坐标
	 */
	CPoint door(int index) const;

	/**
	 * @brief 获取鱼实例
	 * @param index 鱼编号（0/1分别对应A鱼和B鱼）
	 * @return 请求的鱼实例
	 */
	Fish& fish(int index);

	/**
	 * @brief 获取水球对象
	 * @return 水球对象
	 */
	const CBallInfo& ball() const;

	/**
	 * @brief 获取常量表
	 * @return 当前帧常量表
	 */
	const auto& constants() const;

private:
	/*! @brief 目标球门中心坐标 */
	CPoint door_center_[3];

	/*! @brief 当前帧鱼实例 */
	Fish fishes_[2];

	/*! @brief 当前帧水球对象 */
	CBallInfo* ball_;

	/*! @brief 当前帧视觉图像 */
	IplImage* visframe_;

	/*! @brief 字符串-策略编码转换表 */
	std::unordered_map<std::string, int> simap_;

	/*! @brief 策略表 */
	std::map<int, Policy> policies_;

	/*! @brief 策略状态转移图 */
	std::map<int, std::list<int>> std_;

	/*! @brief 常量表 */
	struct {
		int width;	/*!< 场地宽度 */
		int height; /*!< 场地高度 */
		struct {

		} dist; /*!< 距离常量 */
	} constants_;
};