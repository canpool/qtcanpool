#ifndef _CLICDLL_H_
#define _CLICDLL_H_

#ifdef __cplusplus
#define NR_EXTERN extern "C"
#else
#define NR_EXTERN extern
#endif

#define NRLICENSE_DLL

#ifdef WIN32
#ifdef NRLICENSE_DLL
#define LICENSE_EXPORT  __declspec(dllexport)
#else
#define LICENSE_EXPORT  __declspec(dllimport)
#endif
#else
#define LICENSE_EXPORT
#endif

/****************************************************************************
* 
****************************************************************************/
#define NR_PARAM_IN
#define NR_PARAM_OUT
#define NR_PARAM_INOUT
#define NR_IO
#define NR_API
#define NR_STRUCT typedef struct
#define NR_UNION  typedef union
#define NR_LOCAL  static
#define NR_TRUE   1
#define NR_FALSE  0

// #if (defined(__linux__)||defined(__unix__)||defined(__ppc64__)||defined(__i386__)||defined(__x86_64__))
// typedef          int	NR_LONG;
// typedef unsigned int	NR_ULONG;
// typedef unsigned int	NR_DWORD;
// #else
// typedef			 long	NR_LONG;
// typedef unsigned long	NR_ULONG;
// typedef unsigned long	NR_DWORD;
// #endif

#ifdef WIN32
typedef			 long	NR_LONG;
typedef unsigned long	NR_ULONG;
typedef unsigned long	NR_DWORD;
#else
typedef          int	NR_LONG;
typedef unsigned int	NR_ULONG;
typedef unsigned int	NR_DWORD;
#endif

#if defined(WIN32)
typedef unsigned __int64 NR_INT64;
#else
typedef unsigned long long	NR_INT64;
#endif
typedef unsigned char		NR_BYTE;
typedef unsigned char		NR_BOOLEAN;
typedef unsigned char		NR_UCHAR;
typedef unsigned short int	NR_USHORT;
typedef unsigned short int	NR_WORD;
typedef unsigned       int	NR_UINT;
typedef void				NR_VOID;
typedef char				NR_CHAR;
typedef int					NR_INT;
typedef short int			NR_SHORT;
typedef NR_DWORD			NR_STATUS;

//小于等于0失败，大于0成功
#define NR_LICENSE_STATUS_BASE 0

#define NR_LICENSE_FAIL				(NR_LICENSE_STATUS_BASE)
#define NR_LICENSE_WIND_FAIL		(NR_LICENSE_STATUS_BASE-1)
#define NR_LICENSE_NO_DOG			(NR_LICENSE_STATUS_BASE-2)
#define NR_LICENSE_NO_USER			(NR_LICENSE_STATUS_BASE-3)
#define NR_LICENSE_ERROR_USER		(NR_LICENSE_STATUS_BASE-4)
#define NR_LICENSE_NO_SOFTWARE		(NR_LICENSE_STATUS_BASE-5)
#define NR_LICENSE_ERROR_SOFTWARE	(NR_LICENSE_STATUS_BASE-6)
#define NR_LICENSE_NO_RIGHT			(NR_LICENSE_STATUS_BASE-7)
#define NR_LICENSE_ERROT_RIGHT		(NR_LICENSE_STATUS_BASE-8)

#define NR_LICENSE_SUCCESS			(NR_LICENSE_STATUS_BASE+1)
#define NR_LICENSE_FULL				(NR_LICENSE_STATUS_BASE+2)
#define NR_LICENSE_TEMP				(NR_LICENSE_STATUS_BASE+3)
#define NR_LICENSE_DOMAIN			(NR_LICENSE_STATUS_BASE+4)



#define NR_HOSTIDLEN		12//主机号长度
#define NR_LICENSELEN		24//序列号长度，新的
#define NR_TEMPLICENSELEN	41//临时序列号长度，新的
// #define NR_LICENSELEN		19//序列号长度，新的
// #define NR_TEMPLICENSELEN	36//临时序列号长度，新的

#define NR_LICENSELEN_OLD   16//序列号长度，老的
#define	NR_TEMPLICENSELEN_OLD 32//临时序列号长度，老的

#define NR_MAX_RAWDATA_LENGTH		100
#define NR_MAX_STRING_LENGTH		100
#define NR_MAX_DOMAIN_USER_LENGTH	20

#define NR_LICENSE100	0  //保留
#define NR_LICENSE10	10 //研发权限1，查看源码、编辑、下载调试
#define NR_LICENSE11	11 //研发权限2，除查看源码的其他权限；厂内域用户认证，厂外硬件狗认证
#define NR_LICENSE20	20 //工程及代理商权限，查看模块，配置功能，License认证
#define NR_LICENSE30	30 //最终用户权限，开入开出可配置，简单逻辑功能配置，License认证
#define NR_LICENSE40	40 //可以免费开放给用户的软件或模块，不对用户的使用做限制

#define MAX_MACID_NUMBER 20 //最大网卡数

#define LICENSEFILE "/.fclicense"
#define GMUDLICFILE "/gmud.lic"
#define GMUDDATEFILE "/gmud.dat"
#define SOPHIC_DEPLOY_DIR "SOPHIC_DEPLOY_DIR"
#define SYSTEM_TYPE_FILE "/etc/sysman/fclicense.ini"

