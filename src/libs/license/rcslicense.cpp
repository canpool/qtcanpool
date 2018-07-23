
#include "rcslicense.h"
#include <time.h>

/*
* microsoft windows
*/
#if defined(WIN32) || defined(Q_OS_WIN32)
#include <windows.h>
#include <wincon.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <fstream>

using namespace std;

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff [30];
} ASTAT, * PASTAT;

#define LICENSE_SUBKEY "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\FC-1000"
#define LICENSE_VALUE_NAME "LICENSE"
#define LICENSE_VALUE_NAME1 "LICENSE1"
#define HOMEFILE "USERPROFILE"

typedef NR_DWORD (WINAPI *PtrGetAdaptersInfo)(PIP_ADAPTER_INFO, PULONG);
static PtrGetAdaptersInfo ptrGetAdaptersInfo = 0;
typedef NR_ULONG (WINAPI *PtrGetAdaptersAddresses)(ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);
static PtrGetAdaptersAddresses ptrGetAdaptersAddresses = 0;
typedef NR_DWORD (WINAPI *PtrGetNetworkParams)(PFIXED_INFO, PULONG);
static PtrGetNetworkParams ptrGetNetworkParams = 0;


#endif //win32

#if defined(__linux__) || defined(__LINUX__) || defined(Q_OS_LINUX)

//#include <asm/io.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <sys/io.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <scsi/sg.h>

#include <linux/hdreg.h>
#include <linux/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/procfs.h>

#include <features.h>
#include <netpacket/packet.h>

#include <fstream>
#include <iostream>

#define SG_CHECK_CONDITION		0x02
#define SG_DRIVER_SENSE			0x08
#define SG_ATA_16				0x85
#define SG_ATA_16_LEN			16
#define SG_ATA_LBA48			1
#define SG_ATA_PROTO_NON_DATA   (3<<1)
#define SG_ATA_PROTO_PIO_IN		(4<<1)
#define SG_ATA_PROTO_PIO_OUT    (5<<1)
#define SG_ATA_PROTO_DMA		(6<<1)
#define SG_ATA_PROTO_UDMA_IN    (11<<1) /* not yet supported in libata */
#define SG_ATA_PROTO_UDMA_OUT   (12<<1) /* not yet supported in libata */
#define ATA_USING_LBA			(1<<6)

#define START_SERIAL            10  /* ASCII serial number */
#define LENGTH_SERIAL           4  /* 10 words (8 bytes or characters) */

enum
{
	ATA_OP_CHECKPOWERMODE1        = 0xe5,
	ATA_OP_CHECKPOWERMODE2        = 0x98,
	ATA_OP_DOORLOCK               = 0xde,
	ATA_OP_DOORUNLOCK             = 0xdf,
	ATA_OP_FLUSHCACHE             = 0xe7,
	ATA_OP_FLUSHCACHE_EXT         = 0xea,
	ATA_OP_IDENTIFY               = 0xec,
	ATA_OP_PIDENTIFY              = 0xa1,
	ATA_OP_SECURITY_DISABLE       = 0xf6,
	ATA_OP_SECURITY_ERASE_PREPARE = 0xf3,
	ATA_OP_SECURITY_ERASE_UNIT    = 0xf4,
	ATA_OP_SECURITY_FREEZE_LOCK   = 0xf5,
	ATA_OP_SECURITY_SET_PASS	  = 0xf1,
	ATA_OP_SECURITY_UNLOCK        = 0xf2,
	ATA_OP_SETFEATURES            = 0xef,
	ATA_OP_SETIDLE1               = 0xe3,
	ATA_OP_SETIDLE2               = 0x97,
	ATA_OP_SLEEPNOW1              = 0xe5,
	ATA_OP_SLEEPNOW2              = 0x99,
	ATA_OP_SMART                  = 0xb0,
	ATA_OP_STANDBYNOW1            = 0xe0,
	ATA_OP_STANDBYNOW2            = 0x94,
};
enum
{
	SG_CDB2_TLEN_NODATA		= 0 << 0,
	SG_CDB2_TLEN_FEAT		= 1 << 0,
	SG_CDB2_TLEN_NSECT		= 2 << 0,
	SG_CDB2_TLEN_BYTES		= 0 << 2,
	SG_CDB2_TLEN_SECTORS    = 1 << 2,
	SG_CDB2_TDIR_TO_DEV		= 0 << 3,
	SG_CDB2_TDIR_FROM_DEV	= 1 << 3,
	SG_CDB2_CHECK_COND		= 1 << 5,
};

#define MAXINTERFACES   16

#ifndef MAX_IFINDEX
#define MAX_IFINDEX 8
#endif

const char sNRLicenseDir[100] = "/users/ems";
const char sNRLicenseFile[100] = "/users/ems/.fclicense";
#define HOMEFILE "HOME"
#define USER "USER"
#define USERNAME "USERNAME"

#endif //linux

#if defined(__sun) || defined(__solaris__) || defined(__SOLARIS__) || defined(Q_OS_SOLARIS)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/procfs.h>
#include <sys/sockio.h>
#include <sys/scsi/generic/commands.h>
#include <sys/scsi/generic/inquiry.h>
#include <sys/scsi/impl/uscsi.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <fstream.h>
#include "iostream.h"

#define MAXINTERFACES   16

const char sNRLicenseDir[100] = "/users/ems";
const char sNRLicenseFile[100] = "/users/ems/.fclicense";
#define HOMEFILE "HOME"
#define USER "USER"
#define USERNAME "USERNAME"

char* getStrStr(const char * src, const char * dest)
{
	if(strlen(dest) <= 0)
		return 0;
	
	char * stempstr = (char*)malloc(256);
	memset(stempstr,0,101);
	
	char ststr[256] = {'\0'};
	char ssstr[256] = {'\0'};
	memcpy(ststr,src,256);
	int nsrclength = strlen(src);
	int ndestlength = strlen(dest);
	int nidx = 0;
	int i = 0;
	int j = 0;
	int offset = 0;
	while (i < nsrclength)
	{
		sscanf(ststr, "%[^ ]", ssstr);
		if(strcasecmp(ssstr, dest) == 0)
		{
			while(ststr[strlen(ssstr)+offset+1] == ' ')
				offset++;
			if(strcasecmp(dest,"Vendor:") == 0)
				sscanf(ststr+strlen(ssstr)+1+offset,"%[^Product]", stempstr);
			else if(strcasecmp(dest,"Product:") == 0)
				sscanf(ststr+strlen(ssstr)+1+offset,"%[^Revision]", stempstr);
			else if(strcasecmp(dest,"Revision:") == 0)
				sscanf(ststr+strlen(ssstr)+1+offset,"%[^Serial]", stempstr);
			else
				sscanf(ststr+strlen(ssstr)+1+offset,"%[^ ]", stempstr);
			break;
		}
		i = i+ strlen(ssstr) + 1;
		memset(ststr,0,sizeof(ststr));
		while(src[i] == ' ')
			i++;
		memcpy(ststr,src+i,256);
//		printf("%s\n",ststr);
	}
	
	return stempstr;
}

#endif //solaris

/*
* ibm aix
*/
#if defined(__aix__) || defined(__AIX__) || defined(Q_OS_AIX) || defined(_AIX)

//#include <sys/sockio.h>
//#include <sys/scsi/generic/commands.h>
//#include <sys/scsi/generic/inquiry.h>
//#include <sys/scsi/impl/uscsi.h>

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ndd_var.h>
#include <sys/kinfo.h>

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/procfs.h>

#include <fstream.h>
#include "iostream.h"

const char sNRLicenseDir[100] = "/users/ems";
const char sNRLicenseFile[100] = "/users/ems/.fclicense";
#define HOMEFILE "HOME"
#define USER "USER"
#define USERNAME "USERNAME"

//#include <net/if.h>
//#include <net/if_arp.h>

#include <libperfstat.h>
#include <sys/stat.h>

extern "C"
{
	getkerninfo ( int, struct kinfo_ndd *, int *, NR_INT64 );
}

#endif //aix

#if defined(Q_OS_HPUX) || defined(__HPUX) || defined(__hpux)

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream.h>
#include "iostream.h"

const char sNRLicenseDir[100] = "/users/ems";
const char sNRLicenseFile[100] = "/users/ems/.fclicense";
#define HOMEFILE "HOME"
#define USER "USER"
#define USERNAME "USERNAME"

#endif



// int HostIDOrderFit[6] =
// {
// 	4, 2, 5, 1, 3, 0
// };

int HostIDOrderFit[6] =
{
	5, 1, 3, 4, 2, 0
};

// int hostToLicense[16] =
// {
// 	10, 6, 12, 3, 2, 7, 1, 5, 9, 8, 12, 9, 2, 4, 8, 11
// };

int hostToLicense[16] =
{
	10, 6, 12, 3, 6, 7, 2, 5, 7, 8, 12, 9, 2, 2, 8, 11
};

/*
char CharList[16] =
{
	'b', '4', '8', '6', '0', '5', '1', 'c', '3', 'e', '9', '2', 'd', 'a', '7', 'f'
};
*/

// char CharList[16] =
// {
// 	'0', '4', '8', '6', '0', '5', '1', '3', '3', '7', '9', '2', '9', '2', '7', '8'
// };

char CharList[16] =
{
	'0', '2', '8', '6', '0', '1', '9', '5', '3', '7', '4', '2', '7', '2', '7', '8'
};

char CharListOld[36]={
	'b','y','d','4','8','6','0','j','5','q',
	'w','1','c','3','e','i','h','g','f','9',
	'v','l','x','n','o','p','2','r','s','t',
	'u','k','a','m','7','z'
};

CRcsLicense::CRcsLicense()
{
}

static NR_UINT julianDayFromDate(int year, int month, int day)
{
    if (year < 0)
        ++year;
	
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15)))) {
        // Gregorian calendar starting from October 15, 1582
        // Algorithm from Henry F. Fliegel and Thomas C. Van Flandern
        return (1461 * (year + 4800 + (month - 14) / 12)) / 4
			+ (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12
			- (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4
			+ day - 32075;
    } else if (year < 1582 || (year == 1582 && (month < 10 || (month == 10 && day <= 4)))) {
        // Julian calendar until October 4, 1582
        // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
        int a = (14 - month) / 12;
        return (153 * (month + (12 * a) - 3) + 2) / 5
			+ (1461 * (year + 4800 - a)) / 4
			+ day - 32083;
    } else {
        // the day following October 4, 1582 is October 15, 1582
        return 0;
    }
}

/*
* 校验
*/
NR_EXTERN LICENSE_EXPORT NR_STATUS NR_API CheckLicense(NR_PARAM_IN const char * sSoftName)
{
	//test macid
// 	NR_INT64 pmacIDList[MAX_MACID_NUMBER];
// 	memset(pmacIDList,0,MAX_MACID_NUMBER*sizeof(NR_INT64));
// 	int ncount = 0;
// 	if(!getMacIDList(pmacIDList,ncount))
// 		return 0;
// 	printf("mac count: %d\n",ncount);
// 	for (int nmacno = 0; nmacno < ncount && nmacno < MAX_MACID_NUMBER; nmacno++)
// 	{
// 		NR_INT64 macID = pmacIDList[nmacno];
// 		printf("macID%d:  %lld\n",nmacno+1,macID);
// 	}
// 	return NR_LICENSE_SUCCESS;

#if defined(WIN32)
	bool bDomainUser = isDomainUser();
	if(bDomainUser)
		return NR_LICENSE_DOMAIN;
#endif

// 	NR_STATUS nretold = checkLicenseOldPrivate(sSoftName);
// 	if(nretold != NR_LICENSE_SUCCESS && nretold != NR_LICENSE_FULL && nretold != NR_LICENSE_TEMP)
// 	{
		NR_STATUS nretnew = checkLicenseNewPrivate(sSoftName);
		return nretnew;
// 	}
// 	return nretold;
}

NR_STATUS checkLicenseOldPrivate(const char * sSoftName)
{

	//1.判断是否有nrlicense.ini文件，用于判断是否为风电类型
	bool bWind = true;
	int ntype = 1;
	char sTypeStr[201] = {'\0'};
	char sTypeFile[512] = {'\0'};
	memset(sTypeStr,0,sizeof(sTypeStr));
	memset(sTypeFile,0,sizeof(sTypeFile));
	char* sophic_deploy_dir = getenv(SOPHIC_DEPLOY_DIR);
#if defined(LOG_LOG)
	//log
	char slogcontent[1024] = {'\0'};
	char slogtemp[512] = {'\0'};
	char sLogFile[512] = {'\0'};
	memset(slogcontent,0,sizeof(slogcontent));
	memset(slogtemp,0,sizeof(slogtemp));
	memset(sLogFile,0,sizeof(sLogFile));
#endif
	if(sophic_deploy_dir)
	{
		
		sprintf(sTypeFile, "%s%s", sophic_deploy_dir, SYSTEM_TYPE_FILE);
		
#if defined(LOG_LOG)
		sprintf(sLogFile, "%s%s", getenv(HOMEFILE), "/nrlicense.log");
		time_t timer; 
		time(&timer); 
		struct tm* t_tm = localtime(&timer);
		
		memset(slogtemp,0,sizeof(slogtemp));
		strftime(slogtemp,100,"%Y-%m-%d %H:%M:%S-->",t_tm);
		strcat(slogcontent,slogtemp);
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"%s%s,","softname:",sSoftName);
		strcat(slogcontent,slogtemp);
#endif

		ifstream ftypein(sTypeFile);
		if(!ftypein.getline(sTypeStr, 200))
		{
			ftypein.close();
			bWind = false;
		}
		else
		{
			sscanf(sTypeStr, "%*[^=]=%d", &ntype);
			ftypein.close();
		}
		
		if(ntype != 0)
		{
			bWind = false;
		}
	}
	else
		bWind = false;

	if(bWind)
	{
#if defined(LOG_LOG)
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"Type:%s\n","Wind");
		strcat(slogcontent,slogtemp);
		ofstream flogout(sLogFile,ios::out|ios::app);
		flogout<<slogcontent;
		flogout.close();
#endif
		//风电系统，判断有效时间
		if(CheckTime())
			return NR_LICENSE_SUCCESS;
		else
			return NR_LICENSE_WIND_FAIL;
	}
	else
	{
		//非风电系统，判断license
		NR_DWORD lLicense = NR_TEMPLICENSELEN + 1;
		char szUserLicense[NR_LICENSELEN+1] = {'\0'};
		char szLicense[NR_LICENSELEN+1] = {'\0'};
		char szHostid[NR_HOSTIDLEN] = {'\0'};
		char szRegLicense[NR_TEMPLICENSELEN+1] = {'\0'};
		char sContact[100] = {'\0'};
		char sProName[100] = {'\0'};
		memset(szUserLicense,0,sizeof(szUserLicense));
		memset(szLicense,0,sizeof(szLicense));
		memset(szHostid,0,sizeof(szHostid));
		memset(szRegLicense,0,sizeof(szRegLicense));
		memset(sContact,0,sizeof(sContact));
		memset(sProName,0,sizeof(sProName));
		getLicenseInfo(sSoftName, sContact, sProName, szRegLicense);
		//判断序列号长度是否为正式序列号或是临时序列号长度
		lLicense = strlen(szRegLicense);

#if defined(LOG_LOG)
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"License:%s,",szRegLicense);
		strcat(slogcontent,slogtemp);
		ofstream flogout(sLogFile,ios::out|ios::app);
		flogout<<slogcontent;
		flogout.close();
