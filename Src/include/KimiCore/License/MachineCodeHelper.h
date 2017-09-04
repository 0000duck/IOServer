#ifndef __KIMI_KERNEL_MACHINE_CODE_HELPER_H__
#define __KIMI_KERNEL_MACHINE_CODE_HELPER_H__

_KIMI_NAMESPACE_BEGIN

class KIMI_CORE_CLASS CMachineCodeHelper
{
public:
	CMachineCodeHelper(BOOL bInit = TRUE);
	CMachineCodeHelper(const CMachineCodeHelper &mch);
	~CMachineCodeHelper(void);

	CMachineCodeHelper& operator=(const CMachineCodeHelper &mch);
	BOOL operator==(const CMachineCodeHelper &mch);

public:
	/// ��ջ�������Ϣ
	void Clear();

	/// ��ʼ����������Ϣ��������øú������򽫶�ȡ��ǰ������Ļ�������Ϣ
	BOOL Init();

	/// ��ȡ���Ļ������ı�
	string GetMachineCode();
	/// �������Ļ������ı�
	BOOL SetMachineCode(const char *lpszMachineCode);

	/// ��ȡ��ǰ�������Base64����
	string GetMachineBase64Encode();
	/// ����Base64����
	BOOL SetMachineBase64Encode(const char *lpszMachineCode);

protected:

private:
	unsigned long	m_dwVersion;
	string			m_strBIOSInfo;
	string			m_strCLogicDiskSN;
	string			m_strHardDiskSN;
	vector<string>	m_arrMacAddress;
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_MACHINE_CODE_HELPER_H__