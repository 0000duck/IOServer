#include "stdafx.h"
#include <vector>
#include <string>
#include <iphlpapi.h>
#include <Wbemcli.h>
#include <Wbemidl.h>
#include "KimiCore.h"
#include "KimiCore/License/HardwareSerialNumber.h"

#pragma warning( push )
#pragma warning(disable : 4312)

#pragma comment( lib, "Netapi32.lib" )
#pragma comment( lib, "Iphlpapi.lib" )
#pragma comment( lib, "Wbemuuid.lib" )

_KIMI_NAMESPACE_BEGIN

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ( ( FILE_DEVICE_SCSI << 16 ) + 0x0501 )

#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition

#define  IDENTIFY_BUFFER_SIZE  512
#define  SENDIDLENGTH  ( sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE )

#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef struct _DRIVERSTATUS

{
	BYTE  bDriverError;  //  Error code from driver, or 0 if no error.
	BYTE  bIDEStatus;    //  Contents of IDE Error register.
	//  Only valid when bDriverError is SMART_IDE_ERROR.
	BYTE  bReserved[2];  //  Reserved for future expansion.
	DWORD  dwReserved[2];  //  Reserved for future expansion.
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

typedef struct _SENDCMDOUTPARAMS
{
	DWORD         cBufferSize;   //  Size of bBuffer in bytes
	DRIVERSTATUS  DriverStatus;  //  Driver status structure.
	BYTE          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

typedef struct _IDEREGS
{
	BYTE bFeaturesReg;       // Used for specifying SMART "commands".
	BYTE bSectorCountReg;    // IDE sector count register
	BYTE bSectorNumberReg;   // IDE sector number register
	BYTE bCylLowReg;         // IDE low order cylinder value
	BYTE bCylHighReg;        // IDE high order cylinder value
	BYTE bDriveHeadReg;      // IDE drive/head register
	BYTE bCommandReg;        // Actual IDE command.
	BYTE bReserved;          // reserved for future use.  Must be zero.
} IDEREGS, *PIDEREGS, *LPIDEREGS;

typedef struct _SENDCMDINPARAMS
{
	DWORD     cBufferSize;   //  Buffer size in bytes
	IDEREGS   irDriveRegs;   //  Structure with drive register values.
	BYTE bDriveNumber;       //  Physical drive number to send 
	//  command to (0,1,2,3).
	BYTE bReserved[3];       //  Reserved for future expansion.
	DWORD     dwReserved[4]; //  For future use.
	BYTE      bBuffer[1];    //  Input buffer.
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

//////////////////////////////////////////////////////////////////////

//�ṹ���� 
typedef struct _UNICODE_STRING 
{ 
	USHORT  Length;//���� 
	USHORT  MaximumLength;//��󳤶� 
	PWSTR  Buffer;//����ָ�� 
} UNICODE_STRING,*PUNICODE_STRING; 

typedef struct _OBJECT_ATTRIBUTES 
{ 
	ULONG Length;//���� 18h 
	HANDLE RootDirectory;//  00000000 
	PUNICODE_STRING ObjectName;//ָ���������ָ�� 
	ULONG Attributes;//��������00000040h 
	PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR��0 
	PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE��0 
} OBJECT_ATTRIBUTES; 
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES; 

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER	NameBuff[30];

} ASTAT, * PASTAT;

//ASTAT Adapter;

//����ָ���������
typedef DWORD  (__stdcall *ZWOS )( PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES); 
typedef DWORD  (__stdcall *ZWMV )( HANDLE,HANDLE,PVOID,ULONG,ULONG,PLARGE_INTEGER,PSIZE_T,DWORD,ULONG,ULONG); 
typedef DWORD  (__stdcall *ZWUMV )( HANDLE,PVOID); 

/*
BOOL WinNTHDSerialNumAsScsiRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen );
BOOL DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				PDWORD lpcbBytesReturned );
BOOL WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen );
int getmac_one (int lana_num,char *buf);
UINT FindAwardBios( BYTE** ppBiosAddr );
UINT FindAmiBios( BYTE** ppBiosAddr );
UINT FindPhoenixBios( BYTE** ppBiosAddr );
*/

BOOL WinNTHDSerialNumAsScsiRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
	BOOL bInfoLoaded = FALSE;

	for( int iController = 0; iController < 2; ++ iController )
	{
		HANDLE hScsiDriveIOCTL = 0;
		char   szDriveName[256];

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		sprintf_s( szDriveName, 256, "\\\\.\\Scsi%d:", iController );

		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFileA( szDriveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		// if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
		//            controller, GetLastError ());

		if( hScsiDriveIOCTL != INVALID_HANDLE_VALUE )
		{
			int iDrive = 0;
			for( iDrive = 0; iDrive < 2; ++ iDrive )
			{
				char szBuffer[sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH] = { 0 };

				SRB_IO_CONTROL* p = ( SRB_IO_CONTROL* )szBuffer;
				SENDCMDINPARAMS* pin = ( SENDCMDINPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
				DWORD dwResult;

				p->HeaderLength = sizeof( SRB_IO_CONTROL );
				p->Timeout = 10000;
				p->Length = SENDIDLENGTH;
				p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;

				// ���ʹ��strncpy_s()���п����ַ���ʱ����64λ��Win2008ϵͳ��ϵͳ�����޷�����ִ��
				//strncpy_s( ( char* )p->Signature, 8, "SCSIDISK", 8 );
				memcpy(p->Signature, "SCSIDISK", 8);

				pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin->bDriveNumber = iDrive;

				if( DeviceIoControl( hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
					szBuffer,
					sizeof( SRB_IO_CONTROL ) + sizeof( SENDCMDINPARAMS ) - 1,
					szBuffer,
					sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH,
					&dwResult, NULL ) )
				{
					SENDCMDOUTPARAMS* pOut = ( SENDCMDOUTPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
					IDSECTOR* pId = ( IDSECTOR* )( pOut->bBuffer );
					if( pId->sModelNumber[0] )
					{
						if( * puSerialLen + 20U <= uMaxSerialLen )
						{
							// ���к�
							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 10, 20 );

							UINT i;
							// Cut off the trailing blanks
							for( i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )
							{}
							* puSerialLen += i;

							// �ͺ�
							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 27, 40 );
							// Cut off the trailing blanks
							for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )
							{}
							* puSerialLen += i;

							bInfoLoaded = TRUE;
						}
						else
						{
							::CloseHandle( hScsiDriveIOCTL );
							return bInfoLoaded;
						}
					}
				}
			}
			::CloseHandle( hScsiDriveIOCTL );
		}
	}
	return bInfoLoaded;
}


BOOL DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				PDWORD lpcbBytesReturned )
{
	// Set up data structures for IDENTIFY command.
	pSCIP->cBufferSize                  = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg     = 0;
	pSCIP->irDriveRegs.bSectorCountReg  = 1;
	pSCIP->irDriveRegs.bSectorNumberReg = 1;
	pSCIP->irDriveRegs.bCylLowReg       = 0;
	pSCIP->irDriveRegs.bCylHighReg      = 0;

	// calc the drive number.
	pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ( ( bDriveNum & 1 ) << 4 );

	// The command can either be IDE identify or ATAPI identify.
	pSCIP->irDriveRegs.bCommandReg = bIDCmd;
	pSCIP->bDriveNumber = bDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return DeviceIoControl( hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		( LPVOID ) pSCIP,
		sizeof( SENDCMDINPARAMS ) - 1,
		( LPVOID ) pSCOP,
		sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL );
}

