#ifndef __KIMI_KERNEL_DATA_COMPRESSOR_H__
#define __KIMI_KERNEL_DATA_COMPRESSOR_H__

#include <vector>

_KIMI_NAMESPACE_BEGIN

class CLZ4Compressor;
class CLZOCompressor;

//////////////////////////////////////////////////////////////////////////
// ����ѹ���ӿڶ��� - ICompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS ICompressor
{
public:
	enum {
		dcEmpty = 0,
		dcLZO,
		dcLZ4,
		dcQuickLZ,
		dcSnappy
	};
	static ICompressor* CreateCompressor(int nDataCompressType);

public:
	/// ͨ���ύ��Ҫѹ�������ݴ�С�����������Ҫ���ڴ��С
	virtual size_t GetCompressSize(size_t nInputSize) = 0;

	/// ����ѹ��������ݣ���ǰ4���ֽ�Ϊѹ��ǰ���ݳ��ȣ��ú����ɴ�ǰ4���ֽ��ж�ȡ��ԭʼ���ݳ���ֵ
	virtual size_t GetDecompressSize(const BYTE* pSource, size_t nSoureSize);

	/// ѹ������
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize) = 0;

	/// ��ѹ���ݣ�Ϊ�����ܿ��ǣ�pDestSize���Ϊ������ѹ��ǰ���ݴ�С
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize) = 0;
};

#pragma warning( push )
#pragma warning(disable : 4251)

//////////////////////////////////////////////////////////////////////////
// LZ0ѹ���㷨��װ�� - CLZOCompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CLZOCompressor : public ICompressor
{
public:
	CLZOCompressor();
	virtual ~CLZOCompressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);

private:
	// ѹ���ֵ��
	mutable std::vector<BYTE>  workmem_;
};

#pragma warning( pop )

//////////////////////////////////////////////////////////////////////////
// LZ4ѹ���㷨��װ�� - CLZ4Compressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CLZ4Compressor : public ICompressor
{
public:
	CLZ4Compressor();
	virtual ~CLZ4Compressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);

private:
};

//////////////////////////////////////////////////////////////////////////
// QuickLZѹ���㷨��װ�� - CQuickLZCompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CQuickLZCompressor : public ICompressor
{
public:
	CQuickLZCompressor();
	virtual ~CQuickLZCompressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual size_t GetDecompressSize(const BYTE* pSource, size_t nSoureSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);

protected:
	void *state_compress;
	void *state_decompress;
	//qlz_state_compress *state_compress;
	//qlz_state_decompress *state_decompress;
};

//////////////////////////////////////////////////////////////////////////
// google snappyѹ���㷨��װ�� - CSnappyCompressor
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CSnappyCompressor : public ICompressor
{
public:
	CSnappyCompressor();
	virtual ~CSnappyCompressor();

public:
	virtual size_t GetCompressSize(size_t nInputSize);
	virtual size_t GetDecompressSize(const BYTE* pSource, size_t nSoureSize);
	virtual BOOL Compress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
	virtual BOOL Decompress(const BYTE* pSource, size_t nSoureSize, BYTE* pDest, size_t* pDestSize);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_DATA_COMPRESSOR_H__