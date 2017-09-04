#ifndef __KIMI_LICENSE_API_H__
#define __KIMI_LICENSE_API_H__

#include <string>

/// Ĭ�ϵ���Ȩ������
#define DEFAULT_AUTHORIZED_PROPERTY_NAME	"Authorized"

#define KIMI_LICENSE_API(x) x __stdcall

typedef void * KIMILIC_HANDLE;
typedef unsigned long KIMILIC_ERRCODE;

// �����붨��
#define KIMI_LIC_ERR_OK						0	// �ɹ�
#define KIMI_LIC_ERR_FAIL					1	// ʧ��
#define KIMI_LIC_ERR_INVALID_HANDLE			2	// ��Ч���
#define KIMI_LIC_ERR_FILE_NOT_EXIST			3	// ��Ȩ�ļ�������
#define KIMI_LIC_ERR_SOFTWARE_NOT_EXIST		4	// ��Ȩ��������
#define KIMI_LIC_ERR_PROPERTY_NOT_EXIST		5	// ��Ȩ���������
#define KIMI_LIC_ERR_NOT_AUTHORIZED			6	// ���δ����Ȩ
#define KIMI_LIC_ERR_MCODE_NOT_MATCH		7	// �����벻ƥ��
#define KIMI_LIC_ERR_AUTH_EXPIRED			8	// ��Ȩ����
#define KIMI_LIC_ERR_BUFFER_TOO_SMALL		9	// Buffer���Ȳ���


/* 
 * ��ָ������Ȩ�ļ��м��������Ȩ��Ϣ
 *
 * @lpszLicFileName[in]			- ��Ȩ�ļ��������ΪNULL����ʹ��Ĭ��·��DEFAULT_LICENSE_FILE
 * @lpszSoftwareName[in]		- ��Ȩ�������
 * @lpszAuthPropertyName[in]	- �����Ȩ��������Ĭ��ΪDEFAULT_AUTHORIZED_PROPERTY_NAME�����ΪNULL���򲻼�⣬������Լ�����
 * @pLicHandle[out]				- ������Ȩ��Ϣ������������ʧ�ܣ��򷵻�NULL
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_load(
			  const char *lpszLicFileName,
			  const char *lpszSoftwareName,
			  const char *lpszAuthPropertyName,
			  KIMILIC_HANDLE* pLicHandle
			  );

/* 
 * ��ָ�������뷶Χ��������Ч����Ȩ�ļ������������Ȩ��Ϣ
 *
 * @lpszLicFileMask[in]			- ��Ȩ�ļ������룬֧��*��?�����ΪNULL����ʹ��Ĭ������"*.lic"
 * @lpszSoftwareName[in]		- ��Ȩ�������
 * @lpszAuthPropertyName[in]	- �����Ȩ��������Ĭ��ΪDEFAULT_AUTHORIZED_PROPERTY_NAME�����ΪNULL���򲻼�⣬������Լ�����
 * @pLicHandle[out]				- ������Ȩ��Ϣ������������ʧ�ܣ��򷵻�NULL
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_load_mask(
			  const char *lpszLicFileMask,
			  const char *lpszSoftwareName,
			  const char *lpszAuthPropertyName,
			  KIMILIC_HANDLE* pLicHandle
			  );

/* 
 * ��ָ�����ڴ���м��������Ȩ��Ϣ
 *
 * @lpszLicBuffer[in]			- ��Ȩ�ļ����ݵ��ڴ��ָ��
 * @dwLicBufferSize[in]			- ��Ȩ�ļ����ݵ��ڴ���С
 * @lpszSoftwareName[in]		- ��Ȩ�������
 * @lpszAuthPropertyName[in]	- �����Ȩ��������Ĭ��ΪDEFAULT_AUTHORIZED_PROPERTY_NAME�����ΪNULL���򲻼�⣬������Լ�����
 * @pLicHandle[out]				- ������Ȩ��Ϣ������������ʧ�ܣ��򷵻�NULL
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_memory_load(
			  const unsigned char *lpszLicBuffer,
			  const unsigned long dwLicBufferSize,
			  const char *lpszSoftwareName,
			  const char *lpszAuthPropertyName,
			  KIMILIC_HANDLE* pLicHandle
			  );


/* 
 * �ͷż��ص���Ȩ�ļ����
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 */
KIMI_LICENSE_API(void)
kimi_lic_free(
			  KIMILIC_HANDLE hLicHandle
			  );