BOOL WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
#define  DFP_GET_VERSION          0x00074080
	BOOL bInfoLoaded = FALSE;

	for( UINT uDrive = 0; uDrive < 4; ++ uDrive )
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char szDriveName [256];
		sprintf_s( szDriveName, 256, "\\\\.\\PhysicalDrive%d", uDrive );

		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFileA( szDriveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if( hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE )
		{
			GETVERSIONOUTPARAMS VersionParams = { 0 };
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			if( DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 
				0,
				&VersionParams,
				sizeof( GETVERSIONOUTPARAMS ),
				&cbBytesReturned, NULL ) )
			{
				// If there is a IDE device at number "i" issue commands
				// to the device
				if( VersionParams.bIDEDeviceMap != 0 )
				{
					BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
					SENDCMDINPARAMS  scip = { 0 };

					// Now, get the ID sector for all IDE devices in the system.
					// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
					// otherwise use the IDE_ATA_IDENTIFY command
					bIDCmd = ( VersionParams.bIDEDeviceMap >> uDrive & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
					BYTE IdOutCmd[sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1] = { 0 };

					if( DoIdentify( hPhysicalDriveIOCTL, 
						&scip, 
						( PSENDCMDOUTPARAMS )&IdOutCmd, 
						( BYTE )bIDCmd,
						( BYTE )uDrive,
						&cbBytesReturned ) )
					{
						if( * puSerialLen + 20U <= uMaxSerialLen )
						{
							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 10, 20 );  // ���к�

							UINT i;
							// Cut off the trailing blanks
							for( i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )  {}
							* puSerialLen += i;

							CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 27, 40 ); // �ͺ�

							// Cut off the trailing blanks
							for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )  {}
							* puSerialLen += i;

							bInfoLoaded = TRUE;
						}
						else
						{
							::CloseHandle( hPhysicalDriveIOCTL );
							return bInfoLoaded;
						}
					}
				}
			}
			CloseHandle( hPhysicalDriveIOCTL );
		}
	}
	return bInfoLoaded;
}

