/*

  myopc.h

  */

#ifndef _MYOPC_H_
#define _MYOPC_H_
//-------------------------------------------------------------------------
#define DEFBUFFSIZE					_MAX_PATH		// default buffer size

void MytoWchar(WCHAR* lpwcd,LPCTSTR lpszTxt);
//-------------------------------------------------------------------------

#include "safearray.h"
#include "timestmp.h"
#include "shutdownsink.h"
#include "advisesink.h"
#include "datasink20.h"

// ���ר���ڹ���OPC���Ӻ�ᴫ��һ������Ϊ0x1C����Чֵ��BUG
//#define _DABA_OUT_OF_SERVICE_FILTER	1
// ����ʱ��OPC�������Ӻ󴫵�һ������Ϊ0x00����Чֵ

// ����ר�ã��������Ϻ�����HCPS OPC Server����pdrv->m_OpcGroup.RefreshAsync20()�ᱻ����������߳��޷�ֹͣ�����Բ�������øú���
//#define _YUEYANG_HCPS_OPCSERVER

// clipboard formats for data access 1.0 streams
extern UINT CF_DATA_CHANGE;
extern UINT CF_DATA_CHANGE_TIME;
extern UINT CF_WRITE_COMPLETE;

// OPC_XX_DATAXX defines are based on an index in a listbox (do not modify) 
#define OPC_10_DATACHANGE			0	// OnDataChange 1.0 no timestamp request 
#define OPC_10_DATATIMECHANGE		1	// OnDataChange 1.0 with timestamp request 
#define OPC_20_DATACHANGE			2	// OnDataChange 2.0 (always a timestamp) request 

// group defaults
#define GROUP_DEFAULT_ACTIVESTATE	TRUE
#define GROUP_DEFAULT_LANGUAGEID	MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US)
#define GROUP_DEFAULT_NAME			_T("")
#define GROUP_DEFAULT_DEADBAND		0.0f
#define GROUP_DEFAULT_TIMEBIAS		0
#define GROUP_DEFAULT_UPDATERATE	100
#define GROUP_DEFAULT_UPDATEMETHOD	OPC_20_DATACHANGE
#define GROUP_DEFAULT_FORCEDELETE	FALSE
#define GROUP_DEFAULT_REMOVEITEMSONDELETE	TRUE
#define GROUP_DEFAULT_ACTIVATEITEMSONSELECT	TRUE	

// item defaults
#define ITEM_DEFAULT_ACCESSPATH		_T("")
#define ITEM_DEFAULT_ITEMID			_T("")
#define ITEM_DEFAULT_ACTIVESTATE	TRUE
#define ITEM_DEFAULT_DATATYPE		VT_EMPTY

#define OPC_LANGUAGEID_ENGLISH		GROUP_DEFAULT_LANGUAGEID								// Ӣ��
#define OPC_LANGUAGEID_CHINESE		MAKELANGID (LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)   // ��������

// So we can generically handle TCHAR arrays
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

class CKServer;
class CKGroup;