/*
 * ��ȡ��ǰ��Ȩ�������������
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @pdwPropertyCount[out]		- ������������
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_count(
							KIMILIC_HANDLE hLicHandle,
							unsigned long *pdwPropertyCount
							);

/*
 * ��ȡָ������λ�õ���������
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @dwIndex[in]					- ���������ţ�ȡֵΪ0����������kimi_lic_get_property_count()��ȡ������������ֵ
 * @lpszPropertyName[out]		- ���ڽ����������Ƶ�Buffer�����ΪNULL����ͨ��pdwPropertyNameSize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����ص��������ƻ��Զ����ض�
 * @pdwPropertyNameSize[in/out]	- �����lpszPropertyName��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_name(
						   KIMILIC_HANDLE hLicHandle,
						   unsigned long dwIndex,
						   char *lpszPropertyName,
						   unsigned long *pdwPropertyNameSize
						   );

/*
 * ��ȡָ����Ȩ���ԵĻ�����Ϣ
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @lpszPropertyName[in]		- Ҫ��ȡ����������
 * @lpszCategory[out]			- ���ڽ�����������Buffer�����ΪNULL����ͨ��pdwCategorySize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����ص����������Զ����ض�
 * @pdwCategorySize[in/out]		- �����lpszCategory��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 * @lpszDisplayName[out]		- ���ڽ���������ʾ���Ƶ�Buffer�����ΪNULL����ͨ��pdwDisplayNameSize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����ص�������ʾ���ƻ��Զ����ض�
 * @pdwDisplayNameSize[in/out]	- �����lpszDisplayName��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 * @lpszDescription[out]		- ���ڽ�������������Buffer�����ΪNULL����ͨ��pdwDescriptionSize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����ص������������Զ����ض�
 * @pdwDescriptionSize[in/out]	- �����lpszDescription��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 * @lpszOptions[out]			- ���ڽ������Կ�ѡ�б��Buffer�����ΪNULL����ͨ��pdwOptionsSize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����ص����Կ�ѡ�б���Զ����ض�
 * @pdwOptionsSize[in/out]		- �����lpszOptions��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 * @pnMinValue[out]				- �������Ե���Сֵ
 * @pnMaxValue[out]				- �������Ե����ֵ
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_info(
						   KIMILIC_HANDLE hLicHandle,
						   const char *lpszPropertyName,
						   char *lpszCategory,
						   unsigned long *pdwCategorySize,
						   char *lpszDisplayName,
						   unsigned long *pdwDisplayNameSize,
						   char *lpszDescription,
						   unsigned long *pdwDescriptionSize,
						   char *lpszOptions,
						   unsigned long *pdwOptionsSize,
						   int *pnMinValue,
						   int *pnMaxValue
						   );

/* 
 * ��ȡ��Ȩ״̬��Ϣ�����ǿ���ͨ���ú�����ѯ��ǰ��Ȩ�ļ�����Ȩ״̬
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @pbMachineCode[out]			- �Ƿ�Ϊ��������Ȩ�����������Ϊ�գ���ɶ�����������Ȩ��һ��ʹ��������Ȩ
 * @pnExpiredTime[out]			- ��Ȩ�������ڣ������Ȩ�ļ����ù������ڣ��򷵻�ֵΪ0
 *
 * @return						- ������Ȩ��Ч�Ա�ʶ�����룬����Ϊ��KIMI_LIC_ERR_OK��KIMI_LIC_ERR_INVALID_HANDLE��KIMI_LIC_ERR_MCODE_NOT_MATCH��KIMI_LIC_ERR_AUTH_EXPIRED
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_auth_info(
				   KIMILIC_HANDLE hLicHandle,
				   unsigned long* pbMachineCode,
				   long* pnExpiredTime
				   );

/*
 * ��ȡ��ǰ��Ȩ�ļ��ı���Ȩ������
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @lpszUserName[out]			- ���ڽ����û���Ȩ����Ȩ�����Ƶ�Buffer�����ΪNULL����ͨ��pnSize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����ص��û���Ȩ����Ȩ�����ƻ��Զ����ض�
 * @pnSize[in/out]				- �����lpszUserName��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_uername(
					 KIMILIC_HANDLE hLicHandle, 
					 char *lpszUserName, 
					 unsigned long *pdwSize
					 );

/*
 * ��ȡָ����Ȩ���Ե�BOOLֵ
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @lpszPropertyName[in]		- Ҫ��ȡ����������
 * @pbValue[out]				- ���ڽ��շ��ص�BOOL����ֵ
 * @bDefault[in]				- �����Բ�����ʱ��Ĭ��ֵ 
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_bool(
				  KIMILIC_HANDLE hLicHandle, 
				  const char *lpszPropertyName, 
				  bool *pbValue, 
				  bool bDefault
				  );

/*
 * ��ȡָ����Ȩ���Ե�����ֵ
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @lpszPropertyName[in]		- Ҫ��ȡ����������
 * @plValue[out]				- ���ڽ��շ��ص���������ֵ
 * @lDefault[in]				- �����Բ�����ʱ��Ĭ��ֵ 
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_long(
				  KIMILIC_HANDLE hLicHandle, 
				  const char *lpszPropertyName, 
				  long *plValue, 
				  long lDefault
				  );

/*
 * ��ȡָ����Ȩ���Ե�doubleֵ
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @lpszPropertyName[in]		- Ҫ��ȡ����������
 * @pdblValue[out]				- ���ڽ��շ��ص�double����ֵ
 * @dblDefault[in]				- �����Բ�����ʱ��Ĭ��ֵ 
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_double(
					KIMILIC_HANDLE hLicHandle, 
					const char *lpszPropertyName, 
					double *pdblValue, 
					double dblDefault
					);

/*
 * ��ȡָ����Ȩ���Ե��ַ���ֵ
 *
 * @hLicHandle[in]				- ��Ȩ�ļ����ؾ��
 * @lpszPropertyName[in]		- Ҫ��ȡ����������
 * @lpszBuffer[out]				- ���ڽ����ַ�������ֵ��Buffer�����ΪNULL����ͨ��pdwBufferSize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����ص��ַ�������ֵ���Զ����ض�
 * @pdwBufferSize[in/out]		- �����lpszBuffer��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 * @lpszDefault[in]				- �����Բ�����ʱ��Ĭ��ֵ 
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_string(
					KIMILIC_HANDLE hLicHandle, 
					const char *lpszPropertyName, 
					char *lpszBuffer, 
					unsigned long *pdwBufferSize, 
					const char *lpszDefault
					);

/*
 * ��ȡ��ǰ������Ļ�����
 *
 * @lpszMachineCode[out]		- ���ڽ��ջ������Buffer�����ΪNULL����ͨ��pdwSize������Ҫ����СBuffer�ռ��С�������ս�������
 *								  ������̣����صĻ�������Զ����ضϣ�һ����Է���һ��4K��С�Ļ����������Ա�֤ȫ�����յ������Ļ�����
 * @pdwSize[in/out]				- �����lpszMachineCode��С��������ʵ�ʽ��յ����ֽڴ�С���������ս�������
 *
 * @return						- ���ش�����
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_machine_code(
						  char *lpszMachineCode,
						  unsigned long *pdwSize
						  );


//////////////////////////////////////////////////////////////////////////
/// CLicenseMgr - ��Ȩģ�鸨����
//////////////////////////////////////////////////////////////////////////
class CLicenseMgr
{
public:
	CLicenseMgr() : m_hHandle(NULL) {
	}
	~CLicenseMgr() {
		Free();
	}

	/// ������Ȩ�ļ��������Ȩ�ļ����ڣ�����Ч������ֵΪtrue������Ϊfalse
	bool Load(const char *lpszLicFileName, const char *lpszSoftware, const char *lpszAuthPropertyName = DEFAULT_AUTHORIZED_PROPERTY_NAME)
	{
		Free();

		m_errCode = kimi_lic_load(lpszLicFileName, lpszSoftware, lpszAuthPropertyName, &m_hHandle);
		return ((m_hHandle != NULL) && (m_errCode == KIMI_LIC_ERR_OK));
	}

	/// ֧���������Ȩ�ļ����أ������ָ�������У����ҵ���Ч����Ȩ�ļ����򷵻�true
	bool LoadByMask(const char *lpszLicFileMask, const char *lpszSoftware, const char *lpszAuthPropertyName = DEFAULT_AUTHORIZED_PROPERTY_NAME)
	{
		Free();

		m_errCode = kimi_lic_load_mask(lpszLicFileMask, lpszSoftware, lpszAuthPropertyName, &m_hHandle);
		return ((m_hHandle != NULL) && (m_errCode == KIMI_LIC_ERR_OK));
	}

	/// ���ڴ����ݿ��м�����Ȩ��Ϣ�������Ȩ�����ڴ����Ч������ֵΪtrue������Ϊfalse
	bool Load(const unsigned char *lpszLicBuffer, const unsigned long dwLicBufferSize, const char *lpszSoftware, const char *lpszAuthPropertyName = DEFAULT_AUTHORIZED_PROPERTY_NAME)
	{
		Free();

		m_errCode = kimi_lic_memory_load(lpszLicBuffer, dwLicBufferSize, lpszSoftware, lpszAuthPropertyName, &m_hHandle);
		return ((m_hHandle != NULL) && (m_errCode == KIMI_LIC_ERR_OK));
	}

	/// �ͷ���Ȩ�ļ�
	void Free()	{
		if (m_hHandle)
		{
			kimi_lic_free(m_hHandle);
			m_hHandle = NULL;
		}
		m_errCode = KIMI_LIC_ERR_OK;
	}

	/// ��ȡ���һ�ε��õĴ�����
	KIMILIC_ERRCODE GetLastErrorCode() {
		return m_errCode;
	}

	/// ��ȡ��Ȩ�������������
	unsigned long GetPropertyCount() {
		unsigned long dwPropertyCount = 0;
		if (m_hHandle)
			m_errCode = kimi_lic_get_property_count(m_hHandle, &dwPropertyCount);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return dwPropertyCount;
	}

	/// ͨ�������Ŷ���Ȩ��������
	KIMILIC_ERRCODE GetPropertyName(unsigned long dwIndex, char *lpszPropertyName, unsigned long *pdwPropertyNameSize) {
		if (m_hHandle)
			m_errCode =	kimi_lic_get_property_name(m_hHandle, dwIndex, lpszPropertyName, pdwPropertyNameSize);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// ͨ�������Ŷ���Ȩ��������
	std::string GetPropertyName(unsigned long dwIndex) {
		std::string s;

		if (m_hHandle)
		{
			char szPropertyName[256];
			unsigned long dwPropertyNameSize = 256;

			m_errCode =	kimi_lic_get_property_name(m_hHandle, dwIndex, szPropertyName, &dwPropertyNameSize);
			if ((m_errCode == KIMI_LIC_ERR_OK) && (dwPropertyNameSize > 0))
				s = szPropertyName;
		}
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return s;
	}

	/// ��ȡ��Ȩ���Ի�����Ϣ
	KIMILIC_ERRCODE GetPropertyInfo(const char *lpszPropertyName, char *lpszCategory, unsigned long *pdwCategorySize,
		char *lpszDisplayName, unsigned long *pdwDisplayNameSize, char *lpszDescription, unsigned long *pdwDescriptionSize,
		char *lpszOptions, unsigned long *pdwOptionsSize, int *pnMinValue, int *pnMaxValue)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_property_info(m_hHandle, lpszPropertyName, lpszCategory, pdwCategorySize,
			lpszDisplayName, pdwDisplayNameSize, lpszDescription, pdwDescriptionSize, 
			lpszOptions, pdwOptionsSize, pnMinValue, pnMaxValue);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// ��ȡ��Ȩ���Ի�����Ϣ
	KIMILIC_ERRCODE GetPropertyInfo(const char *lpszPropertyName, std::string &strCategory, std::string &strDisplayName, std::string &strDescription, 
		std::string &strOptions, int &nMinValue, int &nMaxValue)
	{
		nMinValue = 0;
		nMinValue = 0;

		if (m_hHandle)
		{
			unsigned long dwCategorySize = 256;
			char *lpszCategory = (char *)malloc(dwCategorySize);
			unsigned long dwDisplayNameSize = 256;
			char *lpszDisplayName = (char *)malloc(dwDisplayNameSize);
			unsigned long dwDescriptionSize = 1024;
			char *lpszDescription = (char *)malloc(dwDescriptionSize);
			unsigned long dwOptionsSize = 1024;
			char *lpszOptions = (char *)malloc(dwOptionsSize);

			m_errCode = kimi_lic_get_property_info(m_hHandle, lpszPropertyName, lpszCategory, &dwCategorySize,
				lpszDisplayName, &dwDisplayNameSize, lpszDescription, &dwDescriptionSize, 
				lpszOptions, &dwOptionsSize, &nMinValue, &nMaxValue);

			if (m_errCode == KIMI_LIC_ERR_OK)
			{
				if (dwCategorySize > 0)
					strCategory = lpszCategory;
				else
					strCategory.clear();

				if (dwDisplayNameSize > 0)
					strDisplayName = lpszDisplayName;
				else
					strDisplayName.clear();

				if (dwDescriptionSize > 0)
					strDescription = lpszDescription;
				else
					strDescription.clear();

				if (dwOptionsSize > 0)
					strOptions = lpszOptions;
				else
					strOptions.clear();
			}
			else {
				strCategory.clear();
				strDisplayName.clear();
				strDescription.clear();
				strOptions.clear();
			}

			free(lpszCategory);
			free(lpszDisplayName);
			free(lpszDescription);
			free(lpszOptions);
		}
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// ��ȡ��Ȩ������ش����룬������Ȩ�ļ��Ƿ���ڡ��Ƿ���ڡ������Ŀ�Ƿ���ڵ���Ϣ
	KIMILIC_ERRCODE GetAuthInfo(unsigned long* pbMachineCode, long* pnExpiredTime)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_auth_info(m_hHandle, pbMachineCode, pnExpiredTime);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// ��ȡ��ǰ��Ȩ�û�����
	bool GetUserName(char *lpszUserName, unsigned long *pdwSize)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_uername(m_hHandle, lpszUserName, pdwSize);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// ��ȡ��ǰ��Ȩ�û�����
	std::string GetUserName()
	{
		if (m_hHandle)
		{
			char szUserName[256];
			unsigned long dwSize = 256;
			m_errCode = kimi_lic_get_uername(m_hHandle, szUserName, &dwSize);
			if (dwSize == 0)
				return "";
			else
				return szUserName;
		}
		else {
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;
			return "";
		}
	}

	/// ��BOOL��������ֵ������ֵΪtrue��ʾ������ֵ���ڣ�false��ʾ�����ڣ�������bDefaultĬ��ֵ
	bool AsBool(const char *lpszPropertyName, bool *bValue, bool bDefault = false)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_bool(m_hHandle, lpszPropertyName, bValue, bDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;
		
		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// ��NUMBER��������ֵ������ֵΪtrue��ʾ������ֵ���ڣ�false��ʾ�����ڣ�������lDefaultĬ��ֵ
	bool AsLong(const char *lpszPropertyName, long *lValue, long lDefault = 0)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_long(m_hHandle, lpszPropertyName, lValue, lDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// ��FLOAT��������ֵ������ֵΪtrue��ʾ������ֵ���ڣ�false��ʾ�����ڣ�������dblDefaultĬ��ֵ
	bool AsDouble(const char *lpszPropertyName, double *dblValue, double dblDefault = 0.0)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_double(m_hHandle, lpszPropertyName, dblValue, dblDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// ��STRING��������ֵ������ֵΪtrue��ʾ������ֵ���ڣ�false��ʾ�����ڣ�������lpszDefaultĬ��ֵ��
	/// lpszBuffer�ռ���Ҫ�ṩ�㹻��С���������ݽ����ضϣ����lpszBuffer==NULL�����ͨ��pdwBufferSize������Ҫ���ٿռ�
	bool AsString(const char *lpszPropertyName, char *lpszBuffer, unsigned long *pdwBufferSize, const char *lpszDefault = NULL)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_string(m_hHandle, lpszPropertyName, lpszBuffer, pdwBufferSize, lpszDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	std::string AsString(const char *lpszPropertyName, const char *lpszDefault = NULL)
	{
		std::string s;

		if (m_hHandle)
		{
			char szBuffer[256];
			unsigned long dwBufferSize = 256;
			m_errCode = kimi_lic_get_string(m_hHandle, lpszPropertyName, szBuffer, &dwBufferSize, lpszDefault);

			if (dwBufferSize > 0)
				s = szBuffer;
		}
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return s;
	}

	/// ��̬��������ȡ��ǰ������Ļ�����
	static KIMILIC_ERRCODE GetMachineCode(char *lpszMachineCode, unsigned long *pdwSize) {
		return kimi_lic_get_machine_code(lpszMachineCode, pdwSize);
	}

	/// ��̬��������ȡ��ǰ������Ļ�����
	static std::string GetMachineCode() {
		unsigned long dwSize = 1024 * 8;
		char *szMachineCode = (char *)malloc(dwSize);
		KIMILIC_ERRCODE err = kimi_lic_get_machine_code(szMachineCode, &dwSize);

		std::string s;
		if ((err == KIMI_LIC_ERR_OK) && (dwSize > 0))
		{
			s = szMachineCode;
		}

		free(szMachineCode);
		return s;
	}
protected:

private:
	KIMILIC_HANDLE m_hHandle;
	KIMILIC_ERRCODE	m_errCode;
};


#ifndef KIMI_LICENSE_EXPORTS
#	ifdef _UNICODE
#		ifdef _KIMI_LICENSE_STATIC_LINK
#			pragma comment( lib, "CLicenseS.lib")
#		else
#			ifdef _DEBUG
#				pragma comment( lib, "CLicenseD.lib")
#			else
#				pragma comment( lib, "CLicense.lib")
#			endif
#		endif
#	else
#		ifdef _KIMI_LICENSE_STATIC_LINK
#			pragma comment( lib, "CLicenseS.lib")
#		else
#			ifdef _DEBUG
#				pragma comment( lib, "CLicenseD.lib")
#			else
#				pragma comment( lib, "CLicense.lib")
#			endif
#		endif
#	endif
#endif

#endif // end of __KIMI_LICENSE_API_H__