#endif

		if ( ( lLicense != NR_LICENSELEN ) && ( lLicense != NR_TEMPLICENSELEN ) )
		{
#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			memset(slogcontent,0,sizeof(slogcontent));
			sprintf(slogtemp,"Error:%s\n","length error");
			strcat(slogcontent,slogtemp);
			ofstream flogout(sLogFile,ios::out|ios::app);
			flogout<<slogcontent;
			flogout.close();
#endif
			return NR_LICENSE_FAIL;
		}
		
		// try to create the  license
		if ( lLicense == NR_LICENSELEN )	//true license
		{
			//strcpy(szUserLicense, szRegLicense);
			for (int i = 0; i < strlen(szRegLicense) && i < NR_LICENSELEN+1; i++)
			{
				szUserLicense[i] = szRegLicense[i];
			}
			//host id
			if ( !createHostID ( szHostid ) )
			{
				return NR_LICENSE_FAIL;
			}

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			memset(slogcontent,0,sizeof(slogcontent));
			sprintf(slogtemp,"HostID:%s,",szHostid);
			strcat(slogcontent,slogtemp);
#endif

			//create license
			if ( !createLicense ( sSoftName, sContact, sProName, szHostid, szLicense ) )
			{
#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Create:%s\n","invalid");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif
				return NR_LICENSE_FAIL;
			}

			szLicense[sizeof(szLicense)-1] = '\0';

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"CreateLicense:%s,",szLicense);
			strcat(slogcontent,slogtemp);
#endif

#ifdef WIN32

			//compare license
			if ( 0 != strcmpi ( szLicense, szRegLicense ) )
			{
#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif
				return NR_LICENSE_FAIL;
			}

#else
			//compare license
			if ( 0 != strcasecmp ( szLicense, szRegLicense ) )
			{
#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif
				return NR_LICENSE_FAIL;
			}

#endif
#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"Check:%s\n","Full license");
			strcat(slogcontent,slogtemp);
			ofstream flogout(sLogFile,ios::out|ios::app);
			flogout<<slogcontent;
			flogout.close();
#endif
			return NR_LICENSE_FULL;
		}

		//temperary license
		if ( lLicense == NR_TEMPLICENSELEN ) 
		{
			for ( int i = 0; i < NR_LICENSELEN + 1; i++ )
			{
				szUserLicense[i] = szRegLicense[i+17];
			}

			for ( int j = 0; j < NR_HOSTIDLEN; j++ )
				szHostid[j] = szRegLicense[j+4];

			szHostid[6] = szRegLicense[1];
			szHostid[9] = szRegLicense[2];

#if defined(LOG_LOG)
			memset(slogcontent,0,sizeof(slogcontent));
#endif
			//create license
			if ( !createLicense ( sSoftName, sContact, sProName, szHostid, szLicense ) )
			{
#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Create:%s\n","invalid");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif
				return NR_LICENSE_FAIL;
			}

			szLicense[sizeof(szLicense)-1] = '\0';

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"CreateLicense:%s,",szLicense);
			strcat(slogcontent,slogtemp);
#endif

#ifdef WIN32
			//compare license
			if ( 0 != strcmpi( szLicense, szUserLicense ) )
			{
#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif
				return NR_LICENSE_FAIL;
			}

#else
			//compare license
			if ( 0 != strcasecmp( szLicense, szUserLicense ) )
			{
#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif
				return NR_LICENSE_FAIL;
			}

#endif
			int nYear = 0;
			int nMon = 0;
			int nDay = 0;
			int nYearCur = 0;
			int nMonCur = 0;
			int nDayCur = 0;
			sscanf(szRegLicense, "%*6s%d-%d-%d[^-]", &nYear, &nMon, &nDay);
			char ctime[30] = {'\0'};
			time_t curTime = time(0);
			strftime(ctime, sizeof(ctime), "%Y-%m-%d %X", localtime(&curTime));
			sscanf(ctime, "%d-%d-%d", &nYearCur, &nMonCur, &nDayCur);


			if (   ( nYear < nYearCur ) ||
					( nYear == nYearCur && nMon < nMonCur ) ||
					( nYear == nYearCur && nMon == nMonCur && nDay < nDayCur) )
			{
#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif
				return NR_LICENSE_FAIL;
			}
#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"Check:%s\n","Temp license");
			strcat(slogcontent,slogtemp);
			ofstream flogout(sLogFile,ios::out|ios::app);
			flogout<<slogcontent;
			flogout.close();
#endif

			return NR_LICENSE_TEMP;
		}

		return NR_LICENSE_FAIL;
	}
}

NR_STATUS checkLicenseNewPrivate(const char * sSoftName)
{
	NR_INT64 pmacIDList[MAX_MACID_NUMBER];//最多20个网卡
	memset(pmacIDList,0,MAX_MACID_NUMBER*sizeof(NR_INT64));
	int ncount = 0;
	if(!getMacIDList(pmacIDList,ncount))
		return NR_LICENSE_FAIL;

	NR_DWORD lLicense = NR_TEMPLICENSELEN + 1;
	char szUserLicense[NR_LICENSELEN+1] = {'\0'};
	char szLicense[NR_LICENSELEN+1] = {'\0'};
	char szRegLicense[NR_TEMPLICENSELEN+1] = {'\0'};
	char sContact[100] = {'\0'};
	char sProName[100] = {'\0'};
	
	memset(szRegLicense,0,sizeof(szRegLicense));
	memset(sContact,0,sizeof(sContact));
	memset(sProName,0,sizeof(sProName));
	getLicenseInfo(sSoftName, sContact, sProName, szRegLicense);

	for (int nmacno = 0; nmacno < ncount && nmacno < MAX_MACID_NUMBER; nmacno++)
	{
		NR_INT64 macID = pmacIDList[nmacno];
		
		char szHostid[NR_HOSTIDLEN+1] = {'\0'};
		memset(szLicense,0,sizeof(szLicense));
		memset(szUserLicense,0,sizeof(szUserLicense));
		memset(szHostid,0,sizeof(szHostid));
		
		//判断序列号长度是否为正式序列号或是临时序列号长度
		lLicense = strlen(szRegLicense);

#if defined(LOG_LOG)
		//log
		char slogcontent[1024] = {'\0'};
		char slogtemp[512] = {'\0'};
		char sLogFile[512] = {'\0'};
		sprintf(sLogFile, "%s%s", getenv(HOMEFILE), "/nrlicense.log");
		time_t timer; 
		time(&timer); 
		struct tm* t_tm = localtime(&timer);
		
		memset(slogtemp,0,sizeof(slogtemp));
		strftime(slogtemp,100,"%Y-%m-%d %H:%M:%S-->",t_tm);
		strcat(slogcontent,slogtemp);
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"%s%d,","Index:",nmacno+1);
		strcat(slogcontent,slogtemp);
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"%s%s,","softname:",sSoftName);
		strcat(slogcontent,slogtemp);

		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"License:%s,",szRegLicense);
		strcat(slogcontent,slogtemp);
		ofstream flogout(sLogFile,ios::out|ios::app);
		flogout<<slogcontent;
		flogout.close();
#endif

		if ( ( lLicense != NR_LICENSELEN ) && ( lLicense != NR_TEMPLICENSELEN ) )
		{

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			memset(slogcontent,0,sizeof(slogcontent));
			sprintf(slogtemp,"Error:%s\n","length error");
			strcat(slogcontent,slogtemp);
			ofstream flogout(sLogFile,ios::out|ios::app);
			flogout<<slogcontent;
			flogout.close();
#endif

			if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
				return NR_LICENSE_FAIL;
			else
				continue;
		}
		
		// try to create the  license
		if ( lLicense == NR_LICENSELEN )	//true license
		{
			//strcpy(szUserLicense, szRegLicense);
			for (int i = 0; i < strlen(szRegLicense) && i < NR_LICENSELEN+1; i++)
			{
				szUserLicense[i] = szRegLicense[i];
			}
			//printf("szUserLicense:%s\n",szUserLicense);
			//szUserLicense[sizeof(szUserLicense)-1] = '\0';

			if ( !createHostIDNew1 ( szHostid, macID ) )
			{
				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			memset(slogcontent,0,sizeof(slogcontent));
			sprintf(slogtemp,"HostID:%s,",szHostid);
			strcat(slogcontent,slogtemp);
#endif

			if ( !createLicense ( sSoftName, sContact, sProName, szHostid, szLicense ) )
			{

#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Create:%s\n","invalid");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif

				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

			szLicense[sizeof(szLicense)-1] = '\0';

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"CreateLicense:%s,",szLicense);
			strcat(slogcontent,slogtemp);
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"szUserLicense:%s,",szUserLicense);
			strcat(slogcontent,slogtemp);
#endif

#ifdef WIN32

			if ( 0 != strcmpi ( szLicense, szRegLicense ) )
			{

#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif

				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

#else

			if ( 0 != strcasecmp ( szLicense, szRegLicense ) )
			{

#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif

				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

#endif

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"Check:%s\n","Full license");
			strcat(slogcontent,slogtemp);
			ofstream flogout(sLogFile,ios::out|ios::app);
			flogout<<slogcontent;
			flogout.close();
#endif
			return NR_LICENSE_FULL;
		}

		if ( lLicense == NR_TEMPLICENSELEN ) //temperary license
		{
			for ( int i = 0; i < NR_LICENSELEN + 1; i++ )
			{
				szUserLicense[i] = szRegLicense[i+17];
			}

			for ( int j = 0; j < NR_HOSTIDLEN; j++ )
				szHostid[j] = szRegLicense[j+4];

			szHostid[6] = szRegLicense[1];
			szHostid[9] = szRegLicense[2];

#if defined(LOG_LOG)
			memset(slogcontent,0,sizeof(slogcontent));
#endif

			if ( !createLicense ( sSoftName, sContact, sProName, szHostid, szLicense ) )
			{

#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Create:%s\n","invalid");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif

				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

			szLicense[sizeof(szLicense)-1] = '\0';

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"CreateLicense:%s,",szLicense);
			strcat(slogcontent,slogtemp);
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"szUserLicense:%s,",szUserLicense);
			strcat(slogcontent,slogtemp);
#endif

#ifdef WIN32

			if ( 0 != strcmpi( szLicense, szUserLicense ) )
			{

#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif

				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

#else

			if ( 0 != strcasecmp( szLicense, szUserLicense ) )
			{

#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif

				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

#endif
			int nYear = 0;
			int nMon = 0;
			int nDay = 0;
			int nYearCur = 0;
			int nMonCur = 0;
			int nDayCur = 0;
			sscanf(szRegLicense, "%*6s%d-%d-%d[^-]", &nYear, &nMon, &nDay);
			char ctime[30] = {'\0'};
			time_t curTime = time(0);
			strftime(ctime, sizeof(ctime), "%Y-%m-%d %X", localtime(&curTime));
			sscanf(ctime, "%d-%d-%d", &nYearCur, &nMonCur, &nDayCur);

			if (   ( nYear < nYearCur ) ||
					( nYear == nYearCur && nMon < nMonCur ) ||
					( nYear == nYearCur && nMon == nMonCur && nDay < nDayCur) )
			{

#if defined(LOG_LOG)
				memset(slogtemp,0,sizeof(slogtemp));
				sprintf(slogtemp,"Check:%s\n","Failed license");
				strcat(slogcontent,slogtemp);
				ofstream flogout(sLogFile,ios::out|ios::app);
				flogout<<slogcontent;
				flogout.close();
#endif

				if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
					return NR_LICENSE_FAIL;
				else
					continue;
			}

#if defined(LOG_LOG)
			memset(slogtemp,0,sizeof(slogtemp));
			sprintf(slogtemp,"Check:%s\n","Temp license");
			strcat(slogcontent,slogtemp);
			ofstream flogout(sLogFile,ios::out|ios::app);
			flogout<<slogcontent;
			flogout.close();
#endif

			return NR_LICENSE_TEMP;
		}

		if(nmacno == ncount-1 || nmacno == MAX_MACID_NUMBER-1)
			return NR_LICENSE_FAIL;
		else
			continue;
	}
	return NR_LICENSE_SUCCESS;
}

NR_EXTERN LICENSE_EXPORT bool NR_API createLicense(
    NR_PARAM_IN const char * sSoftName,
    NR_PARAM_IN const char * sContactID,
    NR_PARAM_IN const char * sProName,
    NR_PARAM_IN const char * sHostID,
    NR_PARAM_OUT char * sLicense)
{
	memset(sLicense, 0, NR_LICENSELEN + 1);
	char pLicense[NR_LICENSELEN-3] = {'\0'};
	int i;

	for ( i = 0; i < NR_LICENSELEN - 4; i++ )
	{
		pLicense[i] = sHostID[i%NR_HOSTIDLEN];
	}

	//hostid
	for ( i = 0; i < NR_LICENSELEN - 4; i++ )
	{
		pLicense[i] = sHostID[hostToLicense[i%16] % NR_HOSTIDLEN];
	}

	//soft name
	for ( i = 0; i < int(strlen(sSoftName)); i++)
	{
        pLicense[i%(NR_LICENSELEN-4)] = CharList[ ( pLicense[i%(NR_LICENSELEN-4)] + abs((int)sSoftName[i]) ) % 16 ]; // add (int) 170701
	}

	//contact id
	for ( i = 0; i < int(strlen(sContactID)); i++ )
	{
        pLicense[i%(NR_LICENSELEN-4)] = CharList[ ( pLicense[i%(NR_LICENSELEN-4)] + abs((int)sContactID[i]) ) % 16 ];
	}

	//project name
	for ( i = 0; i < int(strlen(sProName)); i++ )
	{
        pLicense[i%(NR_LICENSELEN-4)] = CharList[ ( pLicense[i%(NR_LICENSELEN-4)] + abs((int)sProName[i]) ) % 16 ];
	}

	for ( i = 0; i < NR_LICENSELEN - 4; i++ )
	{
		pLicense[i] = CharList[ ( pLicense[i] + i ) % 16];

		if ( pLicense[i] >= '0' && pLicense[i] <= '9' )
		{
			pLicense[i] = CharList[ ( pLicense[i] + 3 * i ) % 16];
		}
	}

	for ( i = 0; i < NR_LICENSELEN - 4; i++ )
	{
		if ( pLicense[i] >= '0' && pLicense[i] <= '9' )
		{
			pLicense[i] = CharList[ ( pLicense[i] + pLicense[ ( i+1 ) % (NR_LICENSELEN-4)] + pLicense[ ( i+2 ) % (NR_LICENSELEN-4)] + pLicense[ ( i+3 ) % (NR_LICENSELEN-4)] ) % 16];
		}
	}

	if (formatLicense(sLicense, pLicense))
		return true;

	return false;
}