// **************************************************************************
class CKItem : public CObject  
	{
	public:
		// construction/destruction
		CKItem ();
		~CKItem ();

#ifdef _DABA_OUT_OF_SERVICE_FILTER
		BOOL m_bOutOfServiceFilter;
#endif
	public:
		// property accessor/manipulators
		void SetGroup(CKGroup *pGroup){m_pGroup=pGroup;};
		CKGroup *GetGroup(){return m_pGroup;};
		void SetAccessPath (LPCTSTR strAccessPath) {m_strAccessPath = strAccessPath;}
		LPCTSTR GetAccessPath () {return (m_strAccessPath);}

		void SetActive (BOOL bActive) {m_bActive = bActive;}
		BOOL IsActive () {return (m_bActive);}

		void SetDataType (VARTYPE vtType) {m_vtDataType = vtType;}
		VARTYPE GetDataType () 
			{
			if (m_vtValue.vt != VT_EMPTY)
				return (m_vtValue.vt);

			return (m_vtDataType);
			}

		void SetItemID (LPCTSTR strItemID) {m_strItemID = strItemID;}
		LPCTSTR GetItemID () {return (m_strItemID);}

		void SetServerHandle (OPCHANDLE hServer) {m_hServer = hServer;}
		OPCHANDLE GetServerHandle () {return (m_hServer);}

		void SetAccessRights (DWORD dwAccess) {m_dwAccessRights = dwAccess;}
		DWORD GetAccessRights () {return (m_dwAccessRights);}

		void SetValid (BOOL bValid) 
			{
			m_bValid = bValid;

			if (!bValid)
				{
				m_wQuality = OPC_QUALITY_BAD_OUT_OF_SERVICE;
				m_bTimeStamped = FALSE;

				VariantInit (&m_vtValue);
				}
			}
		WORD	GetQualityVal(){return m_wQuality;};
		BOOL IsValid () {return (m_bValid);}

		// data
		void UpdateData (VARIANT &vtVal, WORD wQuality);
		void UpdateData (VARIANT &vtVal, WORD wQuality, FILETIME &ftTimeStamp);

		void GetValue (CString &strValue);

		VARIANT* GetValue () {return (&m_vtValue);}

		void SetParam(void* pParam){m_pParam=pParam;};
		void SetParam2(void* pParam){m_pParam2=pParam;};
	protected:
		short ToDBPQuality(WORD wQuality);

	private:

		// properties
		CString m_strAccessPath;	// access path
		CString m_strItemID;		// fully qualified item ID
		BOOL m_bActive;				// active state
		VARTYPE m_vtDataType;		// server's canonical datatype
		DWORD m_dwAccessRights;		// access rights
		OPCHANDLE m_hServer;		// server handle for this item

		typedef struct _flags
			{
			DWORD Reserved					: 32;
			} FLAGS;

		FLAGS m_bfFlags;

		BOOL m_bValid;				// TRUE if successfully added to an OPC server
		BOOL m_bTimeStamped;		// TRUE if the last update included a timestamp

		// data
		FILETIME m_ftTimeStamp;		// timestamp attached to value
		WORD m_wQuality;			// quality attached to value
		VARIANT m_vtValue;			// current value

		// parent server
		CKGroup *m_pGroup;

		void *m_pParam;//Ӧ�ò�����д��CItemDef*
		void *m_pParam2;// COpcDrv*
	};