UINT FindAwardBios( BYTE** ppBiosAddr )
{
	BYTE* pBiosAddr = * ppBiosAddr + 0xEC71;

	BYTE szBiosData[128];
	CopyMemory( szBiosData, pBiosAddr, 127 );
	szBiosData[127] = 0;

	size_t iLen = strlen( ( char* )szBiosData );
	if( iLen > 0 && iLen < 128 )
	{
		//AWard:         07/08/2002-i845G-ITE8712-JF69VD0CC-00 
		//Phoenix-Award: 03/12/2002-sis645-p4s333
		if( szBiosData[2] == '/' && szBiosData[5] == '/' )
		{
			BYTE* p = szBiosData;
			while( * p )
			{
				if( * p < ' ' || * p >= 127 )
				{
					break;
				}
				++ p;
			}
			if( * p == 0 )
			{
				* ppBiosAddr = pBiosAddr;
				return ( UINT )iLen;
			}
		}
	}
	return 0;
}

UINT FindAmiBios( BYTE** ppBiosAddr )
{
	BYTE* pBiosAddr = * ppBiosAddr + 0xF478;

	BYTE szBiosData[128];
	CopyMemory( szBiosData, pBiosAddr, 127 );
	szBiosData[127] = 0;

	size_t iLen = strlen( ( char* )szBiosData );
	if( iLen > 0 && iLen < 128 )
	{
		// Example: "AMI: 51-2300-000000-00101111-030199-"
		if( szBiosData[2] == '-' && szBiosData[7] == '-' )
		{
			BYTE* p = szBiosData;
			while( * p )
			{
				if( * p < ' ' || * p >= 127 )
				{
					break;
				}
				++ p;
			}
			if( * p == 0 )
			{
				* ppBiosAddr = pBiosAddr;
				return ( UINT )iLen;
			}
		}
	}
	return 0;
}

UINT FindPhoenixBios( BYTE** ppBiosAddr )
{
	UINT uOffset[3] = { 0x6577, 0x7196, 0x7550 };
	for( UINT i = 0; i < 3; ++ i )
	{
		BYTE* pBiosAddr = * ppBiosAddr + uOffset[i];

		BYTE szBiosData[128];
		CopyMemory( szBiosData, pBiosAddr, 127 );
		szBiosData[127] = 0;

		size_t iLen = strlen( ( char* )szBiosData );
		if( iLen > 0 && iLen < 128 )
		{
			// Example: Phoenix "NITELT0.86B.0044.P11.9910111055"
			if( szBiosData[7] == '.' && szBiosData[11] == '.' )
			{
				BYTE* p = szBiosData;
				while( * p )
				{
					if( * p < ' ' || * p >= 127 )
					{
						break;
					}
					++ p;
				}
				if( * p == 0 )
				{
					* ppBiosAddr = pBiosAddr;
					return ( UINT )iLen;
				}
			}
		}
	}
	return 0;
}

// ȡӲ�̵�C�߼������кţ���API��ȡ���Ĳ���������Ӳ�����кţ�ÿ�θ�ʽ��Ӳ�̸����кŶ���仯
KIMI_CORE_API(string) GetCLogicDiskSerialNumber()
{
	DWORD VolumeSerialNumber = 0;
	
	if (!::GetVolumeInformationA("c:\\", NULL,12, &VolumeSerialNumber, 
		NULL, NULL, NULL, 10))
	{
		return _DEFAULT_HARD_DISK_SERIAL_NUMBER;
	}

	VolumeSerialNumber ^= 0xF8C304A1;

	char buf[32];
	sprintf_s(buf, 32, "%X", VolumeSerialNumber);

	return string(buf);
}