NR_EXTERN LICENSE_EXPORT bool NR_API createTempLicense(
    NR_PARAM_IN const char * sSoftName,
    NR_PARAM_IN const char * sContactID,
    NR_PARAM_IN const char * sProName,
    NR_PARAM_IN const int nYear,
    NR_PARAM_IN const int nMon,
    NR_PARAM_IN const int nDay,
    NR_PARAM_OUT char * tempLicense)
{
	memset(tempLicense, 0, NR_TEMPLICENSELEN + 1);
	sprintf ( tempLicense, "Temlic%04d-%02d-%02d-", nYear, nMon, nDay );
	char szLicense[NR_LICENSELEN+1] = {'\0'};
	char szHostid[NR_HOSTIDLEN+1] = {'\0'};
	int i = 0;
	memset(szLicense,0,sizeof(szLicense));
	memset(szHostid,0,sizeof(szHostid));

	for ( i = 0; i < NR_HOSTIDLEN; i++ )
		szHostid[i] = tempLicense[i+4];

	szHostid[6] = tempLicense[1];
	szHostid[9] = tempLicense[2];

	if ( !createLicense ( sSoftName, sContactID, sProName, szHostid, szLicense ) )
		return false;

	szLicense[sizeof(szLicense)-1] = '\0';

	strcat(tempLicense, szLicense);
	return true;
}

NR_EXTERN LICENSE_EXPORT bool NR_API createHostID(NR_PARAM_OUT char * sHostID)
{

	NR_INT64 cpuID = 0;
	NR_INT64 hardID = 0;
	NR_INT64 macID = 0;
	NR_INT64 HostID = 0;
	memset(sHostID, 0, NR_HOSTIDLEN + 1);

	if ( !createCPUID ( cpuID ) )
		cpuID = 0;

	if ( !createHardID ( hardID ) )
		hardID = 0;

	if ( !createMACID ( macID ) )
	{
		macID = 0;
	}

#if defined(LOG_LOG)
	char slogcontent[1024] = {'\0'};
	char slogtemp[512] = {'\0'};
	char sLogFile[512] = {'\0'};
	memset(slogcontent,0,sizeof(slogcontent));
	sprintf(sLogFile, "%s%s", getenv(HOMEFILE), "/nrlicense.log");
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%d,","CPUID:",cpuID);
	strcat(slogcontent,slogtemp);
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%x,","HardID:",hardID);
	strcat(slogcontent,slogtemp);
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%d,","MacID:",macID);
	strcat(slogcontent,slogtemp);
	ofstream flogout(sLogFile,ios::out|ios::app);
	flogout<<slogcontent;
	flogout.close();
#endif

	if(cpuID == 0 && hardID == 0 && macID == 0)
		return false;

	HostID += cpuID;
	HostID += hardID;
	NR_BYTE *pFeatureHostID;
	NR_BYTE *pFeatureMacID;
	pFeatureMacID = ( NR_BYTE * ) & macID;

	for ( int i = 0; i < 6; i++ )
		pFeatureMacID[i] ^= pFeatureMacID[HostIDOrderFit[i]];

	pFeatureHostID = ( NR_BYTE* ) & HostID;

	for ( int j = 0; j < 6; j++ )
		pFeatureHostID[j] ^= pFeatureMacID[5-j];

	sprintf ( sHostID, "%02x%02x%02x%02x%02x%02x",
	          pFeatureHostID[0],
	          pFeatureHostID[1],
	          pFeatureHostID[2],
	          pFeatureHostID[3],
	          pFeatureHostID[4],
	          pFeatureHostID[5] );

// 	free(pFeatureHostID);
// 	free(pFeatureMacID);

	return true;
}

NR_EXTERN LICENSE_EXPORT bool NR_API registerLicense(
    NR_PARAM_IN const char * sSoftName,
    NR_PARAM_IN const char * sContactID,
    NR_PARAM_IN const char * sProName,
    NR_PARAM_IN const char * sLicense)
{
	if (sLicense == NULL)
		return false;
	
#ifdef WIN32
	std::locale loc = std::locale::global(std::locale(""));//要打开的文件路径含中文，设置全局locale为本地环境
	//从userprofile中获取序列号
	char sUserFile[512] = {'\0'};
	memset(sUserFile,0,sizeof(sUserFile));
	sprintf(sUserFile, "%s%s", getenv(HOMEFILE), LICENSEFILE);
	ifstream fin(sUserFile);
#else
	char sUserFile[512] = {'\0'};
	memset(sUserFile,0,sizeof(sUserFile));
	if(getenv(HOMEFILE))
		sprintf(sUserFile, "%s", getenv(HOMEFILE));
	else
	{
		if(getenv(USER))
			sprintf(sUserFile,"%s%s","/users/",getenv(USER));
		else if(getenv(USERNAME))
			sprintf(sUserFile,"%s%s","/users/",getenv(USERNAME));
		else
			sprintf(sUserFile,"%s%s","/users/","ems");
	}
	if (access(sUserFile, 0) == -1)
	{
		char sDir[100] = {'\0'};
		sprintf(sDir, "mkdir %s", sUserFile);
		system(sDir);
	}
	
	strcat(sUserFile,"/.fclicense");

	ifstream fin(sUserFile);
	
//	if (access(sNRLicenseDir, 0) == -1)
//	{
//		char sDir[100] = {'\0'};
//		sprintf(sDir, "mkdir %s", sNRLicenseDir);
//		system(sDir);
//	}

//	ifstream fin(sNRLicenseFile);
#endif

	char sLine[101] = {'\0'};
	char sProductName[100] = {'\0'};
	char sList[101*200] = {'\0'};
	char delist[101*200] = {'\0'};
	char enlist[101*200*2+1] = {'\0'};
	char sContactID1[100] = {'\0'};
	char sProName1[100] = {'\0'};
	char sSoftName1[100] = {'\0'};
	memset(sLine,0,sizeof(sLine));
	memset(sProductName,0,sizeof(sProductName));
	memset(sList,0,sizeof(sList));
	memset(delist,0,sizeof(delist));
	memset(enlist,0,sizeof(enlist));
	memset(sContactID1,0,sizeof(sContactID1));
	memset(sProName1,0,sizeof(sProName1));
	memset(sSoftName1,0,sizeof(sSoftName1));
	if(strlen(sSoftName) == 0)
		strcpy(sSoftName1,"Temlic");
	else
		strcpy(sSoftName1,sSoftName);
	bool bFind = false;

	///////////////////////////////////////////////////////////////
	//获取加密字符串
	while (fin.getline(sLine, 101))
	{
		if(strlen(sLine) < 1)
			continue;
		strcat(sList, sLine);
	}
	fin.close();
	fin.clear();
	
	//解密字符串
	decryptStr(sList,delist);
	memset(sList,0,101*200);
	int nl = strlen(delist);
	int nidx = 0;
	int ncurpos = 0;
	char stempstr[101] = {'\0'};
	int ii = 0;
	for (ii = 0; ii < nl; ii++)
	{
		memset(stempstr,0,101);
		getLineFromString(delist+ncurpos,stempstr);
		if (stempstr[0] == '#')
		{
			sscanf(stempstr, "%*s%s", sProductName);
			int noffset = ncurpos+strlen(stempstr);
#ifdef WIN32
			
			if (0 == strcmpi(sProductName, sSoftName1))
			{
				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);
				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);
				sscanf(stempstr, "ContactID=%s", sContactID1);
				if(0 == strcmpi(sContactID1,sContactID))
				{
					memset(stempstr,0,101);
					getLineFromString(delist+noffset,stempstr);
					noffset += strlen(stempstr);
					sscanf(stempstr, "ProjectName=%s", sProName1);
					if(0 == strcmpi(sProName1,sProName))
					{
						memset(stempstr,0,101);
						getLineFromString(delist+noffset,stempstr);
						noffset += strlen(stempstr);
						ncurpos += noffset;
						bFind = true;
						break;
					}
				}
			}
			
#else
			
			if (0 == strcasecmp(sProductName, sSoftName1))
			{
				memset(stempstr,0,101);
				getLineFromString(delist+ncurpos,stempstr);
				noffset += strlen(stempstr);
				memset(stempstr,0,101);
				getLineFromString(delist+ncurpos,stempstr);
				noffset += strlen(stempstr);
				sscanf(stempstr, "ContactID=%s", sContactID1);
				if(0 == strcasecmp(sContactID1,sContactID))
				{
					memset(stempstr,0,101);
					getLineFromString(delist+ncurpos,stempstr);
					noffset += strlen(stempstr);
					sscanf(stempstr, "ProjectName=%s", sProName1);
					if(0 == strcasecmp(sProName1,sProName))
					{
						memset(stempstr,0,101);
						getLineFromString(delist+noffset,stempstr);
						noffset += strlen(stempstr);
						ncurpos += noffset;
						bFind = true;
						break;
					}
				}
			}
#endif
		}
		strcat(sList, stempstr);
		ncurpos += strlen(stempstr);
		ii = ncurpos;
	}
	for (ii = ncurpos; ii < nl; ii++)
	{
		memset(stempstr,0,101);
		getLineFromString(delist+ncurpos,stempstr);
		strcat(sList, stempstr);
		ncurpos += strlen(stempstr);
		ii = ncurpos;
	}
	//////////////////////////////////////////////////////////////

	//保存
	char strDes[100] = {'\0'};
	char strSoft[100] = {'\0'};
	char strContact[100] = {'\0'};
	char strProject[100] = {'\0'};
	char strLicense[100] = {'\0'};
	memset(strDes,0,sizeof(strDes));
	memset(strSoft,0,sizeof(strSoft));
	memset(strContact,0,sizeof(strContact));
	memset(strProject,0,sizeof(strProject));
	memset(strLicense,0,sizeof(strLicense));
	sprintf(strDes, "# %s license", sSoftName1);
	sprintf(strSoft, "ProductName=%s", sSoftName);
	sprintf(strContact, "ContactID=%s", sContactID);
	sprintf(strProject, "ProjectName=%s", sProName);
	sprintf(strLicense, "LicenseKeyExt=%s", sLicense);
//	strcat(sList, "\n");
	strcat(sList, strDes);
	strcat(sList, "\n");
	strcat(sList, strSoft);
	strcat(sList, "\n");
	strcat(sList, strContact);
	strcat(sList, "\n");
	strcat(sList, strProject);
	strcat(sList, "\n");
	strcat(sList, strLicense);
	strcat(sList, "\n");

	//加密
	encryptStr(sList,enlist);
	int nlength = strlen(enlist);
	memset(sList,0,101*200);
	char templist[101] = {'\0'};
	for (int i = 0; i < nlength/100; i++)
	{
		strncpy(templist,enlist+i*100,100);
		strcat(sList,templist);
		strcat(sList, "\n");
	}
	memset(templist,0,101);
	strncpy(templist,enlist+int(nlength/100)*100,nlength%100);
	strcat(sList,templist);
	strcat(sList, "\n");

#ifdef WIN32
	//userprofile
	ofstream foutwin(sUserFile);
	foutwin << sList;
	foutwin.close();
	std::locale::global(loc);//恢复全局locale 
#else
	ofstream foutlinux(sUserFile);

// 	if (access(sNRLicenseDir, 0) == -1)
// 	{
// 		char sDir[100] = {'\0'};
// 		sprintf(sDir, "mkdir %s", sNRLicenseDir);
// 		system(sDir);
// 	}
// 
// 	ofstream foutlinux(sNRLicenseFile);

	foutlinux << sList;
	foutlinux.close();
#endif

	return true;
}


NR_EXTERN LICENSE_EXPORT char * NR_API getErrString(NR_PARAM_IN NR_STATUS status)
{
	static char strErr[100] = {'\0'};

	switch ( status )
	{
	case NR_LICENSE_SUCCESS:
		strcpy(strErr, "Check successfully!");
		break;
	case NR_LICENSE_FAIL:
		strcpy(strErr, "Check failed!");
		break;
	case NR_LICENSE_FULL:
		strcpy(strErr, "Permanent license, check successfully!");
		break;
	case NR_LICENSE_TEMP:
		strcpy(strErr, "Temporary license, check successfully!");
		break;
	case NR_LICENSE_NO_DOG:
		strcpy(strErr, "Can not check hardware dog!");
		break;
	case NR_LICENSE_NO_USER:
		strcpy(strErr, "Can not query the user information!");
		break;
	case NR_LICENSE_ERROR_USER:
		strcpy(strErr, "User checked failed!");
		break;
	case NR_LICENSE_NO_SOFTWARE:
		strcpy(strErr, "Can not query the software information!");
		break;
	case NR_LICENSE_ERROR_SOFTWARE:
		strcpy(strErr, "Software information checked failed!");
		break;
	case NR_LICENSE_NO_RIGHT:
		strcpy(strErr, "Can not query the right information!");
		break;
	case NR_LICENSE_ERROT_RIGHT:
		strcpy(strErr, "Software right checked failed!");
		break;
	case 201:
		strcpy(strErr, "输入参数错误,请确认值的准确性!");
		break;
	case 202:
		strcpy(strErr, "软件标识无效!");
		break;
	case 203:
		strcpy(strErr, "序列号无效!");
		break;
	case 204:
		strcpy(strErr, "硬件狗的特征码无效!");
		break;
	case 205:
		strcpy(strErr, "SP_ERR_INVALID_TOKEN");
		break;
	case 206:
		strcpy(strErr, "序列号为空!");
		break;
	case 207:
		strcpy(strErr, "SP_ERR_INSUFFICIENT_BUFFER");
		break;
	case 208:
		strcpy(strErr, "验证失败!");
		break;
	case 209:
		strcpy(strErr, "不能打开硬件狗驱动!");
		break;
	case 210:
		strcpy(strErr, "访问被拒绝!");
		break;
	case 211:
		strcpy(strErr, "与硬件狗通讯失败!");
		break;
	case 212:
		strcpy(strErr, "与硬件狗通讯失败!");
		break;
	case 213:
		strcpy(strErr, "使用次数限制!");
		break;
	case 214:
		strcpy(strErr, "SP_ERR_MEM_CORRUPT");
		break;
	case 215:
		strcpy(strErr, "SP_ERR_INVALID_FEATURE_TYPE");
		break;
	case 216:
		strcpy(strErr, "SP_ERR_DEVICE_IN_USE");
		break;
	case 217:
		strcpy(strErr, "SP_ERR_INVALID_API_VERSION");
		break;
	case 218:
		strcpy(strErr, "操作超时!");
		break;
	case 219:
		strcpy(strErr, "SP_ERR_INVALID_PACKET");
		break;
	case 220:
		strcpy(strErr, "SP_ERR_KEY_NOT_ACTIVE");
		break;
	case 221:
		strcpy(strErr, "SP_ERR_FUNCTION_NOT_ENABLED");
		break;
	case 222:
		strcpy(strErr, "SP_ERR_DEVICE_RESET");
		break;
	case 223:
		strcpy(strErr, "SP_ERR_TIME_CHEAT");
		break;
	case 224:
		strcpy(strErr, "SP_ERR_INVALID_COMMAND ");
		break;
	case 225:
		strcpy(strErr, "SP_ERR_RESOURCE");
		break;
	case 226:
		strcpy(strErr, "没有检测到硬件狗!");
		break;
	case 227:
		strcpy(strErr, "SP_ERR_DEMO_EXPIRED");
		break;
	case 228:
		strcpy(strErr, "SP_ERR_QUERY_TOO_LONG");
		break;
	case 229:
		strcpy(strErr, "SP_ERR_USER_AUTH_REQUIRED");
		break;
	case 235:
		strcpy(strErr, "");
		break;
	case 701:
		strcpy(strErr, "");
		break;
	case 702:
		strcpy(strErr, "");
		break;
	case 703:
		strcpy(strErr, "");
		break;
	case 704:
		strcpy(strErr, "");
		break;
	case 705:
		strcpy(strErr, "");
		break;
	case 706:
		strcpy(strErr, "");
		break;
	case 707:
		strcpy(strErr, "");
		break;
	case 708:
		strcpy(strErr, "");
		break;
	case 709:
		strcpy(strErr, "");
		break;
	case 710:
		strcpy(strErr, "");
		break;
	case 711:
		strcpy(strErr, "");
		break;
	case 712:
		strcpy(strErr, "");
		break;
	}

	return strErr;
}

