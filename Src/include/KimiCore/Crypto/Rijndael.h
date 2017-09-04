// rijndael.h: interface for the rijndael class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __KIMI_KERNEL_RIJNDAEL_H__
#define __KIMI_KERNEL_RIJNDAEL_H__

_KIMI_NAMESPACE_BEGIN

typedef unsigned char   u1byte; /* an 8 bit unsigned character type */
typedef unsigned short  u2byte; /* a 16 bit unsigned integer type   */
typedef unsigned long   u4byte; /* a 32 bit unsigned integer type   */

typedef signed char     s1byte; /* an 8 bit signed character type   */
typedef signed short    s2byte; /* a 16 bit signed integer type     */
typedef signed long     s4byte; /* a 32 bit signed integer type     */

class KIMI_CORE_CLASS CRijndael
{
public:
	CRijndael();
	virtual ~CRijndael();

public:
	// ���ü���/����KEY
	void SetKey(const u1byte key[], const u4byte key_len);

	// ��������
	bool EncryptData(u1byte *pData, u1byte *pBuffer, u4byte &dwLen);
	// ��������
	bool DecryptData(u1byte *pData, u1byte *pBuffer, u4byte &dwLen);

protected:
	void Encrypt(const u1byte in_blk[16], u1byte out_blk[16]);
	void Decrypt(const u1byte in_blk[16], u1byte out_blk[16]);

private:
	u4byte  k_len;
	u4byte  e_key[64];
	u4byte  d_key[64];
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_RIJNDAEL_H__