// **************************************************************************
class CKGroup : public CObject  
	{
	public:
		// construction/destruction
		CKGroup ();
		~CKGroup ();
		void *m_pDrv;//����
	public:
		// property accessor/manipulators
		void SetName (CString &strName) {m_strName = strName;}
		void SetName (LPCTSTR lpszName) {m_strName = lpszName;}
		LPCTSTR GetName () {return (m_strName);}
		void SetServer (CKServer *pServer){m_pServer = pServer;};
		CKServer *GetServer(){return m_pServer;};
		void SetUpdateRate (DWORD dwRate) {m_dwUpdateRate = dwRate;}
		DWORD GetUpdateRate () {return (m_dwUpdateRate);}

		void SetLanguageID (LCID lcid) {m_dwLanguageID = lcid;}
		LCID GetLanguageID () {return (m_dwLanguageID);}

		BOOL IsActive () {return (m_bActive);}

		void SetBias (long lBias) {m_lBias = lBias;}
		long GetBias () {return (m_lBias);}

		void SetDeadband (float fDeadband) {m_fDeadband = fDeadband;}
		float GetDeadband () {return (m_fDeadband);}

		void SetServerHandle (OPCHANDLE hServer) {m_hServer = hServer;}
		OPCHANDLE GetServerHandle () {return (m_hServer);}

		void SetValid (BOOL bValid) {m_bValid = bValid;}
		BOOL IsValid () {return (m_bValid);}

		void SetUpdateMethod (DWORD dwMethod) {m_dwUpdateMethod = dwMethod;}
		DWORD GetUpdateMethod () {return (m_dwUpdateMethod);}

		// flag accessor/manipulators
		void ForceDeletion (BOOL bSet) {m_bfFlags.bOnDeleteForceDeletion = bSet;}
		BOOL IsForceDeletion () {return (m_bfFlags.bOnDeleteForceDeletion);}

		// OPC Specifics
		void Initialize (LPUNKNOWN pUnk);
		void Uninitialize ();

		BOOL ReadSync (CObArray &cItemList, DWORD cdwItems, bool bDeviceRead, bool bPostMsg = true);//ͬ����
		void WriteOneSync (OPCHANDLE hi, LPCTSTR lpszVal,int opcdadatype);// ͬ��д
		BOOL RefreshAsync10 (bool bDeviceRead);// �첽1.0
		BOOL RefreshAsync20 (bool bDeviceRead);// �첽2.0

		//{{ �ӿ�֧��
		
		bool IsIGroupStateMgtSupported () {return (m_pIGroupState != NULL);}
		bool IsIPublicGroupStateMgtSupported () {return (m_pIPublicGroupState != NULL);}
		bool IsIItemMgtSupported () {return (m_pIItemMgt != NULL);}
		bool IsISyncIOSupported () {return (m_pISync != NULL);}
		bool IsIAsyncIOSupported () {return (m_pIAsync != NULL);}
		bool IsIDataObjectSupported () {return (m_pIDataObject != NULL);}
		bool IsIAsyncIO2Supported () {return (m_pIAsync2 != NULL);}
		bool IsIConnectionPointContainerSupported () {return (m_pIConnPtContainer != NULL);}

		IOPCItemMgt* GetIItemMgt () {return (m_pIItemMgt);}
		IOPCGroupStateMgt* GetIGroupStateMgt () {return (m_pIGroupState);}
		IOPCSyncIO* GetISyncIO () {return (m_pISync);}

		//}}

		//{{ ���������
		BOOL AddItems (CObArray &cItemList, DWORD dwCount, CString &szErr);
		BOOL RemoveItems (CObArray &cItemList, DWORD dwCount, CString &szErr);
		//}}
	private:
		bool MapStringValToVariant (CString &strValue, VARIANT &vtVal, VARTYPE vtType);
		
		typedef enum _tagGETARRELEMRET
			{
			tElement = 0,
			tEndRow,
			tInvalid,
			tOverflow,
			tDone
			} GETARRELEMRET;

		GETARRELEMRET GetArrayElement (LPCTSTR szInBuff, int *pnStart, LPTSTR szOutBuff, int nBuffSize);
		bool MapStringValToArrayVariant (CString &strValue, VARIANT *pvtSrc, VARIANT *pvtDst);

		
	private:
		// {{properties ��������
		CString m_strName;		// group name					����
		
		DWORD m_dwUpdateRate;	// update rate in milliseconds	���ݸ������� MS
		LCID m_dwLanguageID;	// language ID					����

		BOOL m_bActive;			// active state					�״̬
		long m_lBias;			// time bias in minutes
		float m_fDeadband;		// percent deadband

		OPCHANDLE m_hServer;	// server handle for this group	�����OPC Server
		BOOL m_bValid;			// TRUE if successfully added to the OPC server		�Ƿ����Ѽ��������
		DWORD m_dwUpdateMethod;	// update method used by this group (see globals.h)

		typedef struct _flags
			{
			DWORD bOnDeleteForceDeletion	: 1;	// TRUE if the server should force deletion of group even if references exists
//			DWORD bOnDeleteRemoveItems		: 1;	// TRUE if the client should remove items before remove group
			DWORD Reserved					: 31;
			} FLAGS;

		FLAGS m_bfFlags; 
		//}}

		// OPC specifics  ���׼�ӿ�
		IOPCGroupStateMgt *m_pIGroupState;				//��״̬��������Ϣ����
		IOPCPublicGroupStateMgt *m_pIPublicGroupState;	//��ѡ�ӿڣ���˽����ת��Ϊ������
		IOPCItemMgt *m_pIItemMgt;						//item����ӿ�
		IOPCSyncIO *m_pISync;							//ͬ�����ݷ���
		IOPCAsyncIO *m_pIAsync;							//�ɰ汾�ģ��첽���ݷ��ʣ��书���ѱ�IOPCAsyncIO2��IConnectionPointContainerȡ��
		IDataObject *m_pIDataObject;					//�ɰ汾��
		IOPCAsyncIO2 *m_pIAsync2;						//�°汾�첽��
		IConnectionPointContainer *m_pIConnPtContainer;	//�°汾�����

		IKDataSink20 *m_pIDataSink20;
		DWORD m_dwCookieDataSink20;

		CKAdviseSink *m_pIAdviseSink;
		DWORD m_dwCookieRead;
		DWORD m_dwCookieWrite;

		CKServer *m_pServer;

	};