//获取CPU的ID号
bool createCPUID(NR_INT64 & cpuID)
{
#ifdef WIN32
	/*
	* 并不是cpu的序列号，是以下三个变量的数据组合
	* wProcessorArchitecture：  指定系统中的中央处理器的体系结构，可以是如下的值：
	*	PROCESSOR_ARCHITECTURE_INTEL
	*	只适用于Windows NT: PROCESSOR_ARCHITECTURE_MIPS
	*	只适用于Windows NT: PROCESSOR_ARCHITECTURE_ALPHA
	*	只适用于Windows NT: PROCESSOR_ARCHITECTURE_PPC
	*	只适用于Windows NT: PROCESSOR_ARCHITECTURE_UNKNOWN
	*
	* wProcessorRevision 三个成员定义处理器的类型。
	*	这个成员的值是下面中的一个：
	*	PROCESSOR_INTEL_386
	*	PROCESSOR_INTEL_486
	*	PROCESSOR_INTEL_PENTIUM
	*	只适用于Windows NT: PROCESSOR_MIPS_R4000
	*	只适用于Windows NT: PROCESSOR_ALPHA_21064
	*
	* wProcessorLevel：
	*	Windows 95: 不使用这个成员。
	*	Windows NT: 指定系统体系结构依赖的处理器级别。
	*/
	SYSTEM_INFO sysInfo;
	memset ( &sysInfo, 0, sizeof ( sysInfo ) );
	GetSystemInfo ( &sysInfo );
	cpuID = sysInfo.wProcessorArchitecture +
	        ( ( ( NR_INT64 ) sysInfo.wProcessorRevision ) << 16 ) +
	        ( ( ( NR_INT64 ) sysInfo.wProcessorLevel ) << 32 );
	
	return true;
#endif
#if defined(__linux__) || defined(__LINUX__)
	/*
	* 能不能获取到cpu的序列号，要看当eax=01h时，edx的第18位的bit是否为1，
	* 如果不为1，则不支持序列号，采用edx+ecx作为cpuid；
	* 如果为1，则支持序列号，ecx与edx中的数据就是cpu的序列号
	* ecx低32位，edx高32位；
	*/
	NR_DWORD dweax, dwebx, dwecx, dwedx, unuseda, unusedb, unusedc, unusedd;
	//执行cpuid，参数为eax=1
	asm ( "cpuid"
      : "=a" ( unuseda ),
	      "=b" ( unusedb ),
	      "=c" ( unusedc ),
	      "=d" ( unusedd )
			      : "a" ( 1 ) );
	int isSupport = unusedd & ( 1 << 18 );//edx的第18位是否为1代表CPU是否存在序列号

	if ( !isSupport )
	{//不支持
		//qDebug()<<"CPU Serial Number not supported!";
		cpuID += ( NR_INT64 ) unusedc;
		cpuID += ( NR_INT64 ) unusedd << 32;
		return true;
	}

	//执行cpuid，参数为eax=3
	asm ( "cpuid"
      : "=a" ( dweax ),
	      "=b" ( dwebx ),
	      "=c" ( dwecx ),
	      "=d" ( dwedx )
			      : "a" ( 3 ) );
	cpuID += ( NR_INT64 ) dwecx;
	cpuID += ( NR_INT64 ) dwedx << 32;
	return true;
#endif
#if defined(__sun) || defined(__solaris__) || defined(__SOLARIS__) || defined(Q_OS_SOLARIS)
	/*
	* 能不能获取到cpu的序列号，要看当eax=01h时，edx的第18位的bit是否为1，
	* 如果不为1，则不支持序列号，采用edx+ecx作为cpuid；
	* 如果为1，则支持序列号，ecx与edx中的数据就是cpu的序列号
	* ecx低32位，edx高32位；
	*/
#if defined(__i386)//x86
	NR_DWORD dweax, dwebx, dwecx, dwedx, unuseda, unusedb, unusedc, unusedd;
	asm volatile (
	    "pushl %%ebx      \n\t" /* save %ebx */
	    "cpuid            \n\t"
	    "movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
	    "popl %%ebx       \n\t" /* restore the old %ebx */
    : "=a" ( unuseda ),
	    "=r" ( unusedb ),
	    "=c" ( unusedc ),
	    "=d" ( unusedd )
			    : "a" ( 1 )
			    : "cc" );
	int isSupport = unusedd & ( 1 << 18 );//edx的第18位是否为1代表CPU是否存在序列号

	if ( !isSupport )
	{//不支持，采用unusedc与unusedd作为cpu的id号使用
		//qDebug()<<"CPU Serial Number not supported!";
		cpuID += ( NR_INT64 ) unusedc;
		cpuID += ( NR_INT64 ) unusedd << 32;
		return true;
	}

	asm volatile (
	    "pushl %%ebx      \n\t" /* save %ebx */
	    "cpuid            \n\t"
	    "movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
	    "popl %%ebx       \n\t" /* restore the old %ebx */
    : "=a" ( dweax ),
	    "=r" ( dwebx ),
	    "=c" ( dwecx ),
	    "=d" ( dwedx )
			    : "a" ( 3 )
			    : "cc" );
	cpuID += ( NR_INT64 ) dwecx;
	cpuID += ( NR_INT64 ) dwedx << 32;
#elif defined (__sparc)//sparc
// #ifdef__ASSEMBLY__
// 	int idcpu;
// 	__asm__ __volatile__(
// 		"sethi %%hi(___b_hard_smp_processor_id), %0\n\t"
// 		"sethi %%hi(boot_cpu_id), %0\n\t"
// 		"ldub [%0 + %%lo(boot_cpu_id)], %0\n\t" :
// 		"=&r" (idcpu));

// 	__asm__ __volatile__(
// 		"rd %%tbr, %0\n\t"
// 		"srl %0, 12, %0\n\t"
// 		"and %0, 3, %0\n\t" 
// 		:"=&r" (idcpu));
//#endif

// 	NR_DWORD dweax, dwebx, dwecx, dwedx, unuseda, unusedb, unusedc, unusedd;
// 	__asm__ volatile (
// 		"pushl %%ebx      \n\t" /* save %ebx */
// 		"cpuid            \n\t"
// 		"movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
// 		"popl %%ebx       \n\t" /* restore the old %ebx */
//		: "=a" ( unuseda ),
//		"=r" ( unusedb ),
//		"=c" ( unusedc ),
//		"=d" ( unusedd )
//		: "a" ( 1 )
//		: "cc" 
//		);
// 	printf("unusedd:%lld\n",unusedd);
// 	int isSupport = unusedd & ( 1 << 18 );//edx的第18位是否为1代表CPU是否存在序列号
// 	
// 	if ( !isSupport )
// 	{//不支持，采用unusedc与unusedd作为cpu的id号使用
// 		//qDebug()<<"CPU Serial Number not supported!";
// 		cpuID += ( NR_INT64 ) unusedc;
// 		cpuID += ( NR_INT64 ) unusedd << 32;
// 		return true;
// 	}
// 	
// 	__asm__ __volatile__ (
// 		"pushl %%ebx      \n\t" /* save %ebx */
// 		"cpuid            \n\t"
// 		"movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
// 		"popl %%ebx       \n\t" /* restore the old %ebx */
// 		: "=a" ( dweax ),
// 		"=r" ( dwebx ),
// 		"=c" ( dwecx ),
// 		"=d" ( dwedx )
// 		: "a" ( 3 )
// 		: "cc" );
// 	cpuID += ( NR_INT64 ) dwecx;
// 	cpuID += ( NR_INT64 ) dwedx << 32;
//	cpuID += 0;
#endif
	return true;
#endif
#if defined(__aix__) || defined(__AIX__) || defined(_AIX)
// 	NR_DWORD dweax,dwebx,dwecx,dwedx,unuseda,unusedb,unusedc,unusedd;
// 	asm volatile(
// 		"pushl %%ebx      \n\t" /* save %ebx */
// 		"cpuid            \n\t"
// 		"movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
// 		"popl %%ebx       \n\t" /* restore the old %ebx */
// 		:"=a"(unuseda),
// 		"=b"(unusedb),
// 		"=c"(unusedc),
// 		"=d"(unusedd)
// 		:"a"(1)
// 		:"cc");
// 	int isSupport = unusedd & (1<<18);//edx的第18位是否为1代表CPU是否存在序列号
// 	if (!isSupport)
// 	{//不支持
// 		//qDebug()<<"CPU Serial Number not supported!";
// 		cpuID += (NR_INT64)unusedc;
// 		cpuID += (NR_INT64)unusedd<<32;
// 		return true;
// 	}
// 	asm volatile(
// 		"pushl %%ebx      \n\t" /* save %ebx */
// 		"cpuid            \n\t"
// 		"movl %%ebx, %1   \n\t" /* save what cpuid just put in %ebx */
// 		"popl %%ebx       \n\t" /* restore the old %ebx */
// 		:"=a"(dweax),
// 		"=r"(dwebx),
// 		"=c"(dwecx),
// 		"=d"(dwedx)
// 		:"a"(3)
// 		:"cc");
// 	cpuID += (NR_INT64)dwecx;
// 	cpuID += (NR_INT64)dwedx<<32;
	return false;
#endif
#if defined(Q_OS_HPUX) || defined(__HPUX) || defined(__hpux)
	cpuID = 0;
	return false;
#endif
	return false;
}
//获取硬盘的ID号
bool createHardID(NR_INT64 & HardID)
{
#ifdef WIN32
	// get disk driver C: information
	char szVolumename[64], szFileSystemNameBuffer[32];
	NR_DWORD dwVolumeSerialNumber = 0;
	NR_DWORD dwMaximumComponentLength = 0;
	NR_DWORD dwFileSystemFlags = 0;
	GetVolumeInformationA ( "C:\\", szVolumename, 64,
	                        &dwVolumeSerialNumber, &dwMaximumComponentLength,
	                        &dwFileSystemFlags, szFileSystemNameBuffer, 32 );
	HardID += ( ( NR_INT64 ) dwVolumeSerialNumber ) << 8;
	return true;
#endif
#if defined(__linux__) || defined(__LINUX__)

	int fd;
	struct hd_driveid hid;
	FILE *fp;
	DIR *dir_name;
	struct dirent *Device;
	char line[0x100], *disk, *root, *p, *local;
	fp = fopen ( "/etc/mtab", "r" );
	fd = -1;

	if ( fp != NULL )
	{
		while ( fgets ( line, sizeof line, fp ) != NULL )
		{
			disk = strtok ( line, " " );

			if ( disk == NULL )
				continue;

			root = strtok ( NULL, " " );

			if ( root == NULL )
				continue;

			if ( strcmp ( root, "/" ) == 0 )
			{
				for ( p = disk + strlen ( disk ) - 1; p >= disk && isdigit ( *p ); p -- )
					*p = 0;

				fd = open ( disk, O_RDONLY );
				break;
			}
		}
	}

	fclose ( fp );

	if ( fd < 0 )
	{//如果没有找到硬盘，则到/dev目录下找hda或是sda
//		perror ( disk );
		close ( fd );
		disk = 0;
		dir_name = opendir ( "/dev/" );

		while ( ( Device = readdir ( dir_name ) ) )
		{
			if ( !strcmp ( Device->d_name, "sda1" ) )
				disk = "/dev/sda";
			else if ( !strcmp ( Device->d_name, "hda1" ) )
				disk = "/dev/hda";
		}

		closedir ( dir_name );
		fd = open ( disk, O_RDONLY );
	}

	if ( fd < 0 )
	{
//		perror ( disk );
		close ( fd );
		HardID = 0;
		return false;
	}

	if ( ioctl ( fd, HDIO_GET_IDENTITY, &hid ) != -1 )
	{
		char *pserial;
		//char strno[100] = {'\0'};
		sprintf ( pserial, "%s", hid.serial_no );
		//strno.remove ( " " );
		//char *pserial = strno.toAscii().data();
		HardID = ( NR_INT64 ) pserial[0];
		HardID += ( NR_INT64 ) pserial[1] << 8;
		HardID += ( NR_INT64 ) pserial[2] << 16;
		HardID += ( NR_INT64 ) pserial[3] << 24;
		HardID += ( NR_INT64 ) pserial[4] << 32;
		HardID += ( NR_INT64 ) pserial[5] << 40;
		HardID += ( NR_INT64 ) pserial[6] << 48;
		HardID += ( NR_INT64 ) pserial[7] << 56;
		close ( fd );
		return true;
	}

	//如果还没有找到，则下面的方式找
	static unsigned char args[512] = { 0 };
//	unsigned short *id = (void *)(args);
	unsigned char *data = ( args );
	unsigned int data_bytes = 512;
	unsigned char cdb[SG_ATA_16_LEN] = { 0 };
	unsigned char sb[32], *desc;
	unsigned char ata_status, ata_error;
	struct sg_io_hdr io_hdr;
	//cmdp
	cdb[ 0] = SG_ATA_16;
	cdb[ 1] = SG_ATA_PROTO_PIO_IN;
	cdb[ 2] = SG_CDB2_CHECK_COND;
	cdb[2] |= SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS;
	cdb[2] |= SG_CDB2_TDIR_FROM_DEV;
	cdb[13] = ATA_USING_LBA;
	cdb[14] = ATA_OP_IDENTIFY;
	//sdp
	memset ( & ( sb[0] ), 0, sizeof ( sb ) );
	//sg_io_hdr
	memset ( &io_hdr, 0, sizeof ( struct sg_io_hdr ) );
	io_hdr.interface_id    = 'S';
	io_hdr.cmd_len        = SG_ATA_16_LEN;
	io_hdr.mx_sb_len    = sizeof ( sb );
	io_hdr.dxfer_direction    = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len    = data_bytes;
	io_hdr.dxferp        = data;
	io_hdr.cmdp        = cdb;
	io_hdr.sbp        = sb;
	io_hdr.timeout        = 10000; /* msecs */

	//ioctl
	if ( ioctl ( fd, SG_IO, &io_hdr ) == -1 )
	{
		fprintf ( stderr, "SG_IO ioctl not supported\n" );
		close ( fd );
		return false;    /* SG_IO not supported */
	}

	//g_io_hdr
	if ( io_hdr.host_status || io_hdr.driver_status != SG_DRIVER_SENSE
	        || ( io_hdr.status && io_hdr.status != SG_CHECK_CONDITION ) )
	{
		errno = EIO;
		close ( fd );
		HardID = 0;
		return false;
	}

	//dp
	if ( sb[0] != 0x72 || sb[7] < 14 )
	{
		errno = EIO;
		close ( fd );
		HardID = 0;
		return false;
	}

	desc = sb + 8;

	if ( desc[0] != 9 || desc[1] < 12 )
	{
		errno = EIO;
		close ( fd );
		HardID = 0;
		return false;
	}

	ata_error = desc[3];
	ata_status = desc[13];

	if ( ata_status & 0x01 )
	{/* ERR_STAT */
		errno = EIO;
		close ( fd );
		HardID = 0;
		return false;
	}

	char *pHard;
	//char strhard[100] = {'\0'};
	sprintf ( pHard, "%s", data );
	//strhard = strhard.remove ( " " );
	//char *pHard = strhard.toAscii().data();
	HardID += ( NR_INT64 ) pHard[0];
	HardID += ( NR_INT64 ) pHard[1] << 8;
	HardID += ( NR_INT64 ) pHard[2] << 16;
	HardID += ( NR_INT64 ) pHard[3] << 24;
	HardID += ( NR_INT64 ) pHard[4] << 32;
	HardID += ( NR_INT64 ) pHard[5] << 40;
	HardID += ( NR_INT64 ) pHard[6] << 48;
	HardID += ( NR_INT64 ) pHard[7] << 56;
	close ( fd );
	return true;
#endif
#if defined(__sun) || defined(__solaris__) || defined(__SOLARIS__) || defined(Q_OS_SOLARIS)
	char  buf[256] = {'\0'};
	char svendor[256] = {'\0'};
	char sproduct[256] = {'\0'};
	char srevision[256] = {'\0'};
	char sserial[256] = {'\0'};
	char splitch = ' ';
	sprintf(buf,"iostat -E | grep 'Vendor'");
    FILE* fp = popen(buf,"r") ;
	if(fp == NULL)
		return false ;
    while(fgets(buf,256,fp)) {
        if(strlen(buf) > 0)
			buf[strlen(buf)-1] = '\0' ;
// 		printf("%s\n",buf);
// 		sscanf(buf, "%*s%s",svendor);
// 		sscanf(buf, "%*s%*s%*s%s", sproduct);
// 		sscanf(buf, "%*s%*s%*s%*s%*s%s",srevision);
// 		sscanf(buf, "%*s%*s%*s%*s%*s%*s%*s%*s%*s%s",sserial);

		char * ssv = getStrStr(buf,"Vendor:");
		char * ssp = getStrStr(buf,"Product:");
		char * ssr = getStrStr(buf,"Revision:");
		char * sse = getStrStr(buf,"No:");
		strncpy(svendor, ssv,256);
		strncpy(sproduct, ssp,256);
		strncpy(srevision, ssr,256);
		strncpy(sserial, sse,256);
		free(sse);
		free(ssr);
		free(ssp);
		free(ssv);

// 		printf("vendor:%s\n",svendor);
// 		printf("product:%s\n",sproduct);
// 		printf("revision:%s\n",srevision);
// 		printf("serial:%s\n",sserial);

		int nvendor = strlen(svendor);
		int nproduct = strlen(sproduct);
		int nrevision = strlen(srevision);
		int nserial = strlen(sserial);

// 		printf("vendor:%d\n",nvendor);
// 		printf("product:%d\n",nproduct);
// 		printf("revision:%d\n",nrevision);
// 		printf("serial:%d\n",nserial);

#if defined(LOG_LOG)
		char slogcontent[1024] = {'\0'};
		char slogtemp[512] = {'\0'};
		char sLogFile[512] = {'\0'};
		sprintf(sLogFile, "%s%s", getenv(HOMEFILE), "/nrlicense.log");
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"%s%s,","svendor:",svendor);
		strcat(slogcontent,slogtemp);
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"%s%s,","sproduct:",sproduct);
		strcat(slogcontent,slogtemp);
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"%s%s,","srevision:",srevision);
		strcat(slogcontent,slogtemp);
		memset(slogtemp,0,sizeof(slogtemp));
		sprintf(slogtemp,"%s%s,","sserial:",sserial);
		strcat(slogcontent,slogtemp);
		ofstream flogout(sLogFile,ios::out|ios::app);
		flogout<<slogcontent;
		flogout.close();
