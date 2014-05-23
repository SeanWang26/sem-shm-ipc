
#ifndef DHSYSABLE_H
#define DHSYSABLE_H


//////////////////////////////////////////////////////////////////////////
//	��ѯ����
typedef enum
{
	ABILITY_DYNAMIC_CONNECT = 1,        // ��ѯ��̬����������
	ABILITY_WATERMARK_CFG = 17,			// ˮӡ��������
	ABILITY_WIRELESS_CFG = 18,			// wireless��������
	ABILITY_DEVALL_INFO = 26,			// �豸�������б�
	ABILITY_CARD_QUERY = 0x0100,		// ���Ų�ѯ����
	ABILITY_MULTIPLAY = 0x0101,			// �໭��Ԥ������
	ABILITY_QUICK_QUERY_CFG = 0x0102,	// ���ٲ�ѯ��������
	ABILITY_INFRARED = 0x0121,			// ���߱�������
	ABILITY_TRIGGER_MODE = 0x0131,		// �������������ʽ����
	ABILITY_DISK_SUBAREA = 0x0141,		// ����Ӳ�̷�������
	ABILITY_DSP_CFG = 0x0151,			// ��ѯDSP����
	ABILITY_STREAM_MEDIA = 0x0161,		// ��ѯSIP,RTSP����
	ABILITY_INTELLI_TRACKER = 0x0171,   // ��ѯ���ܸ�������
} DH_SYS_ABILITY;

