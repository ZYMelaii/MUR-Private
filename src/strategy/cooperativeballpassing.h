#include "../official/strategyhelper.h"

/**
 * @brief ����ˮ��Э���������
 * @note �ò���ʱ����ɣ���ȷ�����Ե��������ڲ���������
 * @author zymelaii
 * @date 2022-07-04
 */
class CooperativeBallPassing : public CStrategy {
public:
	/**
	 * @brief Ĭ�Ϲ��죬����Ҫ�ĳ�ʼ��
	 * @param width ʶ��ͼ��Ŀ��
	 * @param height ʶ��ͼ��ĸ߶�
	 */
	CooperativeBallPassing(int width, int height);

	/**
	 * @brief ����ˮ��Э���������
	 * @note ���԰�Ԥ����Ƶ��������У������ӦԴ��
	 */
	virtual bool Strategy(
		RefArray<CFishAction> aAction,
		RefArray<CFishInfo>   aFish,
		RefArray<CBallInfo>   aBallinfo,
		RefArray<OBSTAINFO>   aObstacle,
		RefArray<CHANNEL>     aChannel) override;

public:
	/**
	 * @brief �ȶ���ָͣ��
	 * @note ԭ��תȦ
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void stableHoverInstruct(CFishInfo &fish, CFishAction &action);

	/**
	 * @brief ˮ�����򻮷�
	 */
	enum Region {
		Empty  = 0b00000000, /// δ֪����
		Upper  = 0b00000001, /// �ϰ�ƽ��
		Lower  = 0b00000010, /// �°�ƽ��
		Left   = 0b00000100, /// ������ƽ��
		Middle = 0b00001000, /// ������ƽ��
		Right  = 0b00010000, /// ������ƽ��
		UL = Upper | Left,   /// ��������
		UM = Upper | Middle, /// ��������
		UR = Upper | Right,  /// ��������
		LL = Lower | Left,   /// ��������
		LM = Lower | Middle, /// ��������
		LR = Lower | Right,  /// ��������
	};

	/**
	 * @brief �����ж�
	 * @param pos ���ж��ĵ�λ
	 * @return ���������Region��־
	 */
	Region regionPredict(const CPoint &pos) const;

protected:
	/**
	 * @brief ���Խ׶λ���
	 * @note ���Ի���Ϊ����С�׶β���ִ�У������׶������в���
	 *  ��������������
	 */
	enum class Phase { Stage_1st, Stage_2nd, Stage_3rd };

	/**
	 * @brief �׶�Ԥ��
	 * @note ����ִ�еĲ��Խ����ݸ÷������зַ����á�
	 * @return ��ǰӦ��ִ�еĲ��Խ׶�
	 */
	Phase phasePredict() const;

	/**
	 * @brief ��һ�׶β���
	 * @note A�㶥�����Ž�����ƽ��������֤B�����㹻��ռ䣬
	 *  ά��ˮ����ȶ�λ��Ӧ�ڴ˽׶ν��У�ͬʱB��Ӧ����֤A����
	 *  ����ƽ��Ĺ��������㹻�Ļ�ռ䡣
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void StageInstruct_1st(RefArray<CFishAction> aAction);

	/**
	 * @brief �ڶ��׶β���
	 * @note ��ʱA���Ѿ�������ƽ�棬B�㿪ʼ�����һ���Ž���
	 *  ������ƽ�沢�����������������Ϸ���ͬʱA����������
	 *  ���ƶ�������������ˮ�����Ͻǵȴ����ص�ע��A����B���
	 *  �������ͬ���ʡ�
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void StageInstruct_2nd(RefArray<CFishAction> aAction);

	/**
	 * @brief �����׶β���
	 * @note A���Ѿ��������Ž�����ƽ�����Ͻǵȴ���B�㽫������
	 *  ��ƽ�沢׼�����ţ�ͬʱA��ȴ�B��������ɲ���ס�����š�
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void StageInstruct_3rd(RefArray<CFishAction> aAction);

protected:
	/**
	 * @brief ���س�ʼ��
	 * @note �������ȴ�ͨ�����ſ����趨��ϲ���ʼ��������Ϣ��
	 *  �ȴ��Ľ�������Ϊ��1����2������λ�ý��趨��ϡ�
	 * @return �Ƿ���ɳ�ʼ��
	 */
	bool init(const RefArray<CHANNEL> &aChannel);

	/**
	 * @brief ׷�ٲ����±���ʵ������ˮ����Ϣ
	 * @note �ú������Դ��ε���Ч�Խ��м�⣬������Ч��Ϣ��
	 *  ���ĳ�Ա������ģ�����������ݵĸ��¡�
	 * @param aAction ԭʼ�����б�
	 * @param aFish ԭʼ����Ϣ�б�
	 * @param aBallinfo ԭʼˮ����Ϣ�б�
	 * @return �Ƿ�Ӧ��ģ�����
	 */
	bool trackAndUpdate(
		const RefArray<CFishAction> &aAction,
		const RefArray<CFishInfo>   &aFish,
		const RefArray<CBallInfo>   &aBallinfo);

private:
	/**
	 * @brief �Ƿ���ɳ�ʼ��
	 * @note ���в�����Ӧ�ڳ�ʼ�����֮�����
	 */
	bool initialized_;

	/**
	 * @brief ˮ�ؿ��
	 */
	int width_, height_;

	/**
	 * @brief �������ŵ�����λ��
	 */
	CPoint door_center_[3];

	/**
	 * @brief ʵ������Ϣʵʱ����
	 * @note ���궨��ʧʱ������Ϣ������ģ�����
	 */
	CFishInfo fish_[2];

	/**
	 * @brief ˮ����Ϣʵʱ����
	 * @note ���궨��ʧʱ������Ϣ������ģ�����
	 */
	CBallInfo ball_;
};