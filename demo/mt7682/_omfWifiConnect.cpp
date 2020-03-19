//
// Created by wang.zhou on 2020/3/4.
//
//
// Created by wang.zhou on 2020/3/3.
//
#include "omf_api.h"
//#include "omf_api_entry.h"
#include "omf_msg_site.h"
#include "stdlib.h"
#include <string>
#include "string.h"
#include <unistd.h>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <mutex>
#include <thread>
#include <chrono>

static std::mutex wlan_res_mtx;
static std::mutex wlan_ifconfig_mtx;
static int ifconfig_ready = 0;
extern "C" {
	#include "icatchtek_mt7682.h"
	#include "icatchtek_mt7682_def.h"
}

static int xmitCallback(char* s,int len);

//////////////////////////////
#define returnIfErrC(v,c) if(c){printf("%s/%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,#c);return v;}
#define dbgTestPSL(v) printf("%s/%d:%s,%s=%s\n",__FILE__,__LINE__,__FUNCTION__,#v,v)
///////////////////////////////
#define SDIO_SWITCH_LINUX	0xFFFB
#define	SDIO_SWITCH_ICATOS	0xFFFC
///////////////////////////////
static char* _sdioBuf = 0;
static int _sdioMsgId = 100;
struct net_ip_config {
	uint32_t ip;
	uint32_t gw;
	uint32_t netmask;
	uint32_t dns[2];
	char mac[8];
};

#define BUF_LENGTH				(2048)
#define MODULE_PATH				"/lib/modules"
#define WLAN_MODULE_NAME		"icatchtek_wlan.ko"
#define SDIO_MODULE_NAME		"icatchtek_sdio.ko"

#define WLAN_WIFI_HAND_SSID		"/proc/driver/icatchtek_wlan/ssid"
#define WLAN_WIFI_HAND_PWD		"/proc/driver/icatchtek_wlan/pwd"
#define WLAN_WIFI_HAND_ACTION	"/proc/driver/icatchtek_wlan/action"

#define WLAN_WIFI_HAND_IPADDR	"/proc/driver/icatchtek_wlan/ipaddr"

struct cipher_info {
	int enable;
	int cipher_id;
	int key_len;
	char key[33];
	int iv_len;
	char iv[17];
};

static int _get_cipher_info_from_RTOS(struct cipher_info *info)
{
	char buf[256] = {0};
	std::string param = "cmd=omfGetFromHost,sel=0xFFFA";
	if(!info) {
		return -1;
	}
	if(!omfCommand("shm0_cmd",param.c_str(), buf)) {
		return -1;
	}
	void *attr0 = omfCreateAttrSet(buf);
	int err = omfAttrGetInt(attr0,"res");
	if(!err) {
		int len = omfAttrGetInt(attr0,"len");
		int addr = omfAttrGetInt(attr0,"addr");
		printf("len = %d, addr:%#x\n", len, addr);
		int offset = 0;

		void* cipher_info = (void*)addr;
		memcpy(&info->enable, cipher_info + offset, 4);
		offset += 4;

		if(info->enable == 1) {
			memcpy(&info->cipher_id, cipher_info + offset, 4);
			offset += 4;

			memcpy(&info->key_len, cipher_info + offset, 4);
			offset += 4;

			memcpy(info->key,  cipher_info + offset, info->key_len);
			offset += info->key_len;

			memcpy(&info->iv_len, cipher_info + offset, 4);
			offset += 4;

			if(info->iv_len) {
				memcpy(info->iv,  cipher_info + offset, info->iv_len);
				offset += info->iv_len;
			}
#if 0
			int i = 0;
			char *keyInfo = cipher_info + offset;
			for(i = 0; i < info->key_len; i++) {
				sprintf(key + i * 2, "%02x", keyInfo[i] & 0xFF);
			}
			printf("Key:%d\n%s\n",  info->key_len, key);
			offset += info->key_len;

			memcpy(&info->iv_len, cipher_info + offset, 4);
			offset += 4;
			if(info->iv_len > 0) {
				char *ivInfo = cipher_info + offset;
				for(i = 0; i < info->iv_len; i++) {
							sprintf(info->iv + i * 2, "%02x", ivInfo[i] & 0xFF);
				}
				offset += info->iv_len;
				printf("IV:\n%s\n", iv);
			}
#endif
		} else {
			info->enable = 0;
		}
	}	else {
		info->enable = 0;
		printf("==res==:%d\n", err);
		return -1;
	}

	omfDestroy(attr0);
	return 0;
}