// **************************************************************************
/*
��������ṩ��CString m_strProgID;			
		CString m_strRemoteMachine;
		����OPC Server���󣬻�ȡ��׼�ӿ�:
  */
class CKServer : public CObject  
	{
	public:
		// construction/destruction
		CKServer ();
		CKServer (CString &strProgID, CString &strRemoteMachine);

		~CKServer ();

	public:
		void *m_pdrv;
		// property accessor/manipulators
		void SetProgID (CString &strProgID);
		LPCTSTR GetProgID () {return (m_strProgID);}

		void SetRemoteMachine (CString &strRemoteMachine);
		LPCTSTR GetRemoteMachine () 
			{
			if (m_strRemoteMachine.IsEmpty ())
				return (NULL);

			return (m_strRemoteMachine);
			}

		bool IsConnected () {
			CSafeLock lock(&m_csLock);
			return (m_bConnected);
		}
		bool IsKepServerEx () {return (m_bfFlags.bIsKepServerEx);}
		bool IsAlive ();

		BOOL AddGroup (CKGroup *pGroup,CString &szErr);
		void RemoveGroup (CKGroup *pGroup);

		// OPC specifics
		bool Connect ();				//����OPC������
		bool Connect (CString &strProgID, CString &strRemoteMachine);
		void Disconnect ();

		void ShutdownRequest (LPCTSTR lpszReason);
		

		void Start ();
		void Stop ();


		// supported interfaces		
														//�����ֽӿڵ�֧�����
		bool IsIServerSupported () {return (m_pIServer != NULL);}
		bool IsICommonSupported () {return (m_pICommon != NULL);}
		bool IsIConnectionPointContainerSupported () {return (m_pIConnPtContainer != NULL);}
		bool IsIItemPropertiesSupported () {return (m_pIItemProps != NULL);}
		bool IsIBrowsingSupported () {return (m_pIBrowse != NULL);}
		bool IsIServerPublicGroupsSupported () {return (m_pIPublicGroups != NULL);}
		bool IsIPersistFileSupported () {return (m_pIPersistFile != NULL);}

		IOPCServer* GetIServer () {return (m_pIServer);}
		IOPCBrowseServerAddressSpace* GetIBrowse () {return (IsAlive () ? m_pIBrowse : NULL);}
		IOPCItemProperties* GetIItemProps () {return (m_pIItemProps);}
	private:
		// OPC specifics
		HRESULT GetCLSID (CLSID &clsid);

	private:
		CCriticalSection m_csLock;		// ����״̬���ʰ�ȫ��

		// properties
		CString m_strProgID;							//����ID		
		CString m_strRemoteMachine;						//���������IP

		typedef struct _flags
			{
			DWORD bIsKepServerEx			: 1;
			DWORD Reserved					: 31;
			} FLAGS;

		FLAGS m_bfFlags; // �����Ҫ

		
		//OPCÿ���ӿڵĽӿ�ָ�룬IDָ�룬״̬������
		MULTI_QI m_arrMultiQI [7];	// array of interfaces we will query for each server
		
		IOPCServer *m_pIServer;		// interface pointers returned from CoCreateInstanceEx ()�����ӿ�
		IOPCCommon *m_pICommon;							//�����ӿڣ�������OPC�������¼�������
		IConnectionPointContainer *m_pIConnPtContainer;	//���ӵ�ӿ�
		IOPCItemProperties *m_pIItemProps;				//item���Խӿڣ��޷���������
		IOPCBrowseServerAddressSpace *m_pIBrowse;		//��ѡ�ģ����������������������Ա������
		IOPCServerPublicGroups *m_pIPublicGroups;		//��ѡ�ģ����ڹ�������
		IPersistFile *m_pIPersistFile;					//��ѡ�ģ����ڱ�������

		IKShutdownSink *m_pIShutdownSink;
		DWORD m_dwCookieShutdownSink;

		bool m_bConnected;			// our connect state//�Զ�������״̬

	};


#endif // _MYOPC_H_