#endif

		for ( int i = nvendor - 1; i >= 0; --i )
		{
			if ( nvendor - 1 - i >= 8 || i < 0 )
				break;
			
			HardID += ( ( NR_INT64 ) svendor[i] ) << ( ( nvendor - i - 1 ) * 8 );
		}
//		printf("HardID:%x\n",HardID);
		for ( int i = nproduct - 1; i >= 0; --i )
		{
			if ( nproduct - 1 - i >= 8 || i < 0 )
				break;
			
			HardID += ( ( NR_INT64 ) sproduct[i] ) << ( ( nproduct - i - 1 ) * 8 );
		}
//		printf("HardID:%x\n",HardID);
		for ( int i = nrevision - 1; i >= 0; --i )
		{
			if ( nrevision - 1 - i >= 8 || i < 0 )
				break;
			
			HardID += ( ( NR_INT64 ) srevision[i] ) << ( ( nrevision - i - 1 ) * 8 );
		}
//		printf("HardID:%x\n",HardID);
		for ( int i = nserial - 1; i >= 0; --i )
		{
			if ( nserial - 1 - i >= 8 || i < 0 )
				break;
			
			HardID += ( ( NR_INT64 ) sserial[i] ) << ( ( nserial - i - 1 ) * 8 );
		}
//		printf("HardID:%x\n",HardID);

		break;
    }
    pclose(fp) ;
	return true;
#endif
#if defined(__aix__) || defined(__AIX__) || defined(_AIX)

	char  buf[256] = {'\0'};
	char ssystemmodel[256] = {'\0'};
	char sserialnumber[256] = {'\0'};
	char stemp[256] = {'\0'};
	sprintf(buf,"uname -mu");
	FILE* fp = popen(buf,"r") ;
	if(fp == NULL)
		return false;
	int idx = 0;
	while(fgets(buf,256,fp)) {
		if(strlen(buf) > 0)
			buf[strlen(buf)-1] = '\0';
		idx++;
		sscanf(buf,"%[^ ]",stemp);
		memcpy(ssystemmodel,stemp,256);
		//sscanf(stemp,"%*[^,],%s",ssystemmodel);
		sscanf(buf,"%*[^ ]%s",stemp);
		sscanf(stemp,"%*[^,],%s",sserialnumber);
		break;
	}

	int nsystem = strlen(ssystemmodel);
	int nserial = strlen(sserialnumber);
	for ( int i = nsystem - 1; i >= 0; --i )
	{
		HardID += ( ( NR_INT64 ) ssystemmodel[i] ) << ( ( nsystem - i - 1 ) * 8 );
	}
	for ( int i = nserial - 1; i >= 0; --i )
	{
		HardID += ( ( NR_INT64 ) sserialnumber[i] ) << ( ( nserial - i - 1 ) * 8 );
	}

	pclose(fp);
	return true;
#endif

#if defined(Q_OS_HPUX) || defined(__HPUX) || defined(__hpux)
	
	char buf[256] = {'\0'};
	char sidentification[256] = {'\0'};
	char smachineserial[256] = {'\0'};
	char stemp[256] = {'\0'};
	int idx = 0;
	sprintf(buf,"uname -i");
	FILE* fp = popen(buf,"r") ;
	if(fp != NULL)
	{
		while(fgets(buf,256,fp)) {
			if(strlen(buf) > 0)
				buf[strlen(buf)-1] = '\0';
			idx++;
			memcpy(sidentification,buf,256);
			break;
		}
	}

	sprintf(buf,"getconf MACHINE_SERIAL");
	fp = popen(buf,"r") ;
	if(fp != NULL)
	{
		idx = 0;
		while(fgets(buf,256,fp)) {
			if(strlen(buf) > 0)
				buf[strlen(buf)-1] = '\0';
			idx++;
			memcpy(smachineserial,buf,256);
			break;
		}
	}

// 	printf("%s\n",sidentification);
// 	printf("%s\n",smachineserial);

	int nident = strlen(sidentification);
	int nserial = strlen(smachineserial);
	for ( int i = nident - 1; i >= 0; --i )
	{
		HardID += ( ( NR_INT64 ) sidentification[i] ) << ( ( nident - i - 1 ) * 8 );
	}
	for ( int i = nserial - 1; i >= 0; --i )
	{
		HardID += ( ( NR_INT64 ) smachineserial[i] ) << ( ( nserial - i - 1 ) * 8 );
	}

	pclose(fp);

	return true;
#endif
	return false;
}

//获取网卡的MAC地址
NR_EXTERN LICENSE_EXPORT bool createMACID(NR_PARAM_OUT NR_INT64 & macID)
{
	macID = 123456789;
	return true;
//windows 系统
#ifdef WIN32

	ASTAT Adapter;
	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM   lenum;
	int      i;
	bool	 bFind = false;
	memset ( &Ncb, 0, sizeof ( Ncb ) );
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = ( UCHAR * ) & lenum;
	Ncb.ncb_length = sizeof ( lenum );
	uRetCode = Netbios ( &Ncb );

	for ( i = 0; i < lenum.length ; i++ )
	{
		memset ( &Ncb, 0, sizeof ( Ncb ) );
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];
		uRetCode = Netbios ( &Ncb );
		memset ( &Ncb, 0, sizeof ( Ncb ) );
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];
		strcpy ( ( char* ) Ncb.ncb_callname,  "*               " );
		Ncb.ncb_buffer = ( BYTE * ) & Adapter;
		Ncb.ncb_length = sizeof ( Adapter );
		uRetCode = Netbios ( &Ncb );

		if ( Adapter.adapt.adapter_address[0] == 0 &&
		        Adapter.adapt.adapter_address[1] == 0 &&
		        Adapter.adapt.adapter_address[2] == 0 &&
		        Adapter.adapt.adapter_address[3] == 0 &&
		        Adapter.adapt.adapter_address[4] == 0 &&
		        Adapter.adapt.adapter_address[5] == 0 )
			continue;

		if ( uRetCode == 0 )
		{
			macID += ( NR_INT64 ) Adapter.adapt.adapter_address[0];
			macID += ( NR_INT64 ) Adapter.adapt.adapter_address[1] << 8;
			macID += ( NR_INT64 ) Adapter.adapt.adapter_address[2] << 16;
			macID += ( NR_INT64 ) Adapter.adapt.adapter_address[3] << 24;
			macID += ( NR_INT64 ) Adapter.adapt.adapter_address[4] << 32;
			macID += ( NR_INT64 ) Adapter.adapt.adapter_address[5] << 40;
			return true;
		}
	}

	return false;
#endif
//linux系统
#if defined(__linux__) || defined(__LINUX__)
	register int fd, intrface, retn = 0;
	struct ifreq buf[MAXINTERFACES];
	struct arpreq arp;
	struct ifconf ifc;

	if ( ( fd = socket ( AF_INET, SOCK_DGRAM, 0 ) ) >= 0 )
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = ( caddr_t ) buf;

		if ( !ioctl ( fd, SIOCGIFCONF, ( char * ) &ifc ) )
		{
			//获取接口信息
			intrface = ifc.ifc_len / sizeof ( struct ifreq );

			//根据借口信息循环获取设备IP和MAC地址
			while ( intrface-- > 0 )
			{
				//获取设备名称
//				qDebug()<<"intrface: "<<intrface<<",device: "<<buf[intrface].ifr_name;
				//判断网卡类型
//				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface])))
//				{
//					if (buf[intrface].ifr_flags & IFF_PROMISC)
//					{
//						qDebug()<<"the interface is PROMISC";
//						retn++;
//					}
//				}
//				else
//				{
//					qDebug()<<"cpm: ioctl device "<<buf[intrface].ifr_name;
//				}
				//判断网卡状态
//				if (buf[intrface].ifr_flags & IFF_UP)
//				{
//					qDebug()<<"the interface status is UP";
//				}
//				else
//				{
//					qDebug()<<"the interface status is DOWN";
//				}
				//获取当前网卡的IP地址
// 				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
// 				{
// 					qDebug()<<"IP address is: "<<inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr);
// 				}
//				else
//				{
//					qDebug()<<"cpm: ioctl device "<<buf[intrface].ifr_name;
//				}
				/* this section can't get Hardware Address,I don't know whether the reason is module driver*/
				if ( ! ( ioctl ( fd, SIOCGIFHWADDR, ( char * ) &buf[intrface] ) ) )
				{
					macID += ( ( NR_INT64 ) buf[intrface].ifr_hwaddr.sa_data[0] ) << 40;
					macID += ( ( NR_INT64 ) buf[intrface].ifr_hwaddr.sa_data[1] ) << 32;
					macID += ( ( NR_INT64 ) buf[intrface].ifr_hwaddr.sa_data[2] ) << 24;
					macID += ( ( NR_INT64 ) buf[intrface].ifr_hwaddr.sa_data[3] ) << 16;
					macID += ( ( NR_INT64 ) buf[intrface].ifr_hwaddr.sa_data[4] ) << 8;
					macID += ( NR_INT64 ) buf[intrface].ifr_hwaddr.sa_data[5];
					close ( fd );
					return true;
				}

//				else
//				{
//					qDebug()<<"cpm: ioctl device "<<buf[intrface].ifr_name;
//				}
			} //while
		}
		else
		{
			perror ( "cpm: ioctl" );
		}
	}
	else
	{
		perror ( "cpm: socket" );
	}

	close ( fd );
	macID = 0;
	return false;
