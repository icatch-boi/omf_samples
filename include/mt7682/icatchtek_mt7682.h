

#define ICATCHTEK_WLAN_NL_TYPE_ADD      0x01
#define ICATCHTEK_WLAN_NL_TYPE_REMOVE   0x02
#define ICATCHTEK_WLAN_NL_TYPE_XMIT     0x04
#define ICATCHTEK_WLAN_NL_TYPE_LINK     0x08

typedef int (*ReceiveCallback)(char*s, int len);
int icatchtek_mt7682_io_init(ReceiveCallback receiveCallback);
int icatchtek_mt7682_io_deinit();
int icatchtek_mt7682_send_by_type(char*s, int len, int type);
int icatchtek_mt7682_send(char*s, int len);