static bool _file_access(const char* fileName)
{
	return (access(fileName, 0) == 0 ? true : false);
}

static bool _wlan_driver_is_install()
{
	return _file_access((const char*)WLAN_WIFI_HAND_SSID);
}




int doCmd(const char* cmdStr, const char* mode, char* result, int resSize)
{
	char buf_ps[BUF_LENGTH];
	FILE *ptr;
	int totalLen = resSize;
	printf("[%s]\n", cmdStr);
	int offset = 0;
	if((ptr=popen(cmdStr, mode))!=NULL)
	{
		while(fgets(buf_ps, BUF_LENGTH, ptr) != NULL)
		{
			if(result) {
				if(strlen(buf_ps) + offset > totalLen) {
					printf("%s ", result);
					printf("%s\n", buf_ps);
					break;
				}
				strcat(result, buf_ps);
				offset += strlen(buf_ps);
			} else {
				printf("%s ", buf_ps);
			}
		}
		if(result) {
			printf("%s ", result);
		}
		printf("\n");
		pclose(ptr);
		ptr = NULL;
		return 0;
	} else {
		printf("Do [%s] error !!\n", cmdStr);
	}
	return -1;
}

static int _get_system_kernel_verison(char* result)
{
	char cmdBuf[64] = "uname -r";
	if(!doCmd(cmdBuf, "r", result, 128)) {
		return 0;
	}
	return -1;
}

static void _wait_wlan_module_ready(void)
{
	std::lock_guard<std::mutex> lock(wlan_res_mtx);
	while(!_file_access(WLAN_WIFI_HAND_SSID)) {
		std::this_thread::sleep_for(std::chrono::microseconds(20));
	}
}


/**
 * net lonk init,regist call back
 * */
static void _netlinkIoInit(void){
	static int netLinkInited = 0;
	int retryCnt = 0;
	while(1) {
		if (_file_access(WLAN_WIFI_HAND_SSID)) {
			if(!netLinkInited) {
				if(icatchtek_mt7682_io_init(xmitCallback) != 0) {
					std::this_thread::sleep_for(std::chrono::microseconds(20));
					printf("[%s(%d)]netlink init retry %d ...\n" ,__FUNCTION__,__LINE__, ++retryCnt);
				} else {
					netLinkInited = 1;
					printf("[%s(%d)]netlink init done!\n",__FUNCTION__,__LINE__);
					break;
				}
			} else {
				printf("[%s(%d)]netlink reinit done!\n",__FUNCTION__,__LINE__);
				break;
			}
		}
	}
}