//////////////////////////////////////////////////////////////////////////
//	�豸֧�ֹ����б�
enum 
{
	EN_FTP = 0,							// FTP ��λ��1������¼���ļ� 2������ץͼ�ļ�
	EN_SMTP,							// SMTP ��λ��1�����������ı��ʼ� 2����������ͼƬ 3:֧�ֽ����ʼ�����
	EN_NTP,								// NTP	��λ��1������ϵͳʱ��
	EN_AUTO_MAINTAIN,					// �Զ�ά�� ��λ��1������ 2���ر� 3:ɾ���ļ�
	EN_VIDEO_COVER,						// �����ڵ� ��λ��1���������ڵ�
	EN_AUTO_REGISTER,					// ����ע��	��λ��1��ע���sdk������½
	EN_DHCP,							// DHCP	��λ��1��DHCP
	EN_UPNP,							// UPNP	��λ��1��UPNP
	EN_COMM_SNIFFER,					// ����ץ�� ��λ��1:CommATM
	EN_NET_SNIFFER,						// ����ץ�� ��λ�� 1��NetSniffer
	EN_BURN,							// ��¼���� ��λ��1����ѯ��¼״̬
	EN_VIDEO_MATRIX,					// ��Ƶ���� ��λ��1���Ƿ�֧����Ƶ���� 2:�Ƿ�֧��SPOT��Ƶ���� 3:�Ƿ�֧��HDMI��Ƶ����
	EN_AUDIO_DETECT,					// ��Ƶ��� ��λ��1���Ƿ�֧����Ƶ���
	EN_STORAGE_STATION,					// �洢λ�� ��λ��1��Ftp������(Ips) 2��SMB 3��NFS 4��ISCSI 16��DISK 17��U��
	EN_IPSSEARCH,						// IPS�洢��ѯ ��λ��1��IPS�洢��ѯ	
	EN_SNAP,							// ץͼ  ��λ��1���ֱ���2��֡��3��ץͼ��ʽ4��ץͼ�ļ���ʽ5��ͼ������
	EN_DEFAULTNIC,						// ֧��Ĭ��������ѯ ��λ 1��֧��
	EN_SHOWQUALITY,						// CBRģʽ����ʾ���������� ��λ 1:֧��
	EN_CONFIG_IMEXPORT,					// ���õ��뵼���������� ��λ 1:֧��
	EN_LOG,								// �Ƿ�֧�ַ�ҳ��ʽ����־��ѯ ��λ 1��֧��
	EN_SCHEDULE,						// ¼�����õ�һЩ���� ��λ 1:���� 2:Ԥ¼ 3:¼��ʱ���
	EN_NETWORK_TYPE,					// �������Ͱ�λ��ʾ 1:��̬�� 2:���߾��� 3:CDMA/GPRS 4:CDMA/GPRS����������
	EN_MARK_IMPORTANTRECORD,			// ��ʶ��Ҫ¼�� ��λ:1��������Ҫ¼���ʶ
	EN_ACFCONTROL,						// �֡�ʿ��� ��λ��1��֧�ֻ֡�ʿ���, 2:֧�ֶ�ʱ�������ͻ֡�ʿ���(��֧�ֶ���),��������ACF��������
	EN_MULTIASSIOPTION,					// ��·������ ��λ��1��֧����·������, 2:֧�ָ���������ѹ����ʽ��������
	EN_DAVINCIMODULE,					// �����ģ�� ��λ��1��ʱ���ֿ����� 2:��׼I֡�������
	EN_GPS,                             // GPS���� ��λ��1��Gps��λ����
	EN_MULTIETHERNET,					// ֧�ֶ�������ѯ ��λ 1��֧��
	EN_LOGIN_ATTRIBUTE,                 // Login���� ��λ��1��֧��Login���Բ�ѯ
	EN_RECORD_GENERAL,					// ¼����� ��λ��1����ͨ¼��2������¼��3����̬���¼��4�����ش洢��5��Զ�̴洢��6������洢��7�����ؽ����洢��8��֧����������������Զ�̴洢
	EN_JSON_CONFIG,						// Json��ʽ����:��λ��1֧��Json��ʽ, 2: ʹ��F6��NAS����, 3: ʹ��F6��Raid����, 4��ʹ��F6��MotionDetect����, 5������֧����������(V3)��ͨ��F6�������
	EN_HIDE_FUNCTION,					// ���ι��ܣ���λ��1������PTZ��̨����, 2: ����3G�ı���ʱ�ι���
	EN_DISK_DAMAGE,                     // Ӳ�̻�����Ϣ֧������: ��λ��1��Ӳ�̻�����Ϣ��ѯ֧������
	EN_PLAYBACK_SPEED_CTRL,				// ֧�ֻط������ٶȿ���:��λ:1��֧�ֻطż���
	EN_HOLIDAYSCHEDULE,					// ֧�ּ���ʱ�������:��λ:1��֧�ּ���ʱ�������
	EN_FETCH_MONEY_TIMEOUT,				// ATMȡǮ��ʱ
	EN_BACKUP_VIDEO_FORMAT,				// ����֧�ֵĸ�ʽ����λ��1:DAV, 2:ASF
	EN_QUERY_DISK_TYPE,                 // ֧��Ӳ�����Ͳ�ѯ
	EN_CONFIG_DISPLAY_OUTPUT,           // ֧���豸��ʾ�����VGA�ȣ�����,��λ: 1:����ָ���Ѳ����
	EN_SUBBITRATE_RECORD_CTRL,          // ֧����չ����¼���������, ��λ��1-������¼���������
	EN_IPV6,                            // ֧��IPV6����, ��λ��1-IPV6����
	EN_SNMP,                            // SNMP�����������Э�飩
	EN_QUERY_URL,                       // ֧�ֻ�ȡ�豸URL��ַ, ��λ: 1-��ѯ����URL��ַ
	EN_ISCSI,							// ISCSI��InternetС�ͼ����ϵͳ�ӿ����ã�
	EN_RAID,							// ֧��Raid����
	EN_HARDDISK_INFO,					// ֧�ִ�����ϢF5��ѯ
	EN_PICINPIC,                        // ֧�ֻ��л����� ��λ:1,���л�����; 2,���л�Ԥ����¼��洢����ѯ������;3,֧�ֻ��л��������ã�ͬʱ֧�ֻ��л�ͨ����ѯ
	EN_PLAYBACK_SPEED_CTRL_SUPPORT,     // ͬ EN_PLAYBACK_SPEED_CTRL ��ֻΪ�˼���Э��
	EN_LF_XDEV,							// ֧��24��32��64·LF-Xϵ�У���ע�����豸����ı����������㷽ʽ
	EN_DSP_ENCODE_CAP,					// F5 DSP����������ѯ
	EN_MULTICAST,                       // �鲥������ѯ
	EM_NET_LIMIT,                       // ��������������ѯ����λ��1-���緢��������С��                          
	EM_COM422,                          // ����422
	EM_PROTOCAL_FRAMEWORK,				// �Ƿ�֧������Э���ܣ���Ҫʵ��listMethod(),listService()����ͨ��F6�������
	EM_WRITE_DISK_OSD,					// ��¼OSD����, ��λ, 1-��¼OSD��������
	EM_DYNAMIC_MULTI_CONNECT,			// ��̬������, ��λ, 1-������Ƶ����Ӧ��
	EM_CLOUDSERVICE,                    // �Ʒ���,��λ��1-֧�ִ��Ʒ���
};