//////////////////////////////////////////////////////////////////////////
//
KIMI_CORE_API(string) GetHardDiskSerialNumber()
{
	BYTE szSystemInfo[4096]; 
	UINT uSystemInfoLen = 0;

	OSVERSIONINFO ovi = { 0 };
	ovi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &ovi );

	if( ovi.dwPlatformId != VER_PLATFORM_WIN32_NT )
	{
		// Only Windows 2000, Windows XP, Windows Server 2003...
		return _DEFAULT_HARD_DISK_SERIAL_NUMBER;
	}
	else
	{
		if( !WinNTHDSerialNumAsPhysicalRead( szSystemInfo, &uSystemInfoLen, 1024 ) )
		{
			if (!WinNTHDSerialNumAsScsiRead( szSystemInfo, &uSystemInfoLen, 1024 ))
			{
				return _DEFAULT_HARD_DISK_SERIAL_NUMBER;
			}
		}
	}

	string sn;

	for (UINT i = 0; i < uSystemInfoLen; i++)
	{
		if (szSystemInfo[i] != ' ')
		{
			sn += (char)szSystemInfo[i];
		}
	}

	return sn;
}

//ȡ������ MAC ��ַ��ע��: MAC ��ַ�ǿ�����ע������޸ĵģ�
void GetMACAddressInfo(vector<string> &arrMacAddressInfo)
{
	arrMacAddressInfo.clear();
	
	//////////////////////////////////////////////////////
	PIP_ADAPTER_ADDRESSES pAddresses;
	pAddresses = (IP_ADAPTER_ADDRESSES*)GlobalAlloc(GMEM_ZEROINIT,sizeof(IP_ADAPTER_ADDRESSES));
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;       
	
	//Make an initial call to GetAdapters Addresses to get the     
	//size needed into the outBufLen variable     
	
	if (GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		GlobalFree(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)GlobalAlloc(GMEM_ZEROINIT, outBufLen);
	}     
	
	//Make a second call to GetAdapters Addresses to get the     
	//actual data we want     
	
	if ((dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen)) == NO_ERROR)
	{
		//if successful,output some information from the data we received
		while(pAddresses) //��ȡ����������ַ
		{
// 			if ((_tcsstr(pAddresses->Description, _T("PCI")) > 0) || //pAddresses->Description�а���"PCI"Ϊ����������
// 				(pAddresses->IfType == IF_TYPE_ETHERNET_CSMACD) // //pAddresses->IfType��71Ϊ����������
// 				)
			if ( /*(wcsstr(pAddresses->Description, L"VMware") == NULL) &&*/ // ����VMware��ģ�����������ǲ�Ҫ����VMware�������������������������
				(pAddresses->IfType == IF_TYPE_ETHERNET_CSMACD) // ����̫����
				)
			{
				string macAddr;    //������ַ
				char strTmp[32];     //��ʱ����
			
				for (DWORD i = 0; i < pAddresses->PhysicalAddressLength; i++)
				{
					sprintf_s(strTmp, 32, "%02x", pAddresses->PhysicalAddress[i]);
					macAddr += strTmp;
					
					if (i < pAddresses->PhysicalAddressLength - 1)
						macAddr += "-";
				}
				
				//����������
				if (macAddr.size() > 0)
					arrMacAddressInfo.push_back(macAddr);
			}
			
			//��һ��
			pAddresses = pAddresses->Next;
		}
	}
	GlobalFree(pAddresses);
}