static void _wait_wlan_ifconfig_ready(void)
{
	char cmdBuf[1024] = {0};
	char result[BUF_LENGTH] = {0};
	std::lock_guard<std::mutex> lock(wlan_ifconfig_mtx);

	while(!ifconfig_ready) {
		if(!_file_access(WLAN_WIFI_HAND_IPADDR)) {
			// set dns
			sprintf(cmdBuf, "cat /proc/driver/icatchtek_wlan/ipaddr | awk -F ':' '{print $4}'");
			if(!doCmd(cmdBuf, "r", result, BUF_LENGTH)) {
				if(strlen(result) > 0) {
					memset(cmdBuf, 0, strlen(cmdBuf));
					sprintf(cmdBuf, "echo nameserver %s > /etc/resolv.conf", result);
					if(!doCmd(cmdBuf, "r", result, BUF_LENGTH)) {
						break;
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(20));
	}
}
static void _content_to_hexstr(char* dst, char* src, const int src_size)
{
	int i = 0;
	for(i = 0; i < src_size; i++) {
		sprintf(dst + i * 2, "%02x", src[i] & 0xFF);
	}

}
static bool _wifi_prepare(struct cipher_info *info)
{
	char cmdBuf[256] = {0};
	char result[BUF_LENGTH] = {0};
	if(!_wlan_driver_is_install()) {
		char version[64] = {0};
		int ret = _get_system_kernel_verison(version);
		if(ret < 0) {
			return false;
		} else {
			int len = strlen(version);
			version[len -1 ] = '\0';
		}
		//insmod sdio module
		sprintf(cmdBuf, "insmod %s/%s/%s reset=0", MODULE_PATH, version, SDIO_MODULE_NAME);
		if(doCmd(cmdBuf, "r", result, BUF_LENGTH)) {
			return false;
		}

		//insmod wlan module
		memset(cmdBuf, 0, strlen(cmdBuf));
		if(info->enable) {

			char key[65] = {0};
			char iv[33] = {0};

			_content_to_hexstr(key, info->key, info->key_len);
			printf("CipherID:%d\nKEY:%s\n",info->cipher_id,  key);
			if(info->iv_len) {
				_content_to_hexstr(iv, info->iv, info->iv_len);
				printf("IV:%s\n", iv);
				sprintf(cmdBuf, "insmod  %s/%s/%s alg=%d key=%s iv=%s", MODULE_PATH, version, WLAN_MODULE_NAME, info->cipher_id, key, iv);
			} else {
				sprintf(cmdBuf, "insmod %s/%s/%s alg=%d key=%s", MODULE_PATH, version, WLAN_MODULE_NAME, info->cipher_id, key);
			}
		} else {
			sprintf(cmdBuf, "insmod %s/%s/%s", MODULE_PATH, version, WLAN_MODULE_NAME);
		}
		if(doCmd(cmdBuf, "r", result, BUF_LENGTH)) {
			return false;
		}

		_wait_wlan_module_ready();
		std::thread th2(_netlinkIoInit);
		th2.join();
		return true;
	}
	return true;
}


static bool _wifi_connect_ap(std::string ssid, std::string password)
{
	char cmdBuf[512] = {0};
	char result[BUF_LENGTH] = {0};
	std::lock_guard<std::mutex> lock(wlan_res_mtx);

	// set ssid
	sprintf(cmdBuf, "echo -n %s > %s", ssid.c_str(), WLAN_WIFI_HAND_SSID);
	if(doCmd(cmdBuf, "r", result, BUF_LENGTH)) {
		printf("Error info:\n %s\n", result);
		return false;
	}

	// set password
	memset(cmdBuf, 0, strlen(cmdBuf));
	sprintf(cmdBuf, "echo -n %s > %s", password.c_str(), WLAN_WIFI_HAND_PWD);
	if(doCmd(cmdBuf, "r", result, BUF_LENGTH)) {
		printf("Error info:\n %s\n", result);
		return false;
	}

	// connect
	memset(cmdBuf, 0, strlen(cmdBuf));
	sprintf(cmdBuf, "echo -n connect > %s",  WLAN_WIFI_HAND_ACTION);
	if(doCmd(cmdBuf, "r", result, BUF_LENGTH)) {
		printf("Error info:\n %s\n", result);
		return false;
	}
	std::thread th(_wait_wlan_ifconfig_ready);
	th.detach();
	return true;
}



/**
 * set dns server to linux*
 * */
static void dnsSet(void){
	char ssidName[] = "/proc/driver/icatchtek_wlan/ipaddr";
	char dnsName[] = "/tmp/resolv.conf.auto";
	char dns[64] = {0};
	char cmd[64] = {0};
	int  i, j = 0;
	//printf("%s(%d)\n", __FUNCTION__, __LINE__);
	while(1) {
		if(!access(dnsName, 0)) {
			FILE *fp = popen("cat /proc/driver/icatchtek_wlan/ipaddr | awk -F ':' '{print $4}'", "r");
			if(fp) {
				fgets(dns, sizeof(dns), fp);
				pclose(fp);
				if(strlen(dns) <= 0) {
					usleep(100000);
					continue;
				}
				for (i = 0; i < 5; i ++) {
					fp = fopen("/tmp/resolv.conf.auto", "w");
					sprintf(cmd, "nameserver %s", dns);
					int ret = fwrite(cmd, 1, strlen(cmd), fp);
					if(ret < 0 ) {
						usleep(100000);
						fclose(fp);
						continue;
					}
					fflush(fp);
					fclose(fp);
					fp = fopen("/tmp/resolv.conf.auto", "r");
					memset(cmd, 0, 64);
					fseek(fp, 0, SEEK_END);
					int size = ftell(fp);
					//printf("size : %d\n", size);
					fseek(fp, 0, SEEK_SET);
					ret = fread(cmd, 1, size, fp);
					if(!strlen(cmd)) {
						printf("DNS Error\n");
						fclose(fp);
						continue;
					}
					//printf("%s(%d)\n", __FUNCTION__, __LINE__);
					//printf("%s - %d\n", cmd, i);
					if(i >= 4) {
						//dbgTestPSL(cmd);
					}
					fclose(fp);
					sleep(1);
				}
				break;

			}
		}
		if(j++ >= 5) {
			break;
		};
		usleep(100000);
	}
}

/**
 * linux send data to icatos by this call bnck
 * */
static int xmitCallback(char* s,int len)
{
	returnIfErrC(-1, !_sdioBuf);
	returnIfErrC(-1, len > 4096);
	memcpy(_sdioBuf,s,len);
	char buf[1024];

	sprintf(buf,"cmd=omfCmdToHost,sel=0xFFFD,data=%d,size=%d",(int)_sdioBuf,(int)len);
	std::string para = buf;
	if(omfCommand("shm0_cmd",para.c_str(), 0)){
		//printf("send to icatos sdio\n");
	}
	{
		mt7687_packet_t *packet = (mt7687_packet_t*)s;
		if((packet->header.type & 0xFFF) == IOT_CMD_REQUEST_LOW_POWER ) {
			icatchtek_mt7682_io_deinit();
			printf("Recv :IOT_CMD_REQUEST_LOW_POWER\n");
			char cmd[512] = {0};
			const char* wlanSSIDName = "/proc/driver/icatchtek_wlan/ssid";
			int wlanIfExist = access(wlanSSIDName, 0);
			if (!wlanIfExist) {
				//disbale wlan interface
				memset(cmd, 0, 512);

				sprintf(cmd, "ifconfig mtwlan0 down");
				system(cmd);
				//dbgTestPSL(cmd);

				memset(cmd, 0, 512);
				//remove wlan driver.
				sprintf(cmd, "rmmod icatchtek_wlan");
				system(cmd);
				//dbgTestPSL(cmd);

				memset(cmd, 0, 512);
				sprintf(cmd, "rmmod icatchtek_sdio");
				//dbgTestPSL(cmd);
				system(cmd);

			} else {
				printf("Quit\n");
			}
		}
		if((packet->header.type & 0xFFF) == IOT_CMD_WIFI_STA_IP_READY) {
			ifconfig_ready = 1;
			struct in_addr ipaddr, gw, nm, dns;
			char ipStr[24] = {0};
			char cmd[128] = {0};
			char result[BUF_LENGTH] = {0};
			packet->data = (unsigned char*)(s + sizeof(mt7687_packet_t));
			struct net_ip_config *ipInfo = (struct net_ip_config *)(packet->data );
			ipaddr.s_addr = ipInfo->ip;
			gw.s_addr = ipInfo->gw;
			nm.s_addr = ipInfo->netmask;
			dns.s_addr = ipInfo->dns[0];
#if 0
			sprintf(ipStr, "%s", inet_ntoa(ipaddr));
			printf("set ip info(%d) : %s\n", __LINE__, ipStr);
			sprintf(cmd, "ifconfig mtwlan0 %s netmask %s", ipStr, inet_ntoa(nm));
			//printf("%s\n", cmd);
			system(cmd);

			memset(cmd, 0, 128);
			sprintf(cmd, "route add default gw %s", inet_ntoa(gw));
			//printf("%s\n", cmd);
			system(cmd);
#endif
			memset(cmd, 0, 128);
			//sprintf(cmd, "echo nameserver %s > /etc/resolv.conf", inet_ntoa(dns));
			sprintf(cmd, "rm -f /etc/resolv.conf && ln -s /proc/driver/icatchtek_wlan/resolv.conf /etc/resolv.conf");
			if(doCmd(cmd, "r", result, BUF_LENGTH)) {
				return -1;
			}
		}
	}
	return 0;
}

/**
 * call sh to connect wifi ,the sh will insmod wlan and sdio ko then connect wifi
 * */
static void wifiConnect(const char* ssid,const char* pwd, struct cipher_info *info)
{
	bool bRet = _wifi_prepare(info);
	if(bRet == false) {
		printf("Wlan driver install error\n");
		return;
	}
	bRet = _wifi_connect_ap(std::string(ssid),std::string(pwd));
	if(bRet == false) {
		printf("Wlan Connect error:\n SSID:%s PSK:%s\n", ssid, pwd ? pwd : "");
		return;
	}
	while(!ifconfig_ready){
		usleep(20*1000ull);
	}
}

/**
 * swit sdio to linux or icatos
 * */
static void sdioSwitch(int method){
	char buf[128];
	sprintf(buf,"cmd=omfCmdToHost,sel=%d",method);
	std::string para = buf;
	if(omfCommand("shm0_cmd", para.c_str(), buf)){
		dbgTestPSL(buf);
		void *attr0 = omfCreateAttrSet(buf);
		_sdioBuf = (char*)omfAttrGetInt(attr0, "sdioRecvBuf");
		if(!_sdioBuf) printf("sdio buf get failed!!!");
		omfDestroy(attr0);
	}
}

/**deal with data from icatos**/
static int msgCbReceive(void*hd,const void *data, int size, int sender, int target, unsigned flags){
	if(data){
		printf("data:%s\n",(const char*)data);
		void* ap = omfCreateAttrSet((const char*)data);
		returnIfErrC(-1, !ap);
		char* data = (char*)omfAttrGetInt(ap,"data");
		int size = omfAttrGetInt(ap,"size");
		if(icatchtek_mt7682_send(data + 4, size - 4) != 0)
			printf("icatchtek_mt7682_send fail!\n");
	}
	return 1;
}
/**regist callback to deal with data from icatos**/
static int msgProcess() {
	void* site=omfMsgSite0Get();
	returnIfErrC(-1,!site);
	returnIfErrC(-1,!omfMsgSiteIsWorking(site));
	//dbgTestPSL(omfMsgSiteGetID(site));
	return omfMsgSiteRegister1(site,_sdioMsgId,&msgCbReceive,0);
}
/**
 * unregist msg call back
 * */
static int msgClose() {
	void* site=omfMsgSite0Get();
	return omfMsgSiteUnRegister1(site,_sdioMsgId);
}

int main(int argc, char** argv ){

	int ret = 0;
	omfInit(0);

	msgProcess();

	sdioSwitch(SDIO_SWITCH_LINUX);
	struct cipher_info info;
	ret = _get_cipher_info_from_RTOS(&info);
	const char* ssid = 0;
	const char* pwd = 0;
	if(argc > 2) {
		ssid = argv[1];
		pwd = argv[2];
	}
	else{
		char buf[128] = {0};
		std::string para = "cmd=omfGetFromHost,sel=0xFFF6";
		if(omfCommand("shm0_cmd",para.c_str(), buf)){
			void *attr1 = omfCreateAttrSet(buf);
			ssid = omfAttrGetStr(attr1,"ssid");//dbgTestPSL(ssid);
			pwd = omfAttrGetStr(attr1,"pwd");//dbgTestPSL(pwd);
			omfDestroy(attr1);
		}
	}

	if(ssid) wifiConnect(ssid, pwd, &info);
	std::lock_guard<std::mutex> lock(wlan_ifconfig_mtx);
	///judge dns whether ok
	//while(1){
		//auto ret = system("ping www.baidu.com");
		//usleep(2000*1000ull);
	//}

	///wait,for icatos and linux communicate
	//usleep(360000ull*1000000ull);
	//sdioSwitch(SDIO_SWITCH_ICATOS);

	msgClose();

	omfUninit(0);

	return 0;
}