typedef struct 
{
	DWORD IsFucEnable[512];				// �����б�������,�±��Ӧ������ö��ֵ,��λ��ʾ�ӹ���
} DH_DEV_ENABLE_INFO;

//////////////////////////////////////////////////////////////////////////
//	���Ų�ѯ�����ṹ��
typedef struct 
{
	char		IsCardQueryEnable;
	char		iRev[3];
} DH_CARD_QUERY_EN;

//////////////////////////////////////////////////////////////////////////
//	wireless�����ṹ��
typedef struct 
{
	char		IsWirelessEnable;
	char		iRev[3];
} DH_WIRELESS_EN;

//////////////////////////////////////////////////////////////////////////
//	ͼ��ˮӡ�����ṹ��
typedef struct 
{
	char		isSupportWM;			// 1 ֧�֣�0 ��֧��
	char		supportWhat;			// 0������ˮӡ��1��ͼƬˮӡ��2��ͬʱ֧������ˮӡ��ͼƬˮӡ
	char		reserved[2];
} DH_WATERMAKE_EN;

//////////////////////////////////////////////////////////////////////////
//	�໭��Ԥ�������ṹ��
typedef struct  
{
	int			nEnable;				// 1 ֧�֣�0 ��֧��
	DWORD		dwMultiPlayMask;		// �໭��Ԥ������
	char		reserved[4];			// ����
} DH_MULTIPLAY_EN;

//////////////////////////////////////////////////////////////////////////
//	���߱��������ṹ��
typedef struct  
{
	BOOL		bSupport;				// �Ƿ�֧��
	int			nAlarmInCount;			// �������
	int			nAlarmOutCount;			// �������
	int			nRemoteAddrCount;		// ң��������
	BYTE		reserved[32];
} DH_WIRELESS_ALARM_INFO;

//////////////////////////////////////////////////////////////////////////
//	����Ӳ�̷��������ṹ��
typedef struct 
{
	BOOL		bSupported;				// �Ƿ�֧������Ӳ�̷���
	int			nSupportNum;			// ֧�ַ�������Ŀ
	BYTE		bReserved[32];			// �����ֽ�
} DH_DISK_SUBAREA_EN;

//	DSP������ѯ����DSP�����㷨��ʶΪ2ʱʹ�á�
typedef struct  
{
	BYTE		bMainFrame[32];			// �Էֱ���ö��ֵ(CAPTURE_SIZE)Ϊ����,�������ֱ��ʶ�Ӧ֧�ֵ����֡��,�����֧�ִ˷ֱ���,��ֵΪ0.
	BYTE		bExtraFrame_1[32];		// ������1,ʹ��ͬbMainFrame
	BYTE		bReserved[128];			// Ԥ����������2��3.		
} DH_DSP_CFG_ITEM;

typedef struct  
{
	int			nItemNum;				// DH_DSP_CFG_ITEM����Ч����,����ͨ����
	DH_DSP_CFG_ITEM	stuDspCfgItem[32];	// ����������Ϣ
	BYTE		bReserved[128];			// ����
} DH_DSP_CFG; 

//////////////////////////////////////////////////////////////////////////
//	���ٲ�ѯ���������ṹ��
typedef struct 
{
	char		IsQuickQueryEnable;		// 1���豸֧������������ٷ��أ�����������ʱ���㹻���Ա�֤խ�������õ�������ȡ����������60S
	char		iRev[3];
} DH_QUICK_QUERY_CFG_EN;

typedef struct  
{
	int			nStreamType;			// 0,�� 1,SIP 2,RTSP
	BYTE		bReserved[16];			// ����
} DH_STREAM_MEDIA_EN;

//���������������ѯ
typedef struct
{
	char		IsIntelliTrackerEnable;		// ����0��ʾ�����������������
	BYTE		bReserved[3];
}DH_INTELLI_TRACKER_EN;


#endif // DHSYSABLE_H