//ȡ��bios�������Ϣ,�п���ȡ����
int GetBIOSInfo(char *szBIOSInfo)
{
	int uSystemInfoLen = 0;
	SIZE_T ssize; 

	LARGE_INTEGER so; 
	so.LowPart=0x000f0000;
	so.HighPart=0x00000000; 
	ssize=0xffff; 
	wchar_t strPH[30]=L"\\device\\physicalmemory"; 

	DWORD ba=0;

	UNICODE_STRING struniph; 
	struniph.Buffer=(wchar_t *)strPH; 
	struniph.Length=0x2c; 
	struniph.MaximumLength =0x2e; 

	OBJECT_ATTRIBUTES obj_ar; 
	obj_ar.Attributes =64;
	obj_ar.Length =24;
	obj_ar.ObjectName=&struniph;
	obj_ar.RootDirectory=0; 
	obj_ar.SecurityDescriptor=0; 
	obj_ar.SecurityQualityOfService =0; 

	HMODULE hinstLib = LoadLibraryA("ntdll.dll"); 
	ZWOS ZWopenS=(ZWOS)GetProcAddress(hinstLib,"ZwOpenSection"); 
	ZWMV ZWmapV=(ZWMV)GetProcAddress(hinstLib,"ZwMapViewOfSection"); 
	ZWUMV ZWunmapV=(ZWUMV)GetProcAddress(hinstLib,"ZwUnmapViewOfSection"); 

	//���ú������������ڴ����ӳ�� 
	HANDLE hSection; 
	if( 0 == ZWopenS(&hSection,4,&obj_ar))
		if ( 
			0 == ZWmapV( 
			( HANDLE )hSection,   //��Sectionʱ�õ��ľ�� 
			( HANDLE )0xFFFFFFFF, //��Ҫӳ����̵ľ���� 
			&ba,                  //ӳ��Ļ�ַ 
			0,
			0xFFFF,               //����Ĵ�С 
			&so,                  //�����ڴ�ĵ�ַ 
			&ssize,               //ָ���ȡ�ڴ���С��ָ�� 
			1,                    //�ӽ��̵Ŀɼ̳����趨 
			0,                    //�������� 
			2                     //�������� 
			) )
			//ִ�к���ڵ�ǰ���̵Ŀռ俪��һ��64k�Ŀռ䣬����f000:0000��f000:ffff��������ӳ�䵽���� 
			//ӳ��Ļ�ַ��ba����,���ӳ�䲻������,Ӧ����ZwUnmapViewOfSection�Ͽ�ӳ�� 
		{		
			BYTE* pBiosSerial = ( BYTE* )ba;
			UINT uBiosSerialLen = FindAwardBios( &pBiosSerial );
			if( uBiosSerialLen == 0U )
			{
				uBiosSerialLen = FindAmiBios( &pBiosSerial );
				if( uBiosSerialLen == 0U )
				{
					uBiosSerialLen = FindPhoenixBios( &pBiosSerial );
				}
			}
			if( uBiosSerialLen != 0U )
			{
				CopyMemory( szBIOSInfo + uSystemInfoLen, pBiosSerial, uBiosSerialLen );
				uSystemInfoLen += uBiosSerialLen;
			}
			ZWunmapV( ( HANDLE )0xFFFFFFFF, (void*)ba );
		}
		szBIOSInfo[uSystemInfoLen] = '\0';
		return uSystemInfoLen;
}

string GetWmiInfo(IWbemClassObject *pClassObject, const char *lpszName) 
{
	CComVariant varValue ; 
	CComBSTR bstrName(lpszName); 
	string str;

	if( pClassObject->Get( bstrName , 0 , &varValue , NULL , 0 ) == S_OK ) 
	{
		// �Բ�ͬ���������ͷֱ��� 
		if( varValue.vt == VT_BSTR )	//�ַ��� 
		{
			char *s = _com_util::ConvertBSTRToString(varValue.bstrVal);
			str = s;
			delete[] s;
		} 
		else  
			if( varValue.vt == VT_ARRAY ) //���� 
			{ 
				long iLowBound = 0 , iUpBound = 0 ; 
				SafeArrayGetLBound( varValue.parray , 1 , &iLowBound ) ; 
				SafeArrayGetUBound( varValue.parray , 1 , &iUpBound ) ; 
				for( long j = iLowBound ; j <= iUpBound ; j ++ ) 
				{ 
					VARIANT *pvar = NULL ; 
					long temp = j ; 
					if( SafeArrayGetElement( varValue.parray , &temp , pvar ) == S_OK && pvar ) 
					{ 
						CComVariant varTemp ; 
						if( varTemp.ChangeType( VT_BSTR , pvar ) == S_OK ) 
						{ 
							if( !str.empty() ) 
								str += "," ; 

							char *s = _com_util::ConvertBSTRToString(varTemp.bstrVal);
							str += s;
							delete[] s;
						} //if( varTemp... 
					} //if( SafeArrayGet... 
				} //for( long j=iLowBound;... 
			} //if (varValue.vt ... 
			else 
			{ 
				char buf[128];
				switch( varValue.vt) 
				{ 
				case VT_I4:
					sprintf_s(buf, 128, "%d", varValue.lVal); 
					break; 
				case VT_I8: 
					sprintf_s(buf, 128, "%lld", varValue.llVal);
					break; 
				case VT_I2: 
					sprintf_s(buf, 128, "%d", varValue.iVal);
					break; 
				case VT_UI1: 
					sprintf_s(buf, 128, "%d", (int)varValue.bVal);
					break; 
				case VT_R4: 
					sprintf_s(buf, 128, "%f", (double)varValue.fltVal);
					break; 
				case VT_R8: 
					sprintf_s(buf, 128, "%f", varValue.dblVal);
					break; 
				case VT_I1: 
					sprintf_s(buf, 128, "%d", (int)varValue.cVal);
					break; 
				case VT_UI2: 
					sprintf_s(buf, 128, "%u", varValue.uiVal);
					break; 
				case VT_UI4: 
					sprintf_s(buf, 128, "%u", varValue.ulVal);
					break; 
				case VT_UI8: 
					sprintf_s(buf, 128, "%llu", varValue.ulVal);
					break; 
				case VT_BOOL:
					sprintf_s(buf, 128, "%d", (varValue.boolVal == VARIANT_TRUE));
					break; 
				case VT_NULL: 
					buf[0] = '\0';
					break; 
				default: 
					if( varValue.ChangeType( VT_BSTR ) == S_OK )
					{
						char *s = _com_util::ConvertBSTRToString(varValue.bstrVal);
						str += s;
						delete[] s;
					}
					else 
						buf[0] = '\0';
				}
				str = buf;
			}
	} 
	else 
		str = ""; 

	return str; 
} 