#define START_YEAR 2011
#define START_MONTH 1
#define START_DAY 1
#define STOP_YEAR 2014
#define STOP_MONTH 12
#define STOP_DAY 31

/*Core obj public interfaces*/

//加密算法
NR_EXTERN LICENSE_EXPORT bool NR_API encryptStr(NR_PARAM_IN const char * srcStr, char * strDest);
//解密算法
NR_EXTERN LICENSE_EXPORT bool NR_API decryptStr(NR_PARAM_IN const char * strSrc, char * strDest);

/*
 说明：验证权限接口
 输入参数：
     sSoftName : 软件名称，必要
 返回值:
     NR_STATUS : <=0 - 失败，>0 - 成功
*/
NR_EXTERN LICENSE_EXPORT NR_STATUS NR_API CheckLicense(NR_PARAM_IN const char * sSoftName);

/*
 说明：时间校验
 输入参数：
     sTime : 时间字符串，格式："2011,4,12"
 返回值：true-校验成功，false-校验失败
*/
NR_EXTERN LICENSE_EXPORT bool NR_API CheckTime();

/*
 说明：生成永久型序列号
 输入参数：
     sSoftName : 软件名
     sContactID : 合同号
     sProName : 工程名
     sHostID : 主机号
 输出参数：
	 sLicense : 生成的永久型序列号
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createLicense(
												    NR_PARAM_IN const char * sSoftName,
												    NR_PARAM_IN const char * sContactID,
													NR_PARAM_IN const char * sProName,
													NR_PARAM_IN const char * sHostID,
													NR_PARAM_OUT char * sLicense);

/*
 说明：生成永久型序列号，老RCS的接口
 输入参数：
     sHostID : 主机号
 输出参数：
	 sLicense : 生成的永久型序列号
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createLicenseOld(
													NR_PARAM_IN const char * sHostID,
													NR_PARAM_OUT char * sLicense);

/*
 说明：生成临时型序列号
 输入参数：
     sSoftName : 软件名
     sContactID : 合同号
     sProName : 工程名
     nYear : 年
	 nMon : 月
     nDay : 日
 输出参数：
     tempLicense : 临时序列号
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createTempLicense(
													    NR_PARAM_IN const char * sSoftName,
														NR_PARAM_IN const char * sContactID,
														NR_PARAM_IN const char * sProName,
														NR_PARAM_IN const int nYear, 
														NR_PARAM_IN const int nMon, 
														NR_PARAM_IN const int nDay, 
														NR_PARAM_OUT char * tempLicense);

/*
 说明：生成临时型序列号，老的RCS用
 输入参数：
     nYear : 年
	 nMon : 月
     nDay : 日
 输出参数：
     tempLicense : 临时序列号
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createTempLicenseOld(
														NR_PARAM_IN const int nYear, 
														NR_PARAM_IN const int nMon, 
														NR_PARAM_IN const int nDay, 
														NR_PARAM_OUT char * tempLicense);


/*
 说明：获取当前主机的mac地址
 返回值：
     获取失败，返回false
	 获取成功，返回true
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createMACID(NR_PARAM_OUT NR_INT64 & macID);

/*
 说明：生成主机号
 输出参数：
     sHostID : 生成的主机号
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createHostID(NR_PARAM_OUT char * sHostID);

/*
 说明：获取当前主机的mac地址
 输入参数：
     nIndex : MAC地址序号
 返回值：
     获取失败，返回false
	 获取成功，返回true
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createMACIDNew(NR_PARAM_OUT NR_INT64 & macID, NR_PARAM_IN int nIndex = 0);

/*
 说明：生成主机号
 输入参数：
     nIndex : MAC地址序号
 输出参数：
     sHostID : 生成的主机号
*/
NR_EXTERN LICENSE_EXPORT bool NR_API createHostIDNew(NR_PARAM_OUT char * sHostID, NR_PARAM_IN const int nIndex = 0);

/*
 说明：注册序列号
 输入参数：
     sSoftName : 软件名称
     sContactID : 合同号
     sProName : 工程名
     sLicense : 要注册的序列号
*/
NR_EXTERN LICENSE_EXPORT bool NR_API registerLicense(
													 NR_PARAM_IN const char * sSoftName,
													 NR_PARAM_IN const char * sContactID,
													 NR_PARAM_IN const char * sProName,
													 NR_PARAM_IN const char * sLicense);

/*
 说明：根据错误代码获取描述信息
 输入参数：
     status : 错误代码
 返回值:
     错误的描述信息
*/
NR_EXTERN LICENSE_EXPORT char * NR_API getErrString(NR_PARAM_IN const NR_STATUS status);


/*
 说明：根据软件名获取license、合同号、工程名
 输入参数:
     sSoftName : 软件名
 输出参数：
     sLicense : 序列号
	 sContactID : 合同号
	 sProName : 工程名
*/
NR_EXTERN LICENSE_EXPORT bool NR_API getLicenseInfo(
													NR_PARAM_IN const char * sSoftName,
													NR_PARAM_IN const char * sContactID,
													NR_PARAM_IN const char * sProName,
													NR_PARAM_OUT char * sLicense);


#endif
