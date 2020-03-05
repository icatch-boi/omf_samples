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
extern "C" {
	#include "icatchtek_mt7682.h"
	#include "icatchtek_mt7682_def.h"
}
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
static int xmitCallback(char* s,int len){
	returnIfErrC(-1, !_sdioBuf);
	returnIfErrC(-1, len > 4096);
	memcpy(_sdioBuf,s,len);
	char buf[1024];
	sprintf(buf,"cmd=omfCmdToHost,sel=0xFFFD,data=%d,size=%d",(int)_sdioBuf,(int)len);
	std::string para = buf;
	if(omfCommand("shm0_cmd",para.c_str(),0)){
		printf("send to icatos sdio\n");
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
			struct in_addr ipaddr, gw, nm, dns;
			char ipStr[24] = {0};
			char cmd[128] = {0};
			packet->data = (unsigned char*)(s + sizeof(mt7687_packet_t));
			struct net_ip_config *ipInfo = (struct net_ip_config *)(packet->data );
			ipaddr.s_addr = ipInfo->ip;
			gw.s_addr = ipInfo->gw;
			nm.s_addr = ipInfo->netmask;
			dns.s_addr = ipInfo->dns[0];

			sprintf(ipStr, "%s", inet_ntoa(ipaddr));
			printf("set ip info(%d) : %s\n", __LINE__, ipStr);
			sprintf(cmd, "ifconfig wlan0 %s netmask %s", ipStr, inet_ntoa(nm));
			//printf("%s\n", cmd);
			system(cmd);

			memset(cmd, 0, 128);
			sprintf(cmd, "route add default gw %s", inet_ntoa(gw));
			//printf("%s\n", cmd);
			system(cmd);

			memset(cmd, 0, 128);
			sprintf(cmd, "echo nameserver %s > /tmp/resolv.conf", inet_ntoa(dns));
			//printf("%s\n", cmd);
			system(cmd);
			std::thread t1(dnsSet);
			t1.detach();
		}
	}
	return 0;
}

/**
 * net lonk init,regist call back
 * */
static void netlinkIoInit(void){
	char wlanSSIDName[] = "/proc/driver/icatchtek_wlan/ssid";
	static int netLinkInited = 0;
	int retryCnt = 0;
	while(1) {
		int wlanIfExist = access(wlanSSIDName, 0);
		if (!wlanIfExist) {
			if(!netLinkInited) {
				if(icatchtek_mt7682_io_init(xmitCallback) != 0) {
					usleep(50000); //50ms
					printf("[%s(%d)]netlink init retry %d ...\n" ,__FUNCTION__,__LINE__,++retryCnt);
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

/**
 * call sh to connect wifi ,the sh will insmod wlan and sdio ko then connect wifi
 * */
static void wifiConnect(const char* ssid,const char* pwd){
	std::thread t1(netlinkIoInit);
	t1.detach();
	//connect to wifi
	const char* wlanSSIDName = "/proc/driver/icatchtek_wlan/ssid";
	int wlanIfExist = access(wlanSSIDName, 0);
	if (wlanIfExist) {
		char buf[1024] = {0};
		//dbgTestPSL(buf);
		memset(buf, 0, 1024);
		sprintf(buf,"/usr/scripts/mt_wlan_install.sh -S %s -P %s",ssid,pwd);
		printf("[%s(%d)]connect to wifi(%s,%s)...\n" ,__FUNCTION__,__LINE__, ssid, pwd);
		system(buf);
		printf("[%s(%d)]connect to wifi done!\n" ,__FUNCTION__,__LINE__);
		std::thread t2(dnsSet);
		t2.detach();
	}
}

/**
 * swit sdio to linux or icatos
 * */
static void sdioSwitch(int method){
	char buf[128];
	sprintf(buf,"cmd=omfCmdToHost,sel=%d",method);
	std::string para = buf;
	if(omfCommand("shm0_cmd",para.c_str(),0)){
		//dbgTestPSL(para.c_str());
		void *attr0 = omfCreateAttrSet(para.c_str());
		_sdioBuf = (char*)omfAttrGetInt(attr0,"sdioRecvBuf");
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

int main(int argc,char** argv ){

	omfInit(0);

	msgProcess();

	sdioSwitch(SDIO_SWITCH_LINUX);

	const char* ssid = 0;
	const char* pwd = 0;
	if(argc > 2) {
		ssid = argv[1];
		pwd = argv[2];
	}
	else{
		std::string para = "cmd=omfGetFromHost,sel=0xFFF6";
		if(omfCommand("shm0_cmd",para.c_str(),0)){
			void *attr1 = omfCreateAttrSet(para.c_str());
			ssid = omfAttrGetStr(attr1,"ssid");//dbgTestPSL(ssid);
			pwd = omfAttrGetStr(attr1,"pwd");//dbgTestPSL(pwd);
			omfDestroy(attr1);
		}
	}

	if(ssid) wifiConnect(ssid,pwd);

	///judge dns whether ok
	//while(1){
		//auto ret = system("ping www.baidu.com");
		//usleep(2000*1000ull);
	//}

	///wait,for icatos and linux communicate
	usleep(360000ull*1000000ull);

	sdioSwitch(SDIO_SWITCH_ICATOS);

	msgClose();

	omfUninit(0);

	return 0;
	}