#endif
#if defined(__sun) || defined(__solaris__) || defined(__SOLARIS__) || defined(Q_OS_SOLARIS)

	register int fd, intrface;
	struct ifreq buf[MAXINTERFACES];
	struct arpreq arp;
	struct ifconf ifc;

	if ( ( fd = socket ( AF_INET, SOCK_DGRAM, 0 ) ) >= 0 )
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = ( caddr_t ) buf;

		if ( !ioctl ( fd, SIOCGIFCONF, ( char * ) &ifc ) )
		{
			//获取接口信息
			intrface = ifc.ifc_len / sizeof ( struct ifreq );

			//根据借口信息循环获取设备IP和MAC地址
			while ( intrface-- > 0 )
			{
				//获取设备名称
//				qDebug()<<"intrface: "<<intrface<<",device: "<<buf[intrface].ifr_name;
				//判断网卡类型
//				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface])))
//				{
//					if (buf[intrface].ifr_flags & IFF_PROMISC)
//					{
//						qDebug()<<"the interface is PROMISC";
//						retn++;
//					}
//				}
//				else
//				{
//					qDebug()<<"cpm: ioctl device "<<buf[intrface].ifr_name;
//				}
				//判断网卡状态
//				if (buf[intrface].ifr_flags & IFF_UP)
//				{
//					qDebug()<<"the interface status is UP";
//				}
//				else
//				{
//					qDebug()<<"the interface status is DOWN";
//				}
				//获取当前网卡的IP地址
// 				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
// 				{
// 					qDebug()<<"IP address is: "<<inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr);
// 				}
//				else
//				{
//					qDebug()<<"cpm: ioctl device "<<buf[intrface].ifr_name;
//				}
				/* this section can't get Hardware Address,I don't know whether the reason is module driver*/
				//获取MAC地址
				arp.arp_pa.sa_family = AF_INET;
				arp.arp_ha.sa_family = AF_INET;
				( ( struct sockaddr_in* ) &arp.arp_pa )->sin_addr.s_addr = ( ( struct sockaddr_in* ) ( &buf[intrface].ifr_addr ) )->sin_addr.s_addr;

				if ( ! ( ioctl ( fd, SIOCGARP, ( char * ) &arp ) ) )
				{
					macID += ( ( NR_INT64 ) arp.arp_ha.sa_data[0] ) << 40;
					macID += ( ( NR_INT64 ) arp.arp_ha.sa_data[1] ) << 32;
					macID += ( ( NR_INT64 ) arp.arp_ha.sa_data[2] ) << 24;
					macID += ( ( NR_INT64 ) arp.arp_ha.sa_data[3] ) << 16;
					macID += ( ( NR_INT64 ) arp.arp_ha.sa_data[4] ) << 8;
					macID += ( NR_INT64 ) arp.arp_ha.sa_data[5];

					close ( fd );
					return true;
				}
			} //while
		}
		else
		{
			perror ( "cpm: ioctl" );
		}
	}
	else
	{
		perror ( "cpm: socket" );
	}

	close ( fd );
	macID = 0;
	return false;
#endif
//aix系统
#if defined(__aix__) || defined(__AIX__) || defined(_AIX)
	int size;
	struct kinfo_ndd *nddp;
	char addr[6];
	size = getkerninfo ( KINFO_NDD, 0, 0, 0 );

	if ( size <= 0 )
	{
		macID = 0;
		return false;
	}

	nddp = ( struct kinfo_ndd * ) malloc ( size );

	if ( !nddp )
	{
		macID = 0;
		return false;
	}

	if ( getkerninfo ( KINFO_NDD, nddp, &size, 0 ) < 0 )
	{
		free ( nddp );
		macID = 0;
		return false;
	}

	macID += ( ( NR_INT64 ) nddp->ndd_addr[0] ) << 40;
	macID += ( ( NR_INT64 ) nddp->ndd_addr[1] ) << 32;
	macID += ( ( NR_INT64 ) nddp->ndd_addr[2] ) << 24;
	macID += ( ( NR_INT64 ) nddp->ndd_addr[3] ) << 16;
	macID += ( ( NR_INT64 ) nddp->ndd_addr[4] ) << 8;
	macID += ( ( NR_INT64 ) nddp->ndd_addr[5] );

	free ( nddp );
	return true;
#endif

#if defined(Q_OS_HPUX) || defined(__HPUX) || defined(__hpux)

	char buf[256] = {'\0'};
	char sserial[256] = {'\0'};
	char stemp[256] = {'\0'};
	sprintf(buf,"lanscan -a");
    FILE* fp = popen(buf,"r") ;
	if(fp != NULL)
	{
		while(fgets(buf,256,fp)) {
			if(strlen(buf) > 0)
				buf[strlen(buf)-1] = '\0' ;
			sscanf(buf,"%*[^x]x%s",stemp);
			memcpy(sserial,stemp,256);
			break;
		}
	}
	//printf("mac serial:%s\n",sserial);

	int nserial = strlen(sserial);

	for ( int i = nserial - 1; i >= 0; --i )
	{
		macID += ( ( NR_INT64 ) sserial[i] ) << ( ( nserial - i - 1 ) * 8 );
	}

	pclose(fp);

	return  true;
#endif
	return false;
}

bool formatLicense(char * dest, char * src)
{
// 	if(strlen(dest) != strlen(src) || strlen(dest) != NR_LICENSELEN+1)
// 		return false;
	char * p;
	char * q;
	p = src;
	q = dest;
	memcpy(q, p, 4);
	dest[4] = '-';
	p = &src[4];
	q = &dest[5];
	memcpy(q, p, 4);
	dest[9] = '-';
	p = &src[8];
	q = &dest[10];
	memcpy(q, p, 4);
	dest[14] = '-';
	p = &src[12];
	q = &dest[15];
	memcpy(q, p, 4);
	dest[19] = '-';
	p = &src[16];
	q = &dest[20];
	memcpy(q,p,4);
	p = 0;
	q = 0;
	return true;
}

NR_EXTERN LICENSE_EXPORT bool NR_API getLicenseInfo(
    NR_PARAM_IN const char * sSoftName,
    NR_PARAM_IN const char * sContactID,
    NR_PARAM_IN const char * sProName,
	NR_PARAM_OUT char * sLicense)
{
	char sList[101*200] = {'\0'};
	char delist[101*200] = {'\0'};
	memset(sList,0,sizeof(sList));
	memset(delist,0,sizeof(delist));
#ifdef WIN32
	//从userprofile中获取序列号
	std::locale loc = std::locale::global(std::locale(""));//要打开的文件路径含中文，设置全局locale为本地环境 
	char sUserFile[512] = {'\0'};
	memset(sUserFile,0,sizeof(sUserFile));
	char * sHome = getenv(HOMEFILE);
	if(NULL == sHome)
	{
		std::locale::global(loc);//恢复全局locale 
		return false;
	}
	sprintf(sUserFile, "%s%s", sHome, LICENSEFILE);

	ifstream finwin(sUserFile);
	char sLine[101] = {'\0'};
	char sProductName[100] = {'\0'};
	char sContactID1[100] = {'\0'};
	char sProName1[100] = {'\0'};
	char sSoftName1[100] = {'\0'};
	memset(sLine,0,sizeof(sLine));
	memset(sProductName,0,sizeof(sProductName));
	memset(sContactID1,0,sizeof(sContactID1));
	memset(sProName1,0,sizeof(sProName1));
	memset(sSoftName1,0,sizeof(sSoftName1));
	if(strlen(sSoftName) == 0)
		strcpy(sSoftName1,"Temlic");
	else
		strcpy(sSoftName1,sSoftName);

	///////////////////////////////////////////////////////////////
	//获取加密字符串
	while (finwin.getline(sLine, 101))
	{
		if(strlen(sLine) < 1)
			continue;
		strcat(sList, sLine);
	}
	finwin.close();
	finwin.clear();
	std::locale::global(loc);//恢复全局locale 
	//解密字符串
	decryptStr(sList,delist);
	memset(sList,0,101*200);
	int nl = strlen(delist);
	int nidx = 0;
	int ncurpos = 0;
	char stempstr[101] = {'\0'};
	int ii = 0;
	for (ii = 0; ii < nl; ii++)
	{
		memset(stempstr,0,101);
		getLineFromString(delist+ncurpos,stempstr);
		if (stempstr[0] == '#')
		{
			sscanf(stempstr, "%*s%s", sProductName);
			int noffset = ncurpos+strlen(stempstr);
			if (0 == strcmpi(sProductName, sSoftName1))
			{
				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);

				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);
				sscanf(stempstr, "ContactID=%s", sContactID);

				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);
				sscanf(stempstr, "ProjectName=%s", sProName);

				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				sscanf(stempstr, "LicenseKeyExt=%s", sLicense);

				noffset += strlen(stempstr);
				ncurpos += noffset;
				break;
			}
		}
		if(strlen(stempstr) <= 0)
		{
			return false;
		}
		ncurpos += strlen(stempstr);
		ii = ncurpos;
	}
	//////////////////////////////////////////////////////////////
#else
	char sUserFile[512] = {'\0'};
	memset(sUserFile,0,sizeof(sUserFile));
	if(getenv(HOMEFILE))
		sprintf(sUserFile, "%s", getenv(HOMEFILE));
	else
	{
		if(getenv(USER))
			sprintf(sUserFile,"%s%s","/users/",getenv(USER));
		else if(getenv(USERNAME))
			sprintf(sUserFile,"%s%s","/users/",getenv(USERNAME));
		else
			sprintf(sUserFile,"%s%s","/users/","ems");
	}
	if (access(sUserFile, 0) == -1)
	{
		char sDir[100] = {'\0'};
		sprintf(sDir, "mkdir %s", sUserFile);
		system(sDir);
	}

	strcat(sUserFile,"/.fclicense");

	ifstream fin(sUserFile);
//	ifstream fin(sNRLicenseFile);
	char sLine[101] = {'\0'};
	char sProductName[100] = {'\0'};
	char sContactID1[100] = {'\0'};
	char sProName1[100] = {'\0'};
	char sSoftName1[100] = {'\0'};
	memset(sLine,0,sizeof(sLine));
	memset(sProductName,0,sizeof(sProductName));
	memset(sContactID1,0,sizeof(sContactID1));
	memset(sProName1,0,sizeof(sProName1));
	memset(sSoftName1,0,sizeof(sSoftName1));
	if(strlen(sSoftName) == 0)
		strcpy(sSoftName1,"Temlic");
	else
		strcpy(sSoftName1,sSoftName);

	///////////////////////////////////////////////////////////////
	//获取加密字符串
	while (fin.getline(sLine, 101))
	{
		if(strlen(sLine) < 1)
			continue;
		strcat(sList, sLine);
	}
	fin.close();
	fin.clear();
	
	//解密字符串
	decryptStr(sList,delist);
	memset(sList,0,101*200);
	int nl = strlen(delist);
	int nidx = 0;
	int ncurpos = 0;
	char stempstr[101] = {'\0'};
	int ii = 0;
	for (ii = 0; ii < nl; ii++)
	{
		memset(stempstr,0,101);
		getLineFromString(delist+ncurpos,stempstr);
		if (stempstr[0] == '#')
		{
			sscanf(stempstr, "%*s%s", sProductName);
			int noffset = ncurpos+strlen(stempstr);
			
			if (0 == strcasecmp(sProductName, sSoftName1))
			{
				
				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);
				
				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);
				sscanf(stempstr, "ContactID=%s", sContactID);
				
				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				noffset += strlen(stempstr);
				sscanf(stempstr, "ProjectName=%s", sProName);
				
				memset(stempstr,0,101);
				getLineFromString(delist+noffset,stempstr);
				sscanf(stempstr, "LicenseKeyExt=%s", sLicense);
				
				noffset += strlen(stempstr);
				ncurpos += noffset;
				break;
			}
		}
		if(strlen(stempstr) <= 0)
		{
			return false;
		}
		ncurpos += strlen(stempstr);
		ii = ncurpos;
	}
	
#endif

	return true;
}

bool createLicenseOld(NR_PARAM_IN const char * sHostID, NR_PARAM_OUT char * sLicense)
{
	memset(sLicense,0,NR_LICENSELEN_OLD);
	char pLicense[NR_LICENSELEN_OLD+1];
	
	pLicense[0]=0;
	int i;
	for ( i=0;i<NR_LICENSELEN_OLD;i++)
	{
		pLicense[i]=sHostID[i%NR_HOSTIDLEN];
	}	
	
	for ( i=0;i<NR_LICENSELEN_OLD;i++)
	{
		pLicense[i]=sHostID[hostToLicense[i]%NR_HOSTIDLEN];
	}
	
	for ( i=0;i<NR_LICENSELEN_OLD;i++)
	{
		pLicense[i] = CharListOld[(pLicense[i]+i)%36];
		
		if(pLicense[i]>='0' && pLicense[i]<='9')
		{
			pLicense[i]=CharListOld[(pLicense[i]+3*i)%36];
		}
		if(pLicense[i]>='a' && pLicense[i]<='z')
		{
			pLicense[i]=CharListOld[((pLicense[i])%36+2*i)%36];
		}
	}
	
	for ( i=0;i<NR_LICENSELEN_OLD;i++)
	{
		if(pLicense[i]>='0' && pLicense[i]<='9')
		{
			pLicense[i]=CharListOld[(pLicense[i]+pLicense[(i+1)% NR_LICENSELEN_OLD]+pLicense[(i+2)% NR_LICENSELEN_OLD]+pLicense[(i+3)% NR_LICENSELEN_OLD])%36];
		}
		
		if(pLicense[i]>='a' && pLicense[i]<='z')
		{
			pLicense[i]=CharListOld[((pLicense[i])%36+1+pLicense[(i+6)% NR_LICENSELEN_OLD])%36];
		}
	}
	pLicense[NR_LICENSELEN_OLD]=0;
	strcpy(sLicense,pLicense);
	return true;
}

bool createTempLicenseOld(NR_PARAM_IN const int nYear, NR_PARAM_IN const int nMon, NR_PARAM_IN const int nDay, NR_PARAM_OUT char * tempLicense)
{
	memset(tempLicense, 0, NR_TEMPLICENSELEN_OLD + 1);
	sprintf(tempLicense,"Temlic%04d-%02d-%02d",nYear,nMon,nDay);
	
	char szLicense[NR_LICENSELEN_OLD+1] = {'\0'};
	char szHostid[NR_HOSTIDLEN+1] = {'\0'};
	int i = 0;

	for(i=0;i<NR_HOSTIDLEN;i++)
		szHostid[i]=tempLicense[i+4];
	
	szHostid[6]=tempLicense[1];
	szHostid[9]=tempLicense[2];
	
	if(!createLicenseOld(szHostid, szLicense))
		return false;
	
	for(i=0;i<NR_HOSTIDLEN;i++)
		szHostid[i]=szLicense[i];
	
	for(i=0;i<4;i++)
		szHostid[i]=tempLicense[i];
	
	if(!createLicenseOld(szHostid,szLicense))
		return false;
	
	for(i=0;i<NR_LICENSELEN_OLD;i++)
		tempLicense[i+NR_LICENSELEN_OLD]=szLicense[i];

	return true;
}