string GetSeiralNumberOrID( const char *lpszClass, const char *lpszName) 
{
	CoInitializeHelper IniCOM;

	string str;

	/*
	// ��ȡ���� WMI Ȩ��
	static BOOL g_bInitializeSecurity = FALSE;

	if (!g_bInitializeSecurity)
	{
		// CoInitializeSecurity��ÿ��������ֻ��������һ��
		if( ::CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE,NULL, EOAC_NONE, 0 ) != S_OK )
		{
			return str;
		}

		g_bInitializeSecurity = TRUE;
	}
	*/

	// ͨ�� IWbemLocator �� IWbemServices ������ COM �ӿڷ��� WMI, ��ȡϵͳ��Ϣ 

	try
	{	// ���ڿ���spWbemLocator���������ڣ��Ա�֤�ܹ���CoUninitialize()֮ǰ�Զ�����spWbemLocator.Release()
		
		if( ::CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE,NULL, EOAC_NONE, 0 ) != S_OK )
		{
			//return str;
		}

		CComPtr<IWbemLocator> spWbemLocator; 
		if( spWbemLocator.CoCreateInstance( CLSID_WbemAdministrativeLocator , 0 , CLSCTX_INPROC_SERVER| CLSCTX_LOCAL_SERVER ) == S_OK ) 
		{ 
			CComPtr<IWbemServices> spWbemServices ; 
			if( spWbemLocator->ConnectServer( L"root\\cimv2" , NULL, NULL, NULL, 0, NULL, NULL, &spWbemServices ) == S_OK ) 
			{ 
				// ����ָ����Ϣ 

				USES_CONVERSION ; 
				CComPtr<IEnumWbemClassObject> spEnumWbemClassObject ; 
				CComBSTR bstrQuery ( L"Select * from " ) ; 
				bstrQuery += A2OLE(lpszClass) ; 
				if( spWbemServices->ExecQuery( L"WQL" , bstrQuery , WBEM_FLAG_RETURN_IMMEDIATELY , NULL, &spEnumWbemClassObject ) == S_OK ) 
				{ 
					ULONG uCount = 1, uReturned; 
					CComPtr<IWbemClassObject> spClassObject ; 
					if( spEnumWbemClassObject->Reset()  == S_OK ) 
					{ 
						int iEnumIdx = 0; 
						while( spEnumWbemClassObject->Next( WBEM_INFINITE,uCount, &spClassObject, &uReturned) == S_OK ) 
						{ 
							str = GetWmiInfo( spClassObject, lpszName) ; 
							spClassObject.Release(); 
						} 
					}
				}
			}
		} 
	}
	catch(...)
	{

	}

	return str; 
}

#pragma warning( pop ) 

_KIMI_NAMESPACE_END