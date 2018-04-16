#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
using namespace std;

const short DB_KNOW=0;
const short DB_DIGITAL=1;
const short DB_INT32=2;
const short DB_FLOATE32=3; 
const short DB_INT64=4; 
const short DB_FLOATE64=5;
const short DB_STRING=6; 
const short DB_BLOB=7;

//const short TYPE_NONE=0;
//const short TYPE_DIGITAL=1;
//const short TYPE_LONG=2;
//const short TYPE_FLOAT=3;


typedef struct _TDataBlock
{
	UINT nStart;
	UINT nSize;
	UINT nInterval;
	UINT nBlock;
	std::string szType;
	BYTE *pbyBuf;
	long lUpDateTime;
}TDataBlock, *LPTDataBlock;

typedef struct _PointInfo
{
	std::string szID;
	std::string szName;
	std::string szNote;
	WORD utype;
}PointInfo, *pPointInfo;

typedef struct _ParamPointInfo
{
	std::string szID;
	UINT32 uAddStart;
	UINT32 uLKOffset;
	short iDateType;
	short iTypeSize;
	short iBitPos;
}ParamPointInfo, *p_ParamPointInfo;

typedef std::vector<ParamPointInfo> RegVarTable;
typedef std::vector<ParamPointInfo>::const_iterator RegVarTableItor;

typedef std::map<int, RegVarTable> VarTotalMAP;
typedef std::map<int, RegVarTable>::iterator VarTotalMAPItor;

typedef std::vector<LPTDataBlock> CVectorTDataBlock;