//加密函数
NR_EXTERN LICENSE_EXPORT bool NR_API encryptStr(NR_PARAM_IN const char * strSrc, char * strDest)
{
	int nlength = strlen(strSrc);
//	char * strResult = (char*)malloc(nlength*2+1);
	NR_WORD keyKey = KEY;

	memset(strDest,0,sizeof(strDest));

	for(int i = 0; i < nlength; i++) // 对加密结果进行转换
	{
		int j = strSrc[i]; // 提取字符
		// 将字符转换为两个字母保存
		strDest[2*i] = C3+j/C4;
		strDest[2*i+1] = C3+j%C4;
	}
	return true;
}

// 解密函数
NR_EXTERN LICENSE_EXPORT bool NR_API decryptStr(NR_PARAM_IN const char* strSrc, char * strDest)
{
	int nlength = strlen(strSrc);
//	char * strResult = (char*)malloc(nlength/2+1);
	NR_WORD keyKey = KEY;

	memset(strDest,0,sizeof(strDest));

	for(int i=0; i < nlength/2; i++) // 将字符串两个字母一组进行处理
	{
		int j = ((strSrc[2*i]-C3)*C4);//相应的，解密处要改为相同的数
		j = j + strSrc[2*i+1]-C3;
		strDest[i] = j;//还原字符串
	}

	return true;
}

bool getLineFromString(char * src, char * dest)
{
// 	char * stempstr = (char*)malloc(101);
// 	memset(stempstr,0,101);
	int nidx = 0;
	for (int i = 0; i < strlen(src); i++)
	{
		nidx++;
		if(src[i] == 0x0d || src[i] == 0x0a)
		{
			memset(dest,0,101);
			strncpy(dest,src,nidx);
			break;
		}
	}
	return true;
}

NR_EXTERN LICENSE_EXPORT bool NR_API CheckTime()
{
/*
	//1.获取系统类型标识，并判断是否是为风电(0)
	int ntype = 1;
	char sTypeStr[201] = {'\0'};
	char sTypeFile[512] = {'\0'};
	sprintf(sTypeFile, "%s%s", getenv(SOPHIC_DEPLOY_DIR), SYSTEM_TYPE_FILE);
	ifstream ftypein(sTypeFile);
	if(!ftypein.getline(sTypeStr, 200))
	{
		ftypein.close();
		return true;
	}
	ftypein.close();
	sscanf(sTypeStr, "%*[^=]=%d", &ntype);
	if(ntype != 0)
		return true;
*/

	//2 检查安装
	//2.1 获取用户目录
#ifdef WIN32
	std::locale loc = std::locale::global(std::locale("")); //要打开的文件路径含中文，设置全局locale为本地环境
#endif
	char sEnStr[201] = {'\0'};
	char sLicFile[512] = {'\0'};
	char sLic[201] = {'\0'};
	memset(sEnStr,0,sizeof(sEnStr));
	memset(sLicFile,0,sizeof(sLicFile));
	memset(sLic,0,sizeof(sLic));
	char * sHome = getenv(HOMEFILE);
	if(NULL == sHome)
	{
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return false;
	}
	sprintf(sLicFile, "%s%s", sHome, GMUDLICFILE);

	ifstream flicin(sLicFile);
	if(!flicin.getline(sEnStr, 200))
	{
		flicin.close();
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return false;
	}
	flicin.close();
	decryptStr(sEnStr,sLic);

	//2.2 mac地址
	char smacid[30] = {'\0'};
	NR_INT64 macIDOld = 0;
	NR_INT64 macIDNew = 0;
	int nyearold = 0;
	int nmonthold = 0;
	int ndayold = 0;

#if defined(WIN32)
	sscanf(sLic,"%I64u,%d,%d,%d",&macIDOld,&nyearold,&nmonthold,&ndayold);
#else
	sscanf(sLic,"%llu,%d,%d,%d",&macIDOld,&nyearold,&nmonthold,&ndayold);
#endif

	NR_UINT udayold = julianDayFromDate(nyearold,nmonthold,ndayold);

	NR_UINT udaystart = julianDayFromDate(2011,1,1);
	NR_UINT udaystop = julianDayFromDate(2014,12,31);
	if(udayold < udaystart || udayold > udaystop)
	{
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return false;
	}

	if(!createMACID(macIDNew))
	{
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return false;
	}

	if(macIDNew != macIDOld)
	{
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return false;
	}

	//2.3 日期
	time_t m_installTime = time(0);
	struct tm * curtime = localtime(&m_installTime);
	int nyearcur = curtime->tm_year+1900;
	int nmonthcur = curtime->tm_mon+1;
	int ndaycur = curtime->tm_mday;

	NR_UINT udaycur = julianDayFromDate(nyearcur,nmonthcur,ndaycur);

	if( ndaycur < ndayold )
	{
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return false;
	}

	//3 判断时间
	memset(sEnStr,0,201);
	char sDatFile[512] = {'\0'};
	sprintf(sDatFile, "%s%s", sHome, GMUDDATEFILE);

	char sLastTime[200] = {'\0'};
	char sLastTimeEn[401] = {'\0'};
	char sDat[201] = {'\0'};
	memset(sLastTime,0,sizeof(sLastTime));
	memset(sLastTimeEn,0,sizeof(sLastTimeEn));
	memset(sDat,0,sizeof(sDat));

	ifstream fdatin(sDatFile);
	if(!fdatin.getline(sEnStr, 200))
	{
		//空文件，写入当前日期
		fdatin.close();
		ofstream fdatout(sDatFile);
		sprintf(sLastTime, "LastTime=%d-%d-%d", nyearcur, nmonthcur, ndaycur);
		encryptStr(sLastTime,sLastTimeEn);
		fdatout << sLastTimeEn;
		fdatout.close();
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return true;
	}
	fdatin.close();

	//3.1 获取lasttime时间
	decryptStr(sEnStr,sDat);
	int nyearlast = 0;
	int nmonthlast = 0;
	int ndaylast = 0;
	sscanf(sDat, "%*[^=]=%d-%d-%d", &nyearlast, &nmonthlast, &ndaylast);
	NR_UINT udaylast = julianDayFromDate(nyearlast, nmonthlast, ndaylast);

	//3.2 与当前时间进行比较
	int ndaydiff = udaycur - udaylast;
	if(ndaydiff > 180 || ndaydiff < -180)
	{
#ifdef WIN32
		std::locale::global(loc);//恢复全局locale 
#endif
		return false;
	}

	if(ndaydiff > 0)
	{
		ofstream fdatout(sDatFile);
		sprintf(sLastTime, "LastTime=%d-%d-%d", nyearcur, nmonthcur, ndaycur);
		memset(sLastTimeEn,0,sizeof(sLastTimeEn));
		encryptStr(sLastTime,sLastTimeEn);
		fdatout << sLastTimeEn;
		fdatout.close();
	}
#ifdef WIN32
	std::locale::global(loc);//恢复全局locale 
#endif
    return true;
}

bool getMacIDList(NR_INT64* pMacList, int& ncount)
{
//windows 系统
#ifdef WIN32
    static bool done = false;
	
    if (!done) {
        done = true;
		
        HINSTANCE iphlpapiHnd = LoadLibrary(L"iphlpapi");
        if (iphlpapiHnd == NULL)
            return false;
		
#if defined(WINCE) || defined(_WIN32_WCE)
        ptrGetAdaptersInfo = (PtrGetAdaptersInfo)GetProcAddress(iphlpapiHnd, L"GetAdaptersInfo");
        ptrGetAdaptersAddresses = (PtrGetAdaptersAddresses)GetProcAddress(iphlpapiHnd, L"GetAdaptersAddresses");
        ptrGetNetworkParams = (PtrGetNetworkParams)GetProcAddress(iphlpapiHnd, L"GetNetworkParams");
#else
        ptrGetAdaptersInfo = (PtrGetAdaptersInfo)GetProcAddress(iphlpapiHnd, "GetAdaptersInfo");
        ptrGetAdaptersAddresses = (PtrGetAdaptersAddresses)GetProcAddress(iphlpapiHnd, "GetAdaptersAddresses");
        ptrGetNetworkParams = (PtrGetNetworkParams)GetProcAddress(iphlpapiHnd, "GetNetworkParams");
#endif
	}
	if (ptrGetAdaptersAddresses != NULL)
        interfaceListingWinXP(pMacList,ncount);
    else if (ptrGetAdaptersInfo != NULL)
        interfaceListingWin2k(pMacList,ncount);

	return true;

#elif (defined(__linux__) || defined(__linux)) &&  __GLIBC__ - 0 >= 2 && __GLIBC_MINOR__ - 0 >= 1

    int socket;
    if ((socket = qt_safe_socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1)
        return false;      // error
	
    ifaddrs *interfaceListing;
    if (getifaddrs(&interfaceListing) == -1)
	{
        // error
        ::close(socket);
        return false;
    }
	
	NR_INT64 nmacid = 0;
	int nidx = 0;

	// on Linux we use AF_PACKET and sockaddr_ll to obtain hHwAddress
	nidx = 0;
	for (ifaddrs *ptr = interfaceListing; ptr; ptr = ptr->ifa_next)
	{
        if ( !ptr->ifa_addr )
            continue;
		nmacid = 0;
		char smacid[100] = {'\0'};
		if ( ptr->ifa_addr->sa_family == AF_PACKET )
		{
            sockaddr_ll *sll = (sockaddr_ll *)ptr->ifa_addr;
			for(int i = 0; i < sll->sll_halen; i++)
				nmacid += ((NR_INT64)sll->sll_addr[i]) << (i*8);

			for(int j = 0; j < sll->sll_halen; j++)
			{
				char smactemp[10] = {'\0'};
				if(j == sll->sll_halen-1)
					sprintf(smactemp,"%02x",(NR_BYTE)sll->sll_addr[j]);
				else
					sprintf(smactemp,"%02x:",(NR_BYTE)sll->sll_addr[j]);
				strcat(smacid,smactemp);
			}
// 			printf("macid:  %s\n",smacid);
// 			printf("macid:  %lld\n",nmacid);

			if(nmacid == 0)
				continue;

			int nmacno = 0;
			for(nmacno = 0; nmacno < nidx; nmacno++)
			{
				if(pMacList[nmacno] == nmacid)
					break;
			}
			if(nmacno == nidx)
			{
				pMacList[nidx] = nmacid;
				nidx++;
			}
			if(nidx == MAX_MACID_NUMBER)
				break;
        }
	}
	ncount = nidx;

	freeifaddrs(interfaceListing);
    ::close(socket);

	return true;

#elif defined(__sun) || defined(__solaris__) || defined(__SOLARIS__)

	register int fd, intrface;
	struct ifreq buf[MAXINTERFACES];
	struct arpreq arp;
	struct ifconf ifc;

	if ((fd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
		return false;      // error

	ifc.ifc_len = sizeof buf;
	ifc.ifc_buf = ( caddr_t ) buf;

	NR_INT64 nmacid = 0;
	int nidx = 0;

	if (ioctl(fd,SIOCGIFCONF,(char*)&ifc) >= 0)
	{
		//获取接口信息
		intrface = ifc.ifc_len / sizeof ( struct ifreq );

		//根据借口信息循环获取设备IP和MAC地址
		while ( intrface-- > 0 )
		{
			/* this section can't get Hardware Address,I don't know whether the reason is module driver*/
			//获取MAC地址
			arp.arp_pa.sa_family = AF_INET;
			arp.arp_ha.sa_family = AF_INET;
			( ( struct sockaddr_in* ) &arp.arp_pa )->sin_addr.s_addr = ( ( struct sockaddr_in* ) ( &buf[intrface].ifr_addr ) )->sin_addr.s_addr;
			
			nmacid = 0;
			char smacid[100] = {'\0'};
			if ((ioctl(fd,SIOCGARP,(char*)&arp)) >= 0)
			{
				nmacid += (( ( NR_INT64 ) (arp.arp_ha.sa_data[0]) ) << 40);
				nmacid += (( ( NR_INT64 ) (arp.arp_ha.sa_data[1]) ) << 32);
				nmacid += (( ( NR_INT64 ) (arp.arp_ha.sa_data[2]) ) << 24);
				nmacid += (( ( NR_INT64 ) (arp.arp_ha.sa_data[3]) ) << 16);
				nmacid += (( ( NR_INT64 ) (arp.arp_ha.sa_data[4]) ) << 8);
				nmacid += ( NR_INT64 ) (arp.arp_ha.sa_data[5]);

				sprintf(smacid,"%02x:%02x:%02x:%02x:%02x:%02x",(NR_BYTE)arp.arp_ha.sa_data[0],
					(NR_BYTE)arp.arp_ha.sa_data[1],(NR_BYTE)arp.arp_ha.sa_data[2],(NR_BYTE)arp.arp_ha.sa_data[3],
					(NR_BYTE)arp.arp_ha.sa_data[4],(NR_BYTE)arp.arp_ha.sa_data[5]);
// 				printf("macid:  %s\n",smacid);
// 				printf("macid:  %lld\n",nmacid);

				if(nmacid == 0)
					continue;
				
				int nmacno = 0;
				for(nmacno = 0; nmacno < nidx; nmacno++)
				{
					if(pMacList[nmacno] == nmacid)
						break;
				}
				if(nmacno == nidx)
				{
					pMacList[nidx] = nmacid;
					nidx++;
				}
				if(nidx == MAX_MACID_NUMBER)
					break;
			}
		} //while
		close ( fd );
		ncount = nidx;
		return true;
	}
	close ( fd );
	return false;

#elif defined(__aix__) || defined(__AIX__) || defined(_AIX)

	NR_INT64 nmacid = 0;
	int nidx = 0;

#ifdef SIOCGIFHWADDR
	//has SIOCGIFHWADDR
	int socket;
    if ((socket = qt_safe_socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1)
        return false;      // error

	// use if_nameindex
    struct if_nameindex *interfaceList = ::if_nameindex();
    for (struct if_nameindex *ptr = interfaceList; ptr && ptr->if_name; ++ptr)
    {
		int nlength = 0;

		ifreq req;
		memset(&req, 0, sizeof(ifreq));
		if(sizeof(ptr->if_name)-1 > sizeof(req.ifr_name)-1)
			nlength = sizeof(req.ifr_name)-1;
		else
			nlength = sizeof(ptr->if_name)-1;
		memcpy(req.ifr_name, ptr->if_name, nlength);

		nmacid = 0;
		char smacid[100] = {'\0'};
		// Get the HW address

		if(qt_safe_ioctl(socket, SIOCGIFHWADDR, &req) >= 0)
		{
			uchar *addr = (uchar *)&req.ifr_addr;
			for(int i = 0; i < 6; i++)
				nmacid += ((NR_INT64)addr[i]) << (i*8);
			sprintf(smacid,"%02x:%02x:%02x:%02x:%02x:%02x",addr[0],
				addr[1],addr[2],addr[3],addr[4],addr[5]);
			free(addr);
		}

		if(nmacid == 0)
			continue;
		
		int nmacno = 0;
		for(nmacno = 0; nmacno < nidx; nmacno++)
		{
			if(pMacList[nmacno] == nmacid)
				break;
		}
		if(nmacno == nidx)
		{
			pMacList[nidx] = nmacid;
			nidx++;
		}
		if(nidx == MAX_MACID_NUMBER)
			break;
	}
	if_freenameindex(interfaceList);
	::close(socket);

#else
	//no SIOCGIFHWADDR

	int size = 0;
	struct kinfo_ndd *nddp;
	void *end;
	size = getkerninfo(KINFO_NDD, 0, 0, 0);
	if(size > 0)
	{
		nddp = (struct kinfo_ndd *)malloc(size);
		if(nddp)
		{
			if (getkerninfo(KINFO_NDD, nddp, &size, 0) >= 0)
			{
				end = nddp + size;

				int nsizeidx = 0;
				while ((nddp < end) && nsizeidx < size)
				{
					nsizeidx++;

					nmacid = 0;
					char smacid[100] = {'\0'};

					char* ptest = strstr(nddp->ndd_alias,"ent");
					char* ptest1 = strstr(nddp->ndd_name,"ent");
					if(ptest || ptest1)
					{
//						printf("nddp->ndd_alias:%s,nddp->ndd_name:%s\n",nddp->ndd_alias,nddp->ndd_name);
// 						printf("%02x:%02x:%02x:%02x:%02x:%02x\n",nddp->ndd_addr[0],
// 							nddp->ndd_addr[1],nddp->ndd_addr[2],nddp->ndd_addr[3],nddp->ndd_addr[4],nddp->ndd_addr[5]);
						for(int i = 0; i < 6; i++)
							nmacid += ((NR_INT64)nddp->ndd_addr[i]) << (i*8);
// 						sprintf(smacid,"%02x:%02x:%02x:%02x:%02x:%02x",nddp->ndd_addr[0],
// 							nddp->ndd_addr[1],nddp->ndd_addr[2],nddp->ndd_addr[3],nddp->ndd_addr[4],nddp->ndd_addr[5]);
						
//						char slogcontent[1024] = {'\0'};
//						char slogtemp[512] = {'\0'};
//						char sLogFile[512] = {'\0'};
//						sprintf(sLogFile, "%s%s", getenv(HOMEFILE), "/nrlicense.log1");
//						memset(slogtemp,0,sizeof(slogtemp));
//						sprintf(slogtemp,"test\n");
//						strcat(slogcontent,slogtemp);
//						ofstream flogout(sLogFile,ios::out|ios::app);
//						flogout<<slogcontent;
//						flogout.close();

						if(ptest)
							free(ptest);
						if(ptest1)
							free(ptest1);
					}

					if(nmacid == 0)
					{
						nddp++;
						continue;
					}
					
					int nmacno = 0;
					for(nmacno = 0; nmacno < nidx; nmacno++)
					{
						if(pMacList[nmacno] == nmacid)
							break;
					}
					if(nmacno == nidx)
					{
						pMacList[nidx] = nmacid;
						nidx++;
					}
					if(nidx == MAX_MACID_NUMBER)
						break;

					nddp = nddp+1;
				}
			}
			free(nddp);
		}
	}
#endif

	ncount = nidx;
	return true;

#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__)

	int socket;
    if ((socket = qt_safe_socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1)
        return false;      // error
	
    ifaddrs *interfaceListing;
    if (getifaddrs(&interfaceListing) == -1)
	{
        // error
        ::close(socket);
        return false;
    }
	
	NR_INT64 nmacid = 0;
	int nidx = 0;

	// on NetBSD we use AF_LINK and sockaddr_dl
    // scan the list for that family
    nidx = 0;
    for (ifaddrs *ptr = interfaceListing; ptr; ptr = ptr->ifa_next)
	{
		if (ptr->ifa_addr && ptr->ifa_addr->sa_family == AF_LINK)
		{
			sockaddr_dl *sdl = (sockaddr_dl *)ptr->ifa_addr;
			for(int i = 0; i < sdl->sdl_alen; i++)
				nmacid += ((NR_INT64)sdl->sdl_alen[i]) << (i*8);

			if(nmacid == 0)
				continue;
			
			int nmacno = 0;
			for(nmacno = 0; nmacno < nidx; nmacno++)
			{
				if(pMacList[nmacno] == nmacid)
					break;
			}
			if(nmacno == nidx)
			{
				pMacList[nidx] = nmacid;
				nidx++;
			}
			if(nidx == MAX_MACID_NUMBER)
				break;
		}
	}
	ncount = nidx;

	freeifaddrs(interfaceListing);
    ::close(socket);

	return true;

#else

	return false;

#endif

return false;
}

#if defined(WIN32)

bool interfaceListingWinXP(NR_INT64* pMacList, int& ncount)
{
	IP_ADAPTER_ADDRESSES staticBuf[2]; // 2 is arbitrary
    PIP_ADAPTER_ADDRESSES pAdapter = staticBuf;
    NR_ULONG bufSize = sizeof staticBuf;
	NR_ULONG flags = GAA_FLAG_INCLUDE_ALL_INTERFACES |
		GAA_FLAG_INCLUDE_PREFIX |
		GAA_FLAG_SKIP_DNS_SERVER |
		GAA_FLAG_SKIP_MULTICAST;
	flags = GAA_FLAG_INCLUDE_PREFIX;
    NR_ULONG retval = ptrGetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAdapter, &bufSize);
	if (retval == ERROR_BUFFER_OVERFLOW) {
        // need more memory
        pAdapter = (IP_ADAPTER_ADDRESSES *)malloc(bufSize);
        if (!pAdapter)
            return false;
        // try again
        if (ptrGetAdaptersAddresses(AF_UNSPEC, flags, NULL, pAdapter, &bufSize) != ERROR_SUCCESS) {
            free(pAdapter);
            return false;
        }
    } else if (retval != ERROR_SUCCESS) {
        // error
        return false;
    }
	// iterate over the list and add the entries to our listing
	int nidx = 0;
    for (PIP_ADAPTER_ADDRESSES ptr = pAdapter; ptr; ptr = ptr->Next)
	{
		NR_INT64 nmacid = 0;
		char smacid[100] = {'\0'};
		memset(smacid,0,sizeof(smacid));
		if (ptr->PhysicalAddressLength)
		{
			//IF_TYPE_ETHERNET_CSMACD = 6, 以太网网络接口
			//IF_TYPE_ISO88025_TOKENRING = 9, 令牌环网络接口
			//IF_TYPE_PPP = 23,  在PPP网络接口
			//IF_TYPE_SOFTWARE_LOOPBACK = 24, 软件回环网络接口
			//IF_TYPE_ATM = 37, ATM网络接口
			//IF_TYPE_IEEE80211 = 71, IEEE 802.11无线网络接口
			//IF_TYPE_TUNNEL = 131, 隧道类型封装网络接口
			//IF_TYPE_IEEE1394 = 144, 一个IEEE 1394（火线）高性能串行总线网络接口
//			printf("IfType:%d\n",ptr->IfType);
			if(ptr->IfType != 6)
				continue;

			for(int i = 0; i < ptr->PhysicalAddressLength; i++)
				nmacid += ((NR_INT64)ptr->PhysicalAddress[i]) << (i*8);

			for(int j = 0; j < ptr->PhysicalAddressLength; j++)
			{
				char smactemp[10] = {'\0'};
				if(j == ptr->PhysicalAddressLength-1)
					sprintf(smactemp,"%02x",ptr->PhysicalAddress[j]);
				else
					sprintf(smactemp,"%02x:",ptr->PhysicalAddress[j]);
				strcat(smacid,smactemp);
			}
// 			printf("macid:  %s\n",smacid);
// 			printf("macid:  %lld\n",nmacid);

			if(nmacid == 0)
				continue;
			
			int nmacno = 0;
			for(nmacno = 0; nmacno < nidx; nmacno++)
			{
				if(pMacList[nmacno] == nmacid)
					break;
			}
			if(nmacno == nidx)
			{
				pMacList[nidx] = nmacid;
				nidx++;
			}
		}
    }
	ncount = nidx;
	free(pAdapter);
	return true;
}

bool interfaceListingWin2k(NR_INT64* pMacList, int& ncount)
{
	IP_ADAPTER_INFO staticBuf[2]; // 2 is arbitrary
    PIP_ADAPTER_INFO pAdapter = staticBuf;
    NR_ULONG bufSize = sizeof staticBuf;
	
    NR_DWORD retval = ptrGetAdaptersInfo(pAdapter, &bufSize);
    if (retval == ERROR_BUFFER_OVERFLOW) {
        // need more memory
        pAdapter = (IP_ADAPTER_INFO *)malloc(bufSize);
        if (!pAdapter)
            return false;
        // try again
        if (ptrGetAdaptersInfo(pAdapter, &bufSize) != ERROR_SUCCESS) {
            free(pAdapter);
            return false;
        }
    } else if (retval != ERROR_SUCCESS) {
        // error
        return false;
    }
    // iterate over the list and add the entries to our listing
	int nidx = 0;
    for (PIP_ADAPTER_INFO ptr = pAdapter; ptr; ptr = ptr->Next)
	{
		NR_INT64 nmacid = 0;
		if (ptr->AddressLength)
		{
			for(int i = 0; i < ptr->AddressLength; i++)
				nmacid += ((NR_INT64)ptr->Address[i]) << (i*8);

			if(nmacid == 0)
				continue;
			
			int nmacno = 0;
			for(nmacno = 0; nmacno < nidx; nmacno++)
			{
				if(pMacList[nmacno] == nmacid)
					break;
			}
			if(nmacno == nidx)
			{
				pMacList[nidx] = nmacid;
				nidx++;
			}
		}
	}
	ncount = nidx;
	free(pAdapter);
	return true;
}

bool isDomainUser()
{
	char* sUserDomain = getenv("USERDOMAIN");
	char* sUserDnsDomain = getenv("USERDNSDOMAIN");

	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	if(sUserDomain)
	{
		p1 = strstr(sUserDomain,"NR-RD");
		p2 = strstr(sUserDomain,"NARI-RELAYS");
	}
	if(sUserDnsDomain)
		p3 = strstr(sUserDnsDomain,"NARI-RELAYS");
	if(p1 || p2 || p3)
		return true;

	return false;
}

#endif//win32


NR_EXTERN LICENSE_EXPORT bool NR_API createHostIDNew(NR_PARAM_OUT char * sHostID, NR_PARAM_IN const int nIndex)
{
	NR_INT64 cpuID = 0;
	NR_INT64 hardID = 0;
	NR_INT64 macID = 0;
	NR_INT64 HostID = 0;
	memset(sHostID, 0, NR_HOSTIDLEN + 1);

	if ( !createMACIDNew ( macID, nIndex ) )
	{
		macID = 0;
	}

#if defined(LOG_LOG)
	char slogcontent[1024] = {'\0'};
	char slogtemp[512] = {'\0'};
	char sLogFile[512] = {'\0'};
	memset(slogcontent,0,sizeof(slogcontent));
	sprintf(sLogFile, "%s%s", getenv(HOMEFILE), "/nrlicense.log");
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%d,","CPUID:",cpuID);
	strcat(slogcontent,slogtemp);
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%x,","HardID:",hardID);
	strcat(slogcontent,slogtemp);
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%d,","MacID:",macID);
	strcat(slogcontent,slogtemp);
	ofstream flogout(sLogFile,ios::out|ios::app);
	flogout<<slogcontent;
	flogout.close();
#endif
	
	if(cpuID == 0 && hardID == 0 && macID == 0)
		return false;
	
	HostID += cpuID;
	HostID += hardID;
	NR_BYTE *pFeatureHostID;
	NR_BYTE *pFeatureMacID;
	pFeatureMacID = ( NR_BYTE * ) & macID;
	
	for ( int i = 0; i < 6; i++ )
		pFeatureMacID[i] ^= pFeatureMacID[HostIDOrderFit[i]];
	
	pFeatureHostID = ( NR_BYTE* ) & HostID;
	
	for ( int j = 0; j < 6; j++ )
		pFeatureHostID[j] ^= pFeatureMacID[5-j];
	
	sprintf ( sHostID, "%02x%02x%02x%02x%02x%02x",
		pFeatureHostID[0],
		pFeatureHostID[1],
		pFeatureHostID[2],
		pFeatureHostID[3],
		pFeatureHostID[4],
		pFeatureHostID[5] );
	return true;
}

bool createHostIDNew1(char * sHostID, NR_INT64 macID)
{
	NR_INT64 cpuID = 0;
	NR_INT64 hardID = 0;
	NR_INT64 HostID = 0;
	memset(sHostID, 0, NR_HOSTIDLEN + 1);
	
#if defined(LOG_LOG)
	char slogcontent[1024] = {'\0'};
	char slogtemp[512] = {'\0'};
	char sLogFile[512] = {'\0'};
	memset(slogcontent,0,sizeof(slogcontent));
	sprintf(sLogFile, "%s%s", getenv(HOMEFILE), "/nrlicense.log");
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%d,","CPUID:",cpuID);
	strcat(slogcontent,slogtemp);
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%x,","HardID:",hardID);
	strcat(slogcontent,slogtemp);
	memset(slogtemp,0,sizeof(slogtemp));
	sprintf(slogtemp,"%s%d,","MacID:",macID);
	strcat(slogcontent,slogtemp);
	ofstream flogout(sLogFile,ios::out|ios::app);
	flogout<<slogcontent;
	flogout.close();
#endif
	
	if(cpuID == 0 && hardID == 0 && macID == 0)
		return false;
	
	HostID += cpuID;
	HostID += hardID;
	NR_BYTE *pFeatureHostID;
	NR_BYTE *pFeatureMacID;
	pFeatureMacID = ( NR_BYTE * ) & macID;
	
	for ( int i = 0; i < 6; i++ )
		pFeatureMacID[i] ^= pFeatureMacID[HostIDOrderFit[i]];
	
	pFeatureHostID = ( NR_BYTE* ) & HostID;
	
	for ( int j = 0; j < 6; j++ )
		pFeatureHostID[j] ^= pFeatureMacID[5-j];
	
	sprintf ( sHostID, "%02x%02x%02x%02x%02x%02x",
		pFeatureHostID[0],
		pFeatureHostID[1],
		pFeatureHostID[2],
		pFeatureHostID[3],
		pFeatureHostID[4],
		pFeatureHostID[5] );
	return true;
}

NR_EXTERN LICENSE_EXPORT bool NR_API createMACIDNew(NR_PARAM_OUT NR_INT64 & macID, NR_PARAM_IN int nIndex)
{
	NR_INT64 pmacIDList[MAX_MACID_NUMBER];//最多20个网卡
	memset(pmacIDList,0,MAX_MACID_NUMBER*sizeof(NR_INT64));
	int ncount = 0;

	if(!getMacIDList(pmacIDList,ncount))
		return false;

	if(ncount <= nIndex)
		return false;
	
	macID = pmacIDList[nIndex];
	
	return true;
}
