#ifndef NETMODULE_H
#define NETMODULE_H

#ifdef WIN32

#ifdef NETMODULE_EXPORTS
#define H264_DVR_API  extern "C" __declspec(dllexport)
#else
#define H264_DVR_API  extern "C" __declspec(dllimport)   //VC ��
#endif

#ifndef CALL_METHOD
#define CALL_METHOD	__stdcall  //__cdecl
#endif

#else	//linux
#define H264_DVR_API	extern "C"
#define CALL_METHOD
#define CALLBACK
#define WORD	unsigned short
#define DWORD	unsigned long
#define LPDWORD	DWORD*
#define BOOL	int
#define TRUE	1
#define FALSE	0
#define BYTE	unsigned char
#define LONG	long
#define UINT	unsigned int
#define HDC		void*
#define HWND	void*
#define LPVOID	void*
//#define NULL	0
#define LPCSTR  char*
#define LPCTSTR  const char*
#endif


enum SDK_RET_CODE
{	
	H264_DVR_NOERROR					= 0,					//û�д���
	H264_DVR_SUCCESS					= 1,					//���سɹ�
	H264_DVR_SDK_NOTVALID			= -10000,				//�Ƿ�����
	H264_DVR_NO_INIT					= -10001,				//SDKδ����ʼ��
	H264_DVR_ILLEGAL_PARAM			= -10002,			//�û��������Ϸ�
	H264_DVR_INVALID_HANDLE			= -10003,			//�����Ч
	H264_DVR_SDK_UNINIT_ERROR		= -10004,			//SDK�������
	H264_DVR_SDK_TIMEOUT				= -10005,			//�ȴ���ʱ
	H264_DVR_SDK_MEMORY_ERROR		= -10006,			//�ڴ���󣬴����ڴ�ʧ��
	H264_DVR_SDK_NET_ERROR			= -10007,			//�������
	H264_DVR_SDK_OPEN_FILE_ERROR		= -10008,			//���ļ�ʧ��
	H264_DVR_SDK_UNKNOWNERROR		= -10009,			//δ֪����
	H264_DVR_DEV_VER_NOMATCH			= -11000,			//�յ����ݲ���ȷ�����ܰ汾��ƥ��
	H264_DVR_SDK_NOTSUPPORT			= -11001,			//�汾��֧��

	H264_DVR_OPEN_CHANNEL_ERROR		= -11200,			//��ͨ��ʧ��
	H264_DVR_CLOSE_CHANNEL_ERROR		= -11201,			//�ر�ͨ��ʧ��
	H264_DVR_SUB_CONNECT_ERROR		= -11202,			//����ý��������ʧ��
	H264_DVR_SUB_CONNECT_SEND_ERROR	= -11203,			//ý��������ͨѶʧ��
	H264_DVR_NATCONNET_REACHED_MAX  = -11204,           //Nat��Ƶ���Ӵﵽ��󣬲������µ�Nat��Ƶ���� 

	

	/// �û������ִ�����
	H264_DVR_NOPOWER					= -11300,			//��Ȩ��
	H264_DVR_PASSWORD_NOT_VALID		= -11301,			// �˺����벻��
	H264_DVR_LOGIN_USER_NOEXIST		= -11302,			//�û�������
	H264_DVR_USER_LOCKED				= -11303,			// ���û�������
	H264_DVR_USER_IN_BLACKLIST		= -11304,			// ���û����������(�ں�������)
	H264_DVR_USER_HAS_USED			= -11305,			// ���û��Ե�½
	H264_DVR_USER_NOT_LOGIN			= -11306,			// ���û�û�е�½
	H264_DVR_CONNECT_DEVICE_ERROR    = -11307,			//�����豸������
	H264_DVR_ACCOUNT_INPUT_NOT_VALID = -11308,			//�û��������벻�Ϸ�
	H264_DVR_ACCOUNT_OVERLAP			= -11309,			//�����ظ�
	H264_DVR_ACCOUNT_OBJECT_NONE		= -11310,			//�����ڶ���, ���ڲ�ѯʱ
	H264_DVR_ACCOUNT_OBJECT_NOT_VALID= -11311,			//�����ڶ���
	H264_DVR_ACCOUNT_OBJECT_IN_USE	= -11312,			//��������ʹ��
	H264_DVR_ACCOUNT_SUBSET_OVERLAP	= -11313,			//�Ӽ�����Χ (�����Ȩ�޳���Ȩ�ޱ��û�Ȩ�޳������Ȩ�޷�Χ�ȵ�)
	H264_DVR_ACCOUNT_PWD_NOT_VALID	= -11314,			//���벻��ȷ
	H264_DVR_ACCOUNT_PWD_NOT_MATCH	= -11315,			//���벻ƥ��
	H264_DVR_ACCOUNT_RESERVED		= -11316,			//�����ʺ�


	/// ���ù�����ش�����
	
	H264_DVR_OPT_RESTART				= -11400,			// �������ú���Ҫ����Ӧ�ó���
	H264_DVR_OPT_REBOOT				= -11401,			// ��Ҫ����ϵͳ
	H264_DVR_OPT_FILE_ERROR			= -11402,			// д�ļ�����
	H264_DVR_OPT_CAPS_ERROR			= -11403,			// �������Բ�֧��
	H264_DVR_OPT_VALIDATE_ERROR		= -11404,			// ����У��ʧ��
	H264_DVR_OPT_CONFIG_NOT_EXIST	= -11405,			// ����������õ����ò�����
	
	/// 

	H264_DVR_CTRL_PAUSE_ERROR		= -11500,			//��ͣʧ��
	H264_DVR_SDK_NOTFOUND			= -11501,			//����ʧ�ܣ�û���ҵ���Ӧ�ļ�
	H264_DVR_CFG_NOT_ENABLE         = -11502,           //����δ����

	H264_DVR_DECORD_FAIL            = -11503,           //����δ����

	//DNSЭ��������ش�����
	H264_DVR_SOCKET_ERROR             = -11600,         //�����׽���ʧ��
	H264_DVR_SOCKET_CONNECT           = -11601,         //�����׽���ʧ��
	H264_DVR_SOCKET_DOMAIN            = -11602,         //��������ʧ��
	H264_DVR_SOCKET_SEND              = -11603,         //��������ʧ��
	H264_DVR_ARSP_NO_DEVICE           = -11604,         //û�л�ȡ���豸��Ϣ���豸Ӧ�ò�����
	H264_DVR_ARSP_BUSING              = -11605,         //ARSP����æ
	H264_DVR_ARSP_BUSING_SELECT       = -11606,         //ARSP����æ,selectʧ��
	H264_DVR_ARSP_BUSING_RECVICE	  = -11607,         //ARSP����æ,recviceʧ��
	H264_DVR_CONNECTSERVER_ERROR      = -11608,        //���ӷ�����ʧ��

};


//��̨��������
enum PTZ_ControlType
{
	TILT_UP = 0,			//��
	TILT_DOWN,				//��
	PAN_LEFT,				//��
	PAN_RIGHT,				//��
	PAN_LEFTTOP,			//����
	PAN_LEFTDOWN,			//����
	PAN_RIGTHTOP,			//����
	PAN_RIGTHDOWN,			//����
	ZOOM_OUT,				//�䱶С
	ZOOM_IN,				//�䱶��
	FOCUS_FAR,				//������
	FOCUS_NEAR,				//����ǰ��
	IRIS_OPEN,				//��Ȧ����
	IRIS_CLOSE,				//��Ȧ��С

	EXTPTZ_OPERATION_ALARM,			///< �������� 
	EXTPTZ_LAMP_ON,					///< �ƹ⿪
	EXTPTZ_LAMP_OFF,				//�ƹ��
	EXTPTZ_POINT_SET_CONTROL,			//����Ԥ�õ� 
	EXTPTZ_POINT_DEL_CONTROL,			//���Ԥ�õ� 
	EXTPTZ_POINT_MOVE_CONTROL,			//תԤ�õ�
	EXTPTZ_STARTPANCRUISE,			//��ʼˮƽ��ת			
	EXTPTZ_STOPPANCRUISE,			//ֹͣˮƽ��ת	
	EXTPTZ_SETLEFTBORDER,			//������߽�		
	EXTPTZ_SETRIGHTBORDER,			//�����ұ߽�	
	EXTPTZ_STARTLINESCAN,			//�Զ�ɨ�迪ʼ 
	EXTPTZ_CLOSELINESCAN,			//�Զ�ɨ�迪ֹͣ 
	EXTPTZ_ADDTOLOOP,				//����Ԥ�õ㵽Ѳ��	p1Ѳ����·	p2Ԥ�õ�ֵ	
	EXTPTZ_DELFROMLOOP,				//ɾ��Ѳ����Ԥ�õ�	p1Ѳ����·	p2Ԥ�õ�ֵ	
	EXTPTZ_POINT_LOOP_CONTROL,			//��ʼѲ��
	EXTPTZ_POINT_STOP_LOOP_CONTROL,	//ֹͣѲ��
	EXTPTZ_CLOSELOOP,				//���Ѳ��	p1Ѳ����·		
	EXTPTZ_FASTGOTO,				//���ٶ�λ	
	EXTPTZ_AUXIOPEN,				//�������أ��ر�����������//param1 �μ�SDK_PtzAuxStatus��param2���������ֵ
	EXTPTZ_OPERATION_MENU,				//����˵����������а��������أ�ȷ���ȵ�
	EXTPTZ_REVERSECOMM,				//��ͷ��ת
	EXTPTZ_OPERATION_RESET,			///< ��̨��λ

	EXTPTZ_TOTAL,
};
/// ��̨��������
enum SDK_PtzAuxStatus
{
	EXTPTZ_AUX_ON,		///< ��
	EXTPTZ_AUX_OFF,	///< ��
	EXTPTZ_AUX_NR,		
};

#define PAN_AUTO		29	/* ��̨��SS���ٶ������Զ�ɨ�� */
#define EXTPTZ_FASTGOTO	30	//��ά��λ


#define NET_MAX_CHANNUM 			32			//���ͨ������
#define NET_DECORDR_CH              16          //������ͨ������
#define NET_MAX_USER_NUM			60			//����û���
#define NET_MAX_RIGTH_NUM			128			//���Ȩ����
#define NET_MAX_GROUP_NUM			50			//�������
#define NET_MAX_USER_LENGTH			32			//�û���������󳤶�
#define NET_MAX_COMBINE_NUM			2			//�����ϱ���ͨ����
#define NET_MAX_DECORDR_CH          32          //������ͨ������	

#define NET_MAX_DDNS_TYPE			5			//֧�ֵ�DDNS����
#define NET_MAX_ARSP_TYPE			5
#define NET_MAX_ALARMSERVER_TYPE    5           //֧�ֱ�����������
#define NET_MAX_SYSFUNC				20			//���ϵͳ���ܸ���
#define NET_MAX_PTZ_PROTOCOL_LENGTH 32			//��̨Э��������󳤶�
#define NET_N_WEEKS					7			//������
#define	NET_N_TSECT					6			//ʱ�����
#define NET_MD_REGION_ROW			32			//��̬�����������
#define NET_COVERNUM				8			 //����������
#define NET_MAX_FILTERIP_NUM		64			//IP��ַ��������
#define NET_NAME_PASSWORD_LEN		64			//�û���������󳤶�
#define NET_MAX_PATH_LENGTH	        260			//·������
#define NET_N_MIN_TSECT				2
#define NET_MAX_RETURNED_LOGLIST	128			//�����־����
#define NET_MAX_MAC_LEN             32          //MAC��ַ�ַ���󳤶�
#define NET_IW_ENCODING_TOKEN_MAX   128
#define NET_MAX_AP_NUMBER           32          //SID����������ݶ�10
#define NET_MAX_INFO_LEN		    128
#define NET_MAX_USERNAME_LENGTH		128
#define NET_MAX_SERIALNO_LENGTH		128          //������ͨ������


//DDNS����
#define DDNS_MAX_DDNS_NAMELEN 64  //����������
#define DDNS_MAX_DDNS_PWDLEN 32   //���볤��
#define DDNS_MAX_DDNS_IPSIZE 64   //IP��ַ����
#define DDNS_MAX_DDNS_IDSIZE 32   //�豸��ʶ����
#define DDNS_MAX_SERIALINFO_SIZE 16 //���к��Լ��û�������



//���������
#define CAMERAPARA_MAXNUM  16    //�ع�������Ŀǰ��󳤶�
//�����������
#define MAX_RECIVE_MSG_PHONE_COUNT 3	///<�������������

//VGA�ֱ���
#define VGA_MAXNUM     32       //�ֱ��� ����
#define VGA_NAME_LENGTH 10		//�ֱ��� ����	

//��ʾ���豸�б�(�豸������)
#define DEV_LIST_SHOW  128

//#define  MAX_HVR_CHNCAP       16
//DDNS�������豸����Ϣ
typedef struct _DDNS_INFO
{
	char ID[64];    //�豸��ʶ
	char Alias[DDNS_MAX_DDNS_NAMELEN];
	char IP[DDNS_MAX_DDNS_IPSIZE];   //����IP
	char LanIP[DDNS_MAX_DDNS_IPSIZE];//����IP
	int WebPort; //Web�˿�,Ĭ��Ϊ80
	int MediaPort; //ý��˿�,Ĭ��Ϊ34567
	int MobilePort;  //�ֻ���ض˿ڣ�Ĭ��Ϊ34599
	int UPNPWebPort;  //UPNP������Web�˿�,UPNP������Ϊ0
	int UPNPMediaPort; //UPNP������ý��˿�,UPNP������Ϊ0
	int UPNPMobilePort; //UPNP�������ֻ���ض˿�,UPNP������Ϊ0
	int ChannelNum; //ͨ����
	int Status; //״̬
	char Username[DDNS_MAX_DDNS_NAMELEN]; //�û���
	char Password[DDNS_MAX_DDNS_PWDLEN]; //����
}DDNS_INFO,*pDDNS_INFO;

//DDNS��Ϣ��ѯģʽ
typedef struct _SearchMode
{
	int  nType;        //��ѯ���ͣ���SearchModeType
	char szSerIP[DDNS_MAX_DDNS_IPSIZE];//��������ַ
	int nSerPort;           //�������˿ں�
	char szSerialInfo[DDNS_MAX_SERIALINFO_SIZE];  //����ǰ����кţ���Ϊ���кţ�������û�������Ϊ�û���
	char szPassWord[DDNS_MAX_DDNS_PWDLEN]; //���û�����½ʱ�������

}SearchMode;

//DDNS��Ϣ��ѯ����
enum SearchModeType
{
	DDNS_SERIAL = 0,//�����к�
	DDNS_USERNAME,  //���û���
};
//ʵʱԤ����չ�ӿ����ӵĲ�����Ԥ������
typedef enum _H264_DVR_RealPlayType
{
	NET_RType_Realplay = 0,	//ʵʱԤ��
	NET_RType_Main_Realplay2,	//ʵʱ����-����������ͬ��NET_RType_Realplay
	NET_RType_Main_Realplay3,
	NET_RType_Sub_Realplay_1,	//ʵʱ����-������1
	NET_RType_Sub_Realplay_2,	//ʵʱ����-������2
	NET_RType_Sub_Realplay_3,	//ʵʱ����-������3

	NET_RType_Multiplay_1,	//�໭��Ԥ����1����
	NET_RType_Multiplay_4,	//�໭��Ԥ����4����
	NET_RType_Multiplay_6,	//�໭��Ԥ����6����
	NET_RType_Multiplay_8,	//�໭��Ԥ����8����
	NET_RType_Multiplay_9,	//�໭��Ԥ����9����
	NET_RType_Multiplay_12,	//�໭��Ԥ����12����
	NET_RType_Multiplay_16,	//�໭��Ԥ����16����
} NET_RealPlayType;

/// ��ϱ���ģʽ
enum NetCombinType
{
	NET_COMBIN_NONE,
	NET_COMBIN_1,
	NET_COMBIN_2,
	NET_COMBIN_3,
	NET_COMBIN_4,
	NET_COMBIN_5,
	NET_COMBIN_6,
	NET_COMBIN_7,
	NET_COMBIN_8,
	NET_COMBIN_9,
	NET_COMBIN_10,
	NET_COMBIN_11,
	NET_COMBIN_12,
	NET_COMBIN_13,
	NET_COMBIN_14,
	NET_COMBIN_15,
	NET_COMBIN_16,
	NET_COMBIN_1_4,
	NET_COMBIN_5_8,
	NET_COMBIN_9_12,
	NET_COMBIN_13_16,
	NET_COMBIN_1_8,
	NET_COMBIN_9_16,
	NET_COMBIN_1_9,
	NET_COMBIN_8_16,
	NET_COMBIN_1_16
};

//��Щ�ṹ���ö�����ṩ���ⲿʹ�ã����п��ܻ���豸�Ǳ߶�����2��,���Զ���ǰ�����SDK_
enum SDK_CAPTURE_SIZE_t {
	SDK_CAPTURE_SIZE_D1,		///< 720*576(PAL)	720*480(NTSC)
	SDK_CAPTURE_SIZE_HD1,		///< 352*576(PAL)	352*480(NTSC)
	SDK_CAPTURE_SIZE_BCIF,		///< 720*288(PAL)	720*240(NTSC)
	SDK_CAPTURE_SIZE_CIF,		///< 352*288(PAL)	352*240(NTSC)
	SDK_CAPTURE_SIZE_QCIF,		///< 176*144(PAL)	176*120(NTSC)
	SDK_CAPTURE_SIZE_VGA,		///< 640*480(PAL)	640*480(NTSC)
	SDK_CAPTURE_SIZE_QVGA,		///< 320*240(PAL)	320*240(NTSC)
	SDK_CAPTURE_SIZE_SVCD,		///< 480*480(PAL)	480*480(NTSC)
	SDK_CAPTURE_SIZE_QQVGA,		///< 160*128(PAL)	160*128(NTSC)
	SDK_CAPTURE_SIZE_ND1 = 9,     ///< 240*192
	SDK_CAPTURE_SIZE_650TVL,      ///< 926*576
	SDK_CAPTURE_SIZE_720P,        ///< 1280*720
	SDK_CAPTURE_SIZE_1_3M,        ///< 1280*960
	SDK_CAPTURE_SIZE_UXGA ,       ///< 1600*1200
	SDK_CAPTURE_SIZE_1080P,       ///< 1920*1080
	SDK_CAPTURE_SIZE_WUXGA,       ///< 1920*1200
	SDK_CAPTURE_SIZE_2_5M,        ///< 1872*1408
	SDK_CAPTURE_SIZE_3M,          ///< 2048*1536
	SDK_CAPTURE_SIZE_5M,          ///< 3744*1408
	SDK_CAPTURE_SIZE_NR,
};

/// ������������ģʽ����
enum SDK_capture_brc_t {
	SDK_CAPTURE_BRC_CBR,			///< �̶�������
	SDK_CAPTURE_BRC_VBR,			///< �ɱ�������
	SDK_CAPTURE_BRC_MBR,			///< ���������
	SDK_CAPTURE_BRC_NR				///< ö�ٵ���������ģʽ��Ŀ��
};

/// ����ѹ����ʽ����
enum SDK_CAPTURE_COMP_t {
	SDK_CAPTURE_COMP_DIVX_MPEG4,	///< DIVX MPEG4��
	SDK_CAPTURE_COMP_MS_MPEG4,		///< MS MPEG4��
	SDK_CAPTURE_COMP_MPEG2,			///< MPEG2��
	SDK_CAPTURE_COMP_MPEG1,			///< MPEG1��
	SDK_CAPTURE_COMP_H263,			///< H.263
	SDK_CAPTURE_COMP_MJPG,			///< MJPG
	SDK_CAPTURE_COMP_FCC_MPEG4,		///< FCC MPEG4
	SDK_CAPTURE_COMP_H264,			///< H.264
	SDK_CAPTURE_COMP_NR				///< ö�ٵ�ѹ����׼��Ŀ��
};

/// ����ͨ������
enum SDK_CAPTURE_CHANNEL_t {
	SDK_CHL_MAIN_T = 0,		///< ��ͨ��	��	������1 
	SDK_CHL_2END_T = 1,		///< ��ͨ��	��	��������2 
	SDK_CHL_3IRD_T = 2,		///< ��ͨ��	��	��������3 
	SDK_CHL_4RTH_T = 3,		///< ��ͨ��	��	��������4 
	SDK_CHL_JPEG_T = 4,		///< ��ͨ��	��	��JPEGץͼ 
	SDK_CHL_FUNCTION_NUM
};
/// ��Ƶ��������
enum SDK_AudioEncodeTypes
{
	SDK_AUDIO_ENCODE_NONE = 0,
	SDK_AUDIO_ENCODE_G729_8KBIT,
	SDK_AUDIO_ENCODE_G726_16KBIT,
	SDK_AUDIO_ENCODE_G726_24KBIT,
	SDK_AUDIO_ENCODE_G726_32KBIT,
	SDK_AUDIO_ENCODE_G726_40KBIT,
	SDK_AUDIO_ENCODE_PCM_8TO16BIT,
	SDK_AUDIO_ENCODE_PCM_ALAW,
	SDK_AUDIO_ENCODE_PCM_ULAW,
	SDK_AUDIO_ENCODE_ADPCM8K16BIT,
	SDK_AUDIO_ENCODE_ADPCM16K16BIT,
	SDK_AUDIO_ENCODE_G711_ALAW,
	SDK_AUDIO_ENCODE_MPEG2_LAYER1,
	SDK_AUDIO_ENCODE_AMR8K16BIT,
	SDK_AUDIO_ENCODE_G711_ULAW,
	SDK_AUDIO_ENCODE_IMA_ADPCM_8K16BIT,
	SDK_AUDIO_ENCODE_TYPES_NR,
};


typedef enum _SDK_CONFIG_TYPE
{
	E_SDK_CONFIG_NOTHING = 0,		//
	E_SDK_CONFIG_USER,			//�û���Ϣ��������Ȩ���б��û��б�����б�   ��Ӧ�ṹ��USER_MANAGE_INFO
	E_SDK_CONFIG_ADD_USER,		//�����û�		��Ӧ�ṹ��USER_INFO
	E_SDK_CONFIG_MODIFY_USER,		//�޸��û�	��Ӧ�ṹ��CONF_MODIFYUSER
	E_SDK_CONFIG_DELETE_USER,								//��Ӧ�ṹ��USER_INFO
	E_SDK_CONFIG_ADD_GROUP,		//������			��Ӧ�ṹ��USER_GROUP_INFO
	E_SDK_CONFIG_MODIFY_GROUP,	//�޸���		��Ӧ�ṹ��CONF_MODIFYGROUP
	E_SDK_COFIG_DELETE_GROUP,								//��Ӧ�ṹ��USER_GROUP_INFO
	E_SDK_CONFIG_MODIFY_PSW,		//�޸�����	��Ӧ�ṹ��_CONF_MODIFY_PSW


	E_SDK_CONFIG_ABILITY_SYSFUNC = 9,//֧�ֵ����繦��						��Ӧ�ṹ��SDK_SystemFunction
	E_SDK_CONFIG_ABILTY_ENCODE,	//���Ȼ�ñ�������							��Ӧ�ṹ��CONFIG_EncodeAbility
	E_SDK_CONFIG_ABILITY_PTZPRO,	//��̨Э��										��Ӧ�ṹ��SDK_PTZPROTOCOLFUNC
	E_SDK_COMFIG_ABILITY_COMMPRO,	//����Э��										��Ӧ�ṹ��SDK_COMMFUNC

	E_SDK_CONFIG_ABILITY_MOTION_FUNC,	//��̬����							��Ӧ�ṹ��SDK_MotionDetectFunction
	E_SDK_CONFIG_ABILITY_BLIND_FUNC,	//��Ƶ�ڵ���							��Ӧ�ṹ��SDK_BlindDetectFunction
	E_SDK_CONFIG_ABILITY_DDNS_SERVER,	//DDNS����֧������						��Ӧ�ṹ��SDK_DDNSServiceFunction
	E_SDK_CONFIG_ABILITY_TALK,		//�Խ���������								��Ӧ�ṹ��SDK_DDNSServiceFunction




	E_SDK_CONFIG_SYSINFO = 17,		//ϵͳ��Ϣ										��Ӧ�ṹ��H264_DVR_DEVICEINFO
	E_SDK_CONFIG_SYSNORMAL,	//��ͨ����													��Ӧ�ṹ��SDK_CONFIG_NORMAL
	E_SDK_CONFIG_SYSENCODE,	//��������													��Ӧ�ṹ��SDK_EncodeConfigAll_SIMPLIIFY
	E_SDK_CONFIG_SYSNET,		//��������													��Ӧ�ṹ��SDK_CONFIG_NET_COMMON
	E_SDK_CONFIG_PTZ,			//��̨ҳ��														��Ӧ�ṹ��SDK_STR_PTZCONFIG_ALL
	E_SDK_CONFIG_COMM,		//����ҳ��														��Ӧ�ṹ��SDK_CommConfigAll
	E_SDK_CONFIG_RECORD,		//¼�����ý���											��Ӧ�ṹ��SDK_RECORDCONFIG
	E_SDK_CONFIG_MOTION,		//��̬���ҳ��											��Ӧ�ṹ��SDK_MOTIONCONFIG
	E_SDK_CONFIG_SHELTER,		//��Ƶ�ڵ�													��Ӧ�ṹ��SDK_BLINDDETECTCONFIG
	E_SDK_CONFIG_VIDEO_LOSS,  //��Ƶ��ʧ,												��Ӧ�ṹ��SDK_VIDEOLOSSCONFIG
	E_SDK_CONFIG_ALARM_IN,	//��������													��Ӧ�ṹ��SDK_ALARM_INPUTCONFIG
	E_SDK_CONFIG_ALARM_OUT,	//�������													��Ӧ�ṹ��SDK_AlarmOutConfigAll
	E_SDK_CONFIG_DISK_MANAGER,//Ӳ�̹������										��Ӧ�ṹ��SDK_StorageDeviceControl
	E_SDK_CONFIG_OUT_MODE,	//���ģʽ����    									��Ӧ�ṹ��SDK_VideoWidgetConfi
	E_SDK_CONFIG_CHANNEL_NAME,//ͨ������												��Ӧ�ṹ��SDK_ChannelNameConfigAll
	E_SDK_CONFIG_AUTO,		//�Զ�ά����������										��Ӧ�ṹ��SDK_AutoMaintainConfig
	E_SDK_CONFIG_DEFAULT,     //�ָ�Ĭ�Ͻ�������								��Ӧ�ṹ��SDK_SetDefaultConfigTypes
	E_SDK_CONFIG_DISK_INFO,	//Ӳ����Ϣ													��Ӧ�ṹ��SDK_StorageDeviceInformationAll
	E_SDK_CONFIG_LOG_INFO,	//��ѯ��־													��Ӧ�ṹ��SDK_LogList
	E_SDK_CONFIG_NET_IPFILTER,	//����������										��Ӧ�ṹ��SDK_NetIPFilterConfig
	E_SDK_CONFIG_NET_DHCP,			//DHC����												��Ӧ�ṹ��SDK_NetDHCPConfigAll
	E_SDK_CONFIG_NET_DDNS,			//DDNS��Ϣ											��Ӧ�ṹ��SDK_NetDDNSConfigALL
	E_SDK_CONFIG_NET_EMAIL,			//EMAIL													��Ӧ�ṹ��SDK_NetEmailConfig
	E_SDK_CONFIG_NET_MULTICAST,	//�鲥													��Ӧ�ṹ��SDK_NetMultiCastConfig
	E_SDK_CONFIG_NET_NTP,				//NTP														��Ӧ�ṹ��SDK_NetNTPConfig
	E_SDK_CONFIG_NET_PPPOE,			//PPPOE													��Ӧ�ṹ��SDK_NetPPPoEConfig
	E_SDK_CONFIG_NET_DNS,				//DNS														��Ӧ�ṹ��SDK_NetDNSConfig
	E_SDK_CONFIG_NET_FTPSERVER,	//FTP														��Ӧ�ṹ��SDK_FtpServerConfig

	E_SDK_CONFIG_SYS_TIME,	//ϵͳʱ��													��Ӧ�ṹ��SDK_SYSTEM_TIME
	E_SDK_CONFIG_CLEAR_LOG,	//�����־													
	E_SDK_REBOOT_DEV,		//���������豸													
	E_SDK_CONFIG_ABILITY_LANG,	//֧������											��Ӧ�ṹ��SDK_MultiLangFunction
	E_SDK_CONFIG_VIDEO_FORMAT,																	
	E_SDK_CONFIG_COMBINEENCODE,	//��ϱ���											��Ӧ�ṹ��SDK_CombineEncodeConfigAll
	E_SDK_CONFIG_EXPORT,	//���õ���														
	E_SDK_CONFIG_IMPORT,	//���õ���
	E_SDK_LOG_EXPORT,		//��־����														
	E_SDK_CONFIG_COMBINEENCODEMODE, //��ϱ���ģʽ							��Ӧ�ṹ��SDK_CombEncodeModeAll
	E_SDK_WORK_STATE,	//����״̬																
	E_SDK_ABILITY_LANGLIST, //ʵ��֧�ֵ����Լ�									
	E_SDK_CONFIG_NET_ARSP,	//ARSP															��Ӧ�ṹ��SDK_NetARSPConfigAll
	E_SDK_CONFIG_SNAP_STORAGE,//																��Ӧ�ṹ��SDK_SnapshotConfig
	E_SDK_CONFIG_NET_3G, //3G����																��Ӧ�ṹ��SDK_Net3GConfig
	E_SDK_CONFIG_NET_MOBILE, //�ֻ����													��Ӧ�ṹ��SDK_NetMoblieConfig
	E_SDK_CONFIG_UPGRADEINFO, //��ȡ������Ϣ										���� �ļ���
	E_SDK_CONFIG_NET_DECODER,
	E_SDK_ABILITY_VSTD, //ʵ��֧�ֵ���Ƶ��ʽ
	E_SDK_CONFIG_ABILITY_VSTD,	//֧����Ƶ��ʽ									��Ӧ�ṹ��SDK_MultiVstd
	E_SDK_CONFIG_NET_UPNP, //UPUN����														��Ӧ�ṹ��SDK_NetUPNPConfig
	E_SDK_CONFIG_NET_WIFI,//WIFI																��Ӧ�ṹ��SDK_NetWifiConfig
	E_SDK_CONFIG_NET_WIFI_AP_LIST,//														��Ӧ�ṹ��SDK_NetWifiDeviceAll
	E_SDK_CONFIG_SYSENCODE_SIMPLIIFY, //�򻯵ı�������					��Ӧ�ṹ��SDK_EncodeConfigAll_SIMPLIIFY
	E_SDK_CONFIG_ALARM_CENTER,  //�澯����											��Ӧ�ṹ��SDK_NetAlarmServerConfigAll
	E_SDK_CONFIG_NET_ALARM,																			
	E_SDK_CONFIG_NET_MEGA,     //���Ż�ͨ
	E_SDK_CONFIG_NET_XINGWANG, //����
	E_SDK_CONFIG_NET_SHISOU,   //����
	E_SDK_CONFIG_NET_VVEYE,    //VVEYE
	E_SDK_CONFIG_NET_PHONEMSG,  //����													��Ӧ�ṹ��SDK_NetShortMsgCfg
	E_SDK_CONFIG_NET_PHONEMEDIAMSG,  //����											��Ӧ�ṹ��SDK_NetMultimediaMsgCfg
	E_SDK_VIDEO_PREVIEW,
	E_SDK_CONFIG_NET_DECODER_V2,
	E_SDK_CONFIG_NET_DECODER_V3,//����ͨ��											��Ӧ�ṹ��SDK_NetDecorderConfigAll_V3
	E_SDK_CONFIG_ABILITY_SERIALNO,	// ���к�
	E_SDK_CONFIG_NET_RTSP,    //RTSP														��Ӧ�ṹ��SDK_NetRTSPConfig
	E_SDK_GUISET,              //GUISET													��Ӧ�ṹ��SDK_GUISetConfig
	E_SDK_CATCHPIC,               //ץͼ												
	E_SDK_VIDEOCOLOR,             //��Ƶ��ɫ����
	E_SDK_CONFIG_COMM485,//																			��Ӧ�ṹ��SDK_STR_RS485CONFIG_ALL
	E_SDK_COMFIG_ABILITY_COMMPRO485, //����485									��Ӧ�ṹ��SDK_COMMFUNC
	E_SDK_CONFIG_SYS_TIME_NORTC,	//ϵͳʱ��noRtc								��Ӧ�ṹ��SDK_COMMFUNC
	E_SDK_CONFIG_REMOTECHANNEL,   //Զ��ͨ��										��Ӧ�ṹ��SDK_CONFIG_NET_COMMON
	E_SDK_CONFIG_OPENTRANSCOMCHANNEL , //��͸������						��Ӧ�ṹ��TransComChannel
	E_SDK_CONFIG_CLOSETRANSCOMCHANNEL,  //�ر�͸������
	E_SDK_CONFIG_SERIALWIRTE,  //д��͸��������Ϣ
	E_SDK_CONFIG_SERIALREAD,   //��ȡ͸��������Ϣ
	E_SDK_CONFIG_CHANNELTILE_DOT,	//������Ϣ
	E_SDK_CONFIG_CAMERA,           //���������									��Ӧ�ṹ��SDK_CameraParam
	E_SDK_CONFIG_ABILITY_CAMERA,    //�ع�������								��Ӧ�ṹ��SDK_CameraAbility
	E_SDK_CONFIG_BUGINFO,    //�������													
	E_SDK_CONFIG_STORAGENOTEXIST,//Ӳ�̲�����										��Ӧ�ṹ��SDK_VIDEOLOSSCONFIG
	E_SDK_CONFIG_STORAGELOWSPACE, //Ӳ����������								��Ӧ�ṹ��SDK_StorageLowSpaceConfig
	E_SDK_CONFIG_STORAGEFAILURE, //Ӳ�̳���											��Ӧ�ṹ��SDK_VIDEOLOSSCONFIG
	E_SDK_CFG_NETIPCONFLICT,    //IP��ͻ												��Ӧ�ṹ��SDK_VIDEOLOSSCONFIG
	E_SDK_CFG_NETABORT,  //�����쳣															��Ӧ�ṹ��SDK_VIDEOLOSSCONFIG

	E_SDK_CONFIG_CHNSTATUS, //ͨ��״̬													��Ӧ�ṹ��SDK_NetDecorderChnStatusAll
	E_SDK_CONFIG_CHNMODE,  //ͨ��ģʽ														��Ӧ�ṹ��SDK_NetDecorderChnModeConfig

	E_SDK_CONFIG_NET_DAS,    //����ע��													��Ӧ�ṹ��SDK_DASSerInfo
	E_SDK_CONFIG_CAR_INPUT_EXCHANGE,   //�ⲿ��Ϣ�����복��״̬�Ķ�Ӧ��ϵ			
	E_SDK_CONFIG_DELAY_TIME,       //����ϵͳ��ʱ����
	E_SDK_CONFIG_NET_ORDER,            //�������ȼ�
	E_SDK_CONFIG_ABILITY_NETORDER , ////�������ȼ���������
	E_SDK_CONFIG_CARPLATE,				//���ƺ�����	
	E_SDK_CONFIG_LOCALSDK_NET_PLATFORM, ////����ƽ̨��Ϣ����                          ��ӦSDK_LocalSdkNetPlatformConfig
	E_SDK_CONFIG_GPS_TIMING,           //GPSУʱ�������							  ��ӦSDK_GPSTimingConfig
	E_SDK_CONFIG_VIDEO_ANALYZE, //��Ƶ����(����DVR)                                   ��ӦSDK_RULECONFIG
	E_SDK_CONFIG_GODEYE_ALARM,		//���۽Ӿ�����ϵͳ								  ��ӦSDK_GodEyeConfig
	E_SDK_CONFIG_NAT_STATUS_INFO,   //nat״̬��Ϣ									  ��ӦSDK_NatStatusInfo
	E_SDK_CONFIG_BUGINFOSAVE,    //�������(����)									 
	E_SDK_CONFIG_MEDIA_WATERMARK,//ˮӡ����											  ��ӦSDK_WaterMarkConfigAll
	E_SDK_CONFIG_ENCODE_STATICPARAM,	//��������̬����						      ��ӦSDK_EncodeStaticParamAll
	E_SDK_CONFIG_LOSS_SHOW_STR,		//��Ƶ��ʧ��ʾ�ַ���
	E_SDK_CONFIG_DIGMANAGER_SHOW,	//ͨ��������ʾ����                                ��ӦSDK_DigManagerShowStatus
	E_SDK_CONFIG_ABILITY_ANALYZEABILITY,        //���ܷ�������                        ��ӦSDK_ANALYZEABILITY
	E_SDK_CONFIG_VIDEOOUT_PRIORITY,   //��ʾHDMI VGA���ȼ�������
	E_SDK_CONFIG_NAT,		  	//NAT���ܣ�MTUֵ����                                  ��ӦSDK_NatConfig
	E_SDK_CONFIG_CPCINFO,		//����CPC����������Ϣ                                 ��ӦSDK_CPCDataAll
	E_SDK_CONFIG_STORAGE_POSITION,     // ¼��洢�豸���ͣ�                                 ��Ӧ SDK_RecordStorageType
	E_SDK_CONFIG_ABILITY_CARSTATUSNUM,         //����״̬��                                   ��Ӧ SDK_CarStatusNum
	E_SDK_CFG_VPN,								//VPN                                          ��ӦSDK_VPNConfig
	E_SDK_CFG_VIDEOOUT,								///VGA��Ƶ�ֱ���                           ��ӦSDK_VGAresolution
	E_SDK_CFG_ABILITY_VGARESOLUTION,            //֧�ֵ�VGA�ֱ����б�							��ӦSDK_VGAResolutionAbility
	E_SDK_CFG_NET_LOCALSEARCH,              //�����豸���豸�˵ľ������豸					��ӦSDK_NetDevList
	E_SDK_CFG_NETPLAT_KAINENG,				//���ݿ���ƽ̨										SDK_CONFIG_KAINENG_INFO
	E_SDK_CFG_ENCODE_STATICPARAM_V2,	//DVR��������̬����                                    SDK_EncodeStaticParamV2
	E_SDK_ABILITY_ENC_STATICPARAM,	//��̬���������� DVR										SDK_EncStaticParamAbility (����)
}SDK_CONFIG_TYPE;



/// �����¼���
enum SDK_EventCodeTypes
{
	SDK_EVENT_CODE_INIT = 0,
	SDK_EVENT_CODE_LOCAL_ALARM = 1,	//���ر���
	SDK_EVENT_CODE_NET_ALARM,		//���籨��
	SDK_EVENT_CODE_MANUAL_ALARM,	//�ֶ�����
	SDK_EVENT_CODE_VIDEO_MOTION,	//��̬���
	SDK_EVENT_CODE_VIDEO_LOSS,		//��Ƶ��ʧ
	SDK_EVENT_CODE_VIDEO_BLIND,		//��Ƶ�ڵ�
	SDK_EVENT_CODE_VIDEO_TITLE,		
	SDK_EVENT_CODE_VIDEO_SPLIT,
	SDK_EVENT_CODE_VIDEO_TOUR,
	SDK_EVENT_CODE_STORAGE_NOT_EXIST,//�洢�豸������
	SDK_EVENT_CODE_STORAGE_FAILURE,//�洢�豸����ʧ��
	SDK_EVENT_CODE_LOW_SPACE,		//�洢�豸��������
	SDK_EVENT_CODE_NET_ABORT,
	SDK_EVENT_CODE_COMM,
	SDK_EVENT_CODE_STORAGE_READ_ERROR,//�洢�豸������
	SDK_EVENT_CODE_STORAGE_WRITE_ERROR,//�洢�豸д����
	SDK_EVENT_CODE_NET_IPCONFLICT,	//ip��ͻ
	SDK_EVENT_CODE_ALARM_EMERGENCY,
	SDK_EVENT_CODE_DEC_CONNECT,	
	SDK_EVENT_CODE_VideoAnalyze=25,	
	SDK_EVENT_CODE_NR,
};
//! �������õ�����
enum SDK_ENCODE_TYPE_BY_RECORD
{
	SDK_ENCODE_TYPE_TIM = 0,
	SDK_ENCODE_TYPE_MTD = 1,
	SDK_ENCODE_TYPE_ALM = 2,
	SDK_ENCODE_TYPE_NUM = 3,
	SDK_ENCODE_TYPE_SNAP_TIMER = 0,
	SDK_ENCODE_TYPE_SNAP_TRIGGER = 1,
};
#define SDK_EXTRATYPES 3 //����������

//���紫�����
enum SDK_TransferPolicy
{
	SDK_TRANSFER_POLICY_AUTO,		///< ����Ӧ
	SDK_TRANSFER_POLICY_QUALITY,	///< ��������
	SDK_TRANSFER_POLICY_FLUENCY,	///< ��������
	SDK_TRANSFER_POLICY_TRANSMISSION,//< ��������
	SDK_TRANSFER_POLICY_NR,
};



typedef struct {
	int left;
	int top;
	int right;
	int bottom;
}sdkRect;

/// ��Ƶ�����ʽ�������Խ���
struct SDK_AudioInFormatConfig
{
	int iBitRate;	   ///< ������С��kbpsΪ��λ������192kbps��128kbps
	int iSampleRate;   ///< �����ʣ�HzΪ��λ������44100Hz
	int iSampleBit;    ///< ������λ��
	int iEncodeType;   ///< ���뷽ʽ������AudioEncodeTypes����
};

//�����Խ���ʽ
typedef enum __TALK_CODING_TYPE
{
	TALK_DEFAULT = 0,
	TALK_PCM = 1,					//PCM
	TALK_G711a,						//G711a
	TALK_AMR,						//AMR
	TALK_G711u,						//G711u
	TALK_G726,						//G726
}TALK_CODING_TYPE;
//�����Խ�
typedef struct  
{
	TALK_CODING_TYPE		encodeType;		//��������
	int						nAudioBit;		//��ʵ�ʵ�ֵ��ʾ����8λ ����ֵΪ8
	unsigned int			dwSampleRate;	//�����ʣ���16k ����ֵΪ16000
	char					reserved[64];
}H264_DVR_TALKDECODE_INFO;


/// ϵͳʱ��ṹ
typedef struct SDK_SYSTEM_TIME{
	int  year;///< �ꡣ   
	int  month;///< �£�January = 1, February = 2, and so on.   
	int  day;///< �ա�   
	int  wday;///< ���ڣ�Sunday = 0, Monday = 1, and so on   
	int  hour;///< ʱ��   
	int  minute;///< �֡�   
	int  second;///< �롣   
	int  isdst;///< ����ʱ��ʶ��   
}SDK_SYSTEM_TIME;

//¼��������ؽṹ��
typedef struct tagSDK_TIMESECTION
{
	//!ʹ��
	int enable;
	//!��ʼʱ��:Сʱ
	int startHour;
	//!��ʼʱ��:����
	int	startMinute;
	//!��ʼʱ��:����
	int	startSecond;
	//!����ʱ��:Сʱ
	int	endHour;
	//!����ʱ��:����
	int	endMinute;
	//!����ʱ��:����
	int	endSecond;
}SDK_TIMESECTION;	

typedef struct{
	int dwYear;		//��
	int dwMonth;	//��
	int dwDay;		//��
	int dwHour;		//ʱ
	int dwMinute;	//��
	int dwSecond;	//��
}H264_DVR_TIME,*LPH264_DVR_TIME;

//ʱ��ṹ
typedef struct _NEW_NET_TIME								
{
	unsigned int second			:6;					//	��	1-60		
	unsigned int minute			:6;					//	��	1-60		
	unsigned int hour			:5;					//	ʱ	1-24		
	unsigned int day			:5;					//	��	1-31		
	unsigned int month			:4;					//	��	1-12		
	unsigned int year			:6;					//	��	2000-2063	
}NEW_NET_TIME, *LPNET_TIME;
///< ����ʱ�ṹ
struct DSTPoint
{
	int	iYear;
	int	iMonth;
	int	iWeek;		///<��1:first  to2 3 4 -1:last one   0:��ʾʹ�ð��ռ���ķ���[-1,4]
	int	iWeekDay;	///<weekday from sunday=0	[0, 6]
	int Hour;
	int Minute;
};

struct SDK_VIDEO_FORMAT
{
	int		iCompression;			//  ѹ��ģʽ 	
	int		iResolution;			//  �ֱ��� ����ö��SDK_CAPTURE_SIZE_t
	int		iBitRateControl;		//  �������� ����ö��SDK_capture_brc_t
	int		iQuality;				//  �����Ļ��� ����1-6		
	int		nFPS;					//  ֡��ֵ��NTSC/PAL������,������ʾ����һ֡		
	int		nBitRate;				//  0-4096k,���б���Ҫ�ɿͻ��˱��棬�豸ֻ����ʵ�ʵ�����ֵ�������±ꡣ
	int		iGOP;					//  ��������I֮֡��ļ��ʱ�䣬2-12 
} ;
struct  SDK_AUDIO_FORMAT
{
	int		nBitRate;				//  ����kbps	
	int		nFrequency;				//  ����Ƶ��	
	int		nMaxVolume;				//  ���������ֵ
} ;
// ý���ʽ
struct SDK_MEDIA_FORMAT
{
	struct SDK_VIDEO_FORMAT vfFormat;			//  ��Ƶ��ʽ���� 			
	struct SDK_AUDIO_FORMAT afFormat;			//  ��Ƶ��ʽ���� 
	bool	bVideoEnable;				//  ������Ƶ���� 
	bool	bAudioEnable;				//  ������Ƶ���� 	
} ;

typedef union {	//IP addr
	unsigned char	c[4];
	unsigned short	s[2];
	unsigned int 	l;
}CONFIG_IPAddress;








///////////////////�û��ʺŹ�����ؽṹ��/////////////////////////////////
typedef struct _OPR_RIGHT
{
	char name[NET_MAX_USER_LENGTH];
}OPR_RIGHT;

typedef struct _USER_INFO
{
	int			rigthNum;
	char		rights[NET_MAX_RIGTH_NUM][NET_MAX_USER_LENGTH];
	char		Groupname[NET_MAX_USER_LENGTH];
	char		memo[NET_MAX_USER_LENGTH];
	char		name[NET_MAX_USER_LENGTH];
	char		passWord[NET_MAX_USER_LENGTH];
	bool		reserved;		//�Ƿ���
	bool		shareable;		//���û��Ƿ������� 1-���ã�0-������
}USER_INFO;

typedef struct _USER_GROUP_INFO
{
	int			rigthNum;
	char		memo[NET_MAX_USER_LENGTH]; 
	char		name[NET_MAX_USER_LENGTH];
	char		rights[NET_MAX_RIGTH_NUM][NET_MAX_USER_LENGTH];	//Ȩ���б�
}USER_GROUP_INFO;

//�û���Ϣ���ýṹ
typedef struct _USER_MANAGE_INFO
{
	int					rigthNum;
	OPR_RIGHT			rightList[NET_MAX_RIGTH_NUM];
	int					groupNum;
	USER_GROUP_INFO		groupList[NET_MAX_GROUP_NUM];
	int					userNum;
	USER_INFO			userList[NET_MAX_USER_NUM];
}USER_MANAGE_INFO;

//�޸��û�
typedef struct _CONF_MODIFYUSER
{
	char UserName[NET_MAX_USER_LENGTH];
	USER_INFO User;
}CONF_MODIFYUSER;

//�޸���
typedef struct _CONF_MODIFYGROUP
{
	char GroupName[NET_MAX_USER_LENGTH];
	USER_GROUP_INFO Group;
}CONF_MODIFYGROUP;


/// �޸��û���������
struct _CONF_MODIFY_PSW
{
	char sUserName[NET_MAX_USER_LENGTH];
	char Password[NET_MAX_USER_LENGTH];
	char NewPassword[NET_MAX_USER_LENGTH];
};

//��ͨ����ҳ�ṹ��
typedef struct _SDK_CONFIG_NORMAL
{	
	struct SDK_SYSTEM_TIME sysTime;		//ϵͳʱ��

	int iLocalNo;			/*!< �������:[0, 998] */
	int iOverWrite;			/*!< Ӳ����ʱ���� "OverWrite", "StopRecord" */		
	int iSnapInterval;			///< ��ʱץͼ��ʱ����������Ϊ��λ 
	char sMachineName[64];	///< ������
	int iVideoStartOutPut;	/*!< ���ģʽ */
	int iAutoLogout;			///< ���ز˵��Զ�ע��(����)	[0, 120]

	int iVideoFormat;		/*!< ��Ƶ��ʽ:��PAL��, ��NTSC��, ��SECAM�� */
	int iLanguage;			/*!< ����ѡ��:��English��, ��SimpChinese��, ��TradChinese��, ��Italian��, ��Spanish��, ��Japanese��, ��Russian��, ��French��, ��German�� */	
	int iDateFormat;		/*!< ���ڸ�ʽ:��YYMMDD��, ��MMDDYY��, ��DDMMYY�� */
	int iDateSeparator;		/*!< ���ڷָ��:��.��, ��-��, ��/�� */
	int iTimeFormat;		/*!< ʱ���ʽ:��12��, ��24�� */
	int iDSTRule;			///< ����ʱ���� 
	int iWorkDay;			///< ������
	DSTPoint dDSTStart;
	DSTPoint dDSTEnd;
}SDK_CONFIG_NORMAL;


//���±����������
// ��������
struct SDK_CONFIG_ENCODE
{
	struct SDK_MEDIA_FORMAT dstMainFmt[SDK_ENCODE_TYPE_NUM];		//  ��������ʽ 	
	struct SDK_MEDIA_FORMAT dstExtraFmt[SDK_EXTRATYPES];	//  ��������ʽ 
	struct SDK_MEDIA_FORMAT dstSnapFmt[SDK_ENCODE_TYPE_NUM];		//  ץͼ��ʽ 
};
struct  SDK_EncodeConfigAll
{
	struct SDK_CONFIG_ENCODE vEncodeConfigAll[NET_MAX_CHANNUM];
};


// �򻯰汾��������
/// ý���ʽ
struct SDK_MEDIA_FORMAT_SIMPLIIFY
{
	struct SDK_VIDEO_FORMAT vfFormat;			//  ��Ƶ��ʽ���� 			
	bool	bVideoEnable;			//  ������Ƶ���� 
	bool	bAudioEnable;			//  ������Ƶ���� 	
};

/// ��������
struct SDK_CONFIG_ENCODE_SIMPLIIFY
{
	struct SDK_MEDIA_FORMAT dstMainFmt;		//  ��������ʽ 	
	struct SDK_MEDIA_FORMAT dstExtraFmt;	//  ��������ʽ 
};

/// ȫͨ����������
struct SDK_EncodeConfigAll_SIMPLIIFY
{
	struct SDK_CONFIG_ENCODE_SIMPLIIFY vEncodeConfigAll[NET_MAX_CHANNUM];
};


struct  SDK_CombineEncodeConfigAll
{
	struct SDK_CONFIG_ENCODE vEncodeConfigAll[NET_MAX_COMBINE_NUM];
};

/// ��ϱ���ģʽ
struct SDK_CombEncodeParam
{
	int iEncodeMode;	 //��CombineEncodeMode
};

struct SDK_CombEncodeModeAll
{
	struct SDK_CombEncodeParam vEncodeParam[NET_MAX_COMBINE_NUM];
};

//!��Ƶ����ṹ
struct  SDK_VIDEO_WIDGET
{
	unsigned int rgbaFrontground;		///< �����ǰ��MakeRGB����͸����	
	unsigned int rgbaBackground;		///< ����ĺ�MakeRGB����͸����	
	sdkRect	rcRelativePos;			///< ����߾��������ı���*8191
	bool	bPreviewBlend;			///< Ԥ������	
	bool	bEncodeBlend;			///< �������
};

//!��Ƶ�������
struct SDK_CONFIG_VIDEOWIDGET
{
	struct SDK_VIDEO_WIDGET	dstCovers[NET_COVERNUM];
	struct SDK_VIDEO_WIDGET	ChannelTitle;
	struct SDK_VIDEO_WIDGET	TimeTitle;
	struct 
	{
		char strName[NET_NAME_PASSWORD_LEN];
#ifdef WIN32
 		__int64 iSerialNo;	
#else
		long long iSerialNo;	
#endif
	}ChannelName;					/// ͨ������
	int				iCoverNum;		/*!< ��ǰ��ͨ���м������ӵ����� */
};

/// ��Ƶ���(���ģʽ�Ի���)
struct SDK_VideoWidgetConfigAll
{
	struct SDK_CONFIG_VIDEOWIDGET vVideoWidegetConfigAll[NET_MAX_CHANNUM];
};

//��Ƶ��ɫ
struct SDK_VIDEOCOLOR_PARAM
{
	int	nBrightness;		///< ����	0-100
	int	nContrast;			///< �Աȶ�	0-100
	int	nSaturation;		///< ���Ͷ�	0-100
	int	nHue;				///< ɫ��	0-100
	int	mGain;				///< ����	0-100 �ڣ�λ��1��ʾ�Զ�����		
	int	mWhitebalance;		///< �Զ��׵�ƽ���ƣ�bit7��λ��ʾ�����Զ�����.0x0,0x1,0x2�ֱ�����,��,�ߵȼ�
	int nAcutance;          ///< ���   0-15
};
///< ��Ƶ��ɫ����
struct SDK_VIDEOCOLOR
{
	SDK_TIMESECTION			tsTimeSection;		/// ʱ���
	struct SDK_VIDEOCOLOR_PARAM	dstColor;			/// ��ɫ����
	int					iEnable;
};

struct SDK_CONFIG_VIDEOCOLOR
{
	struct SDK_VIDEOCOLOR dstVideoColor[NET_N_MIN_TSECT];
};

struct SDK_VideoColorConfigAll
{
	struct SDK_CONFIG_VIDEOCOLOR vVideoColorAll[NET_MAX_CHANNUM];
};


/// ����ͨ�����Ʊ���
struct SDK_ChannelNameConfigAll
{
	char channelTitle[NET_MAX_CHANNUM][NET_NAME_PASSWORD_LEN];
};

//���ģʽ
struct SDK_GUISetConfig
{
	int iWindowAlpha;			///< ����͸����	[128, 255]
	bool bTimeTitleEn;			///< ʱ�������ʾʹ��
	bool bChannelTitleEn;		///< ͨ��������ʾʹ��	
	bool bAlarmStatus;			///<  ����״̬
	bool bRecordStatus;			///<  ¼��״̬��ʾʹ��
	bool bChanStateRecEn;		///< ¼���־��ʾʹ��
	bool bChanStateVlsEn;		///< ��Ƶ��ʧ��־��ʾʹ��
	bool bChanStateLckEn;		///< ͨ��������־��ʾʹ��	
	bool bChanStateMtdEn;		///< ��̬����־��ʾʹ��
	bool bBitRateEn;			///< ������ʾʹ��
	bool bRemoteEnable;			///< ң����ʹ��
	bool bDeflick;				///< ������
};


////!��ͨ��������
struct SDK_CONFIG_NET_COMMON
{
	//!������
	char HostName[NET_NAME_PASSWORD_LEN];
	//!����IP
	CONFIG_IPAddress HostIP;
	//!��������
	CONFIG_IPAddress Submask;
	//!����IP
	CONFIG_IPAddress Gateway;
	//!HTTP����˿�
	int HttpPort;
	//!TCP�����˿�
	int TCPPort;	
	//!SSL�����˿�
	int SSLPort;
	//!UDP�����˿�
	int UDPPort;
	//!���������
	int MaxConn;
	//!����Э�� {"TCP","UDP","MCAST",��}
	int MonMode;
	//!�޶�����ֵ
	int MaxBps;
	//!�������
	//char TransferPlan[NET_NAME_PASSWORD_LEN];
	int TransferPlan;

	//!�Ƿ����ø���¼�����ز���
	bool bUseHSDownLoad;

	//!MAC��ַ
	char sMac[NET_MAX_MAC_LEN];
};

//DHCP
#define SDK_MAX_ETH_NUM 4
struct SDK_NetDHCPConfig
{
	bool bEnable;
	char ifName[32];
};
/// ����������DHCP����
struct SDK_NetDHCPConfigAll
{
	SDK_NetDHCPConfig vNetDHCPConfig[SDK_MAX_ETH_NUM];
};

///< DNS����
struct SDK_NetDNSConfig
{
	CONFIG_IPAddress		PrimaryDNS;
	CONFIG_IPAddress		SecondaryDNS;
};
///< �������ṹ����
struct SDK_RemoteServerConfig
{
	char ServerName[NET_NAME_PASSWORD_LEN];	///< ������
	CONFIG_IPAddress ip;						///< IP��ַ
	int Port;							///< �˿ں�
	char UserName[NET_NAME_PASSWORD_LEN];		///< �û���
	char Password[NET_NAME_PASSWORD_LEN];		///< ����	
	bool Anonymity;							///< �Ƿ�������¼
};
///< IPȨ������
struct SDK_NetIPFilterConfig
{
	bool Enable;		///< �Ƿ���
	CONFIG_IPAddress BannedList[NET_MAX_FILTERIP_NUM];		///< �������б�
	CONFIG_IPAddress TrustList[NET_MAX_FILTERIP_NUM];		///< �������б�
};

///< �鲥����
struct SDK_NetMultiCastConfig
{
	bool Enable;		///< �Ƿ���
	SDK_RemoteServerConfig Server;		///< �鲥������
};

///< pppoe����
struct SDK_NetPPPoEConfig
{
	bool Enable;	///< �Ƿ���
	SDK_RemoteServerConfig Server;		///< PPPOE������
	CONFIG_IPAddress addr;		///< ���ź��õ�IP��ַ
};

///< DDNS����
struct SDK_NetDDNSConfig
{
	bool Enable;	///< �Ƿ���
	bool Online;		///< �Ƿ�����
	char DDNSKey[NET_NAME_PASSWORD_LEN];	///< DDNS��������, Ŀǰ��: JUFENG
	char HostName[NET_NAME_PASSWORD_LEN];	///< ������
	SDK_RemoteServerConfig Server;			///< DDNS������
	
};

///< DDNS����
struct SDK_NetDDNSConfigALL
{
	SDK_NetDDNSConfig ddnsConfig[NET_MAX_DDNS_TYPE];
};

///< FTP����
struct SDK_FtpServerConfig{
	bool bEnable;        ///< ������ʹ��     
	SDK_RemoteServerConfig Server;	///< FTP������
	char cRemoteDir[NET_MAX_PATH_LENGTH];	///< Զ��Ŀ¼
	int iMaxFileLen;	///< �ļ���󳤶�
};

///< NTP����
struct SDK_NetNTPConfig
{
	///< �Ƿ���
	bool Enable;
	///< PPPOE������
	SDK_RemoteServerConfig Server;
	///< ��������
	int UpdatePeriod;
	///< ʱ��
	int TimeZone;
};
#define  NET_MAX_EMAIL_TITLE_LEN 64
#define  NET_MAX_EMAIL_RECIEVERS  5
#define  NET_EMAIL_ADDR_LEN  32

///< EMAIL����
struct SDK_NetEmailConfig
{
	///< �Ƿ���
	bool Enable;
	///< smtp ��������ַʹ���ַ�����ʽ���
	///< ������ip,Ҳ����������
	SDK_RemoteServerConfig Server;
	bool bUseSSL;
	///< ���͵�ַ
	char SendAddr[NET_EMAIL_ADDR_LEN];
	///< �����˵�ַ
	char Recievers[NET_MAX_EMAIL_RECIEVERS][NET_EMAIL_ADDR_LEN];
	///< �ʼ�����
	char Title[NET_MAX_EMAIL_TITLE_LEN];
	///< email��Чʱ��
	SDK_TIMESECTION Schedule[NET_N_MIN_TSECT];
};

///< ARSP(����ע�������)����
struct SDK_NetARSPConfig
{
	bool bEnable;	///< �Ƿ���
	char sARSPKey[NET_NAME_PASSWORD_LEN];	///< DNS��������
	int iInterval;	///< ������ʱ��
	char sURL[NET_NAME_PASSWORD_LEN];    ///< ��������
	SDK_RemoteServerConfig Server;		///< DDNS������
	int nHttpPort;                  ///< ������HTTP�˿�
};

struct SDK_NetARSPConfigAll
{
	SDK_NetARSPConfig vNetARSPConfigAll[NET_MAX_ARSP_TYPE];
};

///< ��������ַ����
struct SDK_NetDecoderConfig
{
	bool Enable;						///< �Ƿ���
	char UserName[NET_NAME_PASSWORD_LEN];	///< DDNS��������, Ŀǰ��: JUFENG
	char PassWord[NET_NAME_PASSWORD_LEN];	///< ������
	char Address[NET_NAME_PASSWORD_LEN];
	int Protocol;
	int Port;							///< ���������Ӷ˿�
	int Channel;						///< ����������ͨ����
	int Interval;                       ///< ��Ѳ�ļ��ʱ��(s)
};

/// ����ͨ���Ľ�������ַ����
struct SDK_NetDecoderConfigAll
{
	SDK_NetDecoderConfig vNetDecoderConfig[NET_MAX_DECORDR_CH];
};


/// ��������ַ����V2�汾
struct SDK_NetDecoderConfig_V2
{
	int nTotalNum;            //��Ч��������������ΪNET_MAX_CHANNUM
	SDK_NetDecoderConfig vNetDecoderConfig[NET_MAX_CHANNUM];
};

/// ����ͨ���Ľ�������ַ����V2�汾
struct SDK_NetDecoderConfigAll_V2
{
	SDK_NetDecoderConfig_V2 vNetDecoderArray[NET_MAX_DECORDR_CH];
};


///< ��������ַ����
struct SDK_NetDecorderConfigV3
{
	bool Enable;						///< �Ƿ���
	char UserName[NET_NAME_PASSWORD_LEN];	///< DDNS��������, Ŀǰ��: JUFENG
	char PassWord[NET_NAME_PASSWORD_LEN];	///< ������
	char Address[NET_NAME_PASSWORD_LEN];
	int Protocol;
	int Port;							///< ���������Ӷ˿�
	int Channel;						///< ����������ͨ����
	int Interval;                       ///< ��Ѳ�ļ��ʱ��(s),0:��ʾ����
	char ConfName[NET_NAME_PASSWORD_LEN];	///<��������
	int DevType;						///<�豸����
	int StreamType;						///<���ӵ���������CaptureChannelTypes
};

/*��������������*/
enum SDK_DecorderConnType
{
	SDK_CONN_SINGLE = 0, 	/*������*/
	SDK_CONN_MULTI = 1,		/*��������Ѳ*/
	SDK_CONN_TYPE_NR,
};

/*����ͨ��������*/
struct SDK_NetDigitChnConfig
{
	bool Enable;		/*����ͨ���Ƿ���*/		
	int ConnType;		/*�������ͣ�ȡDecoderConnectType��ֵ*/
	int TourIntv;		/*������ʱ��Ѳ���*/
	unsigned int SingleConnId;	/*������ʱ����������ID, ��1��ʼ��0��ʾ��Ч*/
	bool EnCheckTime;	/*������ʱ*/
	SDK_NetDecorderConfigV3 NetDecorderConf[32]; /*�����豸ͨ�����ñ�*/
};

/*��������ͨ��������*/
struct SDK_NetDecorderConfigAll_V3
{
	SDK_NetDigitChnConfig DigitChnConf[NET_MAX_DECORDR_CH];
};

//ͨ��ģʽ
#define MAX_HVR_CHNCAP_CHN		32
typedef struct SDK_HVR_CHNCAP 
{
	int nD1Chn;		// ֧�ֵ�D1·��
	int n960HChn;	// ֧�ֵ�960H·��
	int n720PChn;	// ֧�ֵ�720P·��
	int n1080PChn;	// ֧�ֵ�1080P·��
	int nCIFChn;	//֧�ֵ�CIFͨ����
	int nHD1Chn;	//֧�ֵ�HD1ͨ����
	unsigned char nPlayChn; //֧�ֻط�·��
	unsigned char nDigiTalChn; //֧���������ͨ��·��
	unsigned char nResChar[2]; //����
	int nRes[1];
}SDK_HVR_CHNCAP, *SDK_PHVR_CHNCAP;

typedef struct SDK_CAPTURE_HVRCAP 
{
	SDK_HVR_CHNCAP DigitalCap;		// ֧�ֵ�����ͨ����Ϣ
	SDK_HVR_CHNCAP AnalogCap;		// ֧�ֵ�ģ��ͨ����Ϣ
}SDK_CAPTURE_HVRCAP, *SDK_PCAPTURE_HVRCAP;

typedef struct SDK_CAPTURE_TOTAL_HVRCAP 
{
	int		nHVRCap;			// ʵ��֧�ֵ�ģʽ
	SDK_CAPTURE_HVRCAP	HVRCap[MAX_HVR_CHNCAP_CHN];		// ����ģʽ�Ļ���
}SDK_CAPTURE_TOTAL_HVRCAP, *SDK_PCAPTURE_TOTAL_HVRCAP;

//ͨ��ģʽ����
struct SDK_NetDecorderChnModeConfig
{
	SDK_CAPTURE_TOTAL_HVRCAP 	HVRTotalCap;
	int HVRCurCapMode;
};


/*����ͨ��״̬*/
struct SDK_NetDecorderChnStatus
{
	char ChnName[NET_NAME_PASSWORD_LEN];
	char pMaxResName[50];
	char	pCurResName[50];
	char pStatus[50];
};

/*��������ͨ��״̬*/
struct SDK_NetDecorderChnStatusAll
{
	SDK_NetDecorderChnStatus ChnStatusAll[MAX_HVR_CHNCAP_CHN];
};

///< 3G��������
struct SDK_Net3GConfig
{
	bool bEnable;			  ///< ����ģ��ʹ�ܱ�־
	int iNetType;			  ///< ������������
	char sAPN[NET_NAME_PASSWORD_LEN];		///< ���������
	char sDialNum[NET_NAME_PASSWORD_LEN];   ///< ���ź���
	char sUserName[NET_NAME_PASSWORD_LEN];  ///< �����û���
	char sPassword[NET_NAME_PASSWORD_LEN];  ///< ��������
	CONFIG_IPAddress addr;			  ///< ���ź��õ�IP��ַ
};

///< �ֻ�������ð���
struct SDK_NetMoblieConfig
{
	bool bEnable;	///< �Ƿ���
	SDK_RemoteServerConfig Server;		///< ������
};

//RTSP
struct SDK_NetRTSPConfig
{
	bool bServer;
	bool bClient;
	SDK_RemoteServerConfig Server;		///< ������ģʽ
	SDK_RemoteServerConfig Client;		///< �ͻ���ģʽ
};

///< UPNP����
struct SDK_NetUPNPConfig
{
	bool bEnable;			  ///< ʹ�ܱ�־
	bool bState;              ///< ״̬, 1: OK 0: NOK
	int iHTTPPort;			  ///< HTTP����ӳ���Ķ˿�
	int iMediaPort;			  ///< ý������ӳ���Ķ˿�
	int iMobliePort;		  ///< �ֻ����ӳ���Ķ˿�
};

///< WIFI����
struct SDK_NetWifiConfig
{
	bool bEnable;
	char sSSID[36];            //SSID Number
	int nChannel;                   //channel
	char sNetType[32];         //Infra, Adhoc
	char sEncrypType[32];      //NONE, WEP, TKIP, AES
	char sAuth[32];            //OPEN, SHARED, WEPAUTO, WPAPSK, WPA2PSK, WPANONE, WPA, WPA2
	int  nKeyType;                  //0:Hex 1:ASCII
	char sKeys[NET_IW_ENCODING_TOKEN_MAX];
	CONFIG_IPAddress HostIP;		///< host ip
	CONFIG_IPAddress Submask;		///< netmask
	CONFIG_IPAddress Gateway;		///< gateway
};

enum SDK_RSSI_SINGNAL
{
	SDK_RSSI_NO_SIGNAL,   //<= -90db
	SDK_RSSI_VERY_LOW,     //<= -81db
	SDK_RSSI_LOW,          //<= -71db
	SDK_RSSI_GOOD,         //<= -67db
	SDK_RSSI_VERY_GOOD,    //<= -57db
	SDK_RSSI_EXCELLENT     //>-57db
};

struct SDK_NetWifiDevice
{
	char sSSID[36];            //SSID Number
	int nRSSI;                 //SEE SDK_RSSI_SINGNAL
	int nChannel;
	char sNetType[32];         //Infra, Adhoc
	char sEncrypType[32];      //NONE, WEP, TKIP, AES
	char sAuth[32];            //OPEN, SHARED, WEPAUTO, WPAPSK, WPA2PSK, WPANONE, WPA, WPA2
};

struct SDK_NetWifiDeviceAll
{
	int nDevNumber;
	SDK_NetWifiDevice vNetWifiDeviceAll[NET_MAX_AP_NUMBER];
};

///< ������������
struct SDK_NetAlarmCenterConfig
{
	bool bEnable;		///< �Ƿ���
	char sAlarmServerKey[NET_NAME_PASSWORD_LEN];	///< ��������Э����������, 
	///< �������ķ�����
	SDK_RemoteServerConfig Server;	
	bool bAlarm;
	bool bLog;
};

struct SDK_NetAlarmServerConfigAll
{
	SDK_NetAlarmCenterConfig vAlarmServerConfigAll[NET_MAX_ALARMSERVER_TYPE];
};


//���Ż�ͨ
struct SDK_CONFIG_NET_MEGA
{
	bool bEnable;
	bool bNetManEnable;
	CONFIG_IPAddress ServerIP;
	int iServerPort;
	char sDeviceId[32];
	char sUserName[24];
	char sPasswd[32];
	int iMaxCon;
	int iVideoPort;
	int iAudioPort;
	int iMsgPort;
	int iUpdatePort;
};

// ����ƽ̨
struct SDK_CONFIG_NET_XINGWANG
{
	bool bEnable;
	bool bSyncTime;
	bool bSubStream;
	CONFIG_IPAddress ServerIP;
	int iServerPort;
	int iDownLoadPort;
	char sPasswd[32];
	char szSID[32];
};

// ����ƽ̨
struct SDK_CONFIG_NET_SHISOU
{
	bool bEnable;
	SDK_RemoteServerConfig Server;
	char szSID[NET_MAX_USERNAME_LENGTH];
};

// VVEYEƽ̨
struct SDK_CONFIG_NET_VVEYE
{
	bool bEnable;                
	bool bCorpEnable;            //ֻ����ʹ����ҵ������ʱ����Ҫ����Server
	SDK_RemoteServerConfig Server;
	char szDeviceName[NET_MAX_USERNAME_LENGTH];
};

// ����������Ϣ����
enum SDK_AlarmCenterMsgType
{
	SDK_ALARMCENTER_ALARM,
	SDK_ALARMCENTER_LOG,
};

// ����������Ϣ����
enum SDK_AlarmCenterStatus
{
	SDK_AC_START,
	SDK_AC_STOP,
};

// �澯������Ϣ����
struct SDK_NetAlarmCenterMsg
{
	CONFIG_IPAddress HostIP;		///< �豸IP
	int nChannel;                   ///< ͨ��
	int nType;                      ///< ���� ��AlarmCenterMsgType
	int nStatus;                    ///< ״̬ ��AlarmCenterStatus
	SDK_SYSTEM_TIME Time;           ///< ����ʱ��
	char sEvent[NET_MAX_INFO_LEN];  ///< �¼�
	char sSerialID[NET_MAX_MAC_LEN]; ///< �豸���к�
	char sDescrip[NET_MAX_INFO_LEN];  ///< ����
};

/// ������Ϣ
struct SDK_EncodeInfo
{
	bool bEnable;			///< ʹ����
	int iStreamType;		///< �������ͣ�capture_channel_t
	bool bHaveAudio;		///< �Ƿ�֧����Ƶ
	unsigned int uiCompression;		///< capture_comp_t������
	unsigned int uiResolution;		///< SDK_CAPTURE_SIZE_t������
};

/// ��������
struct CONFIG_EncodeAbility
{
	unsigned int iMaxEncodePower;		///< ֧�ֵ�����������
	int iChannelMaxSetSync;		///< ÿ��ͨ���ֱ����Ƿ���Ҫͬ�� 0-��ͬ��, 1 -ͬ��
	unsigned int nMaxPowerPerChannel[NET_MAX_CHANNUM];		///< ÿ��ͨ��֧�ֵ���߱�������
	unsigned int ImageSizePerChannel[NET_MAX_CHANNUM];		///< ÿ��ͨ��֧�ֵ�ͼ��ֱ���
	unsigned int ExImageSizePerChannel[NET_MAX_CHANNUM];		///< ÿ��ͨ��֧�ֵĸ�����ͼ��ֱ���
	SDK_EncodeInfo vEncodeInfo[SDK_CHL_FUNCTION_NUM];	///< ������Ϣ,��ʱ����4������
	SDK_EncodeInfo vCombEncInfo[SDK_CHL_FUNCTION_NUM];	///< ��ϱ�����Ϣ,��ʱ����4������
	int	iMaxBps;				///< �������Kbps
	unsigned int ExImageSizePerChannelEx[NET_MAX_CHANNUM][SDK_CAPTURE_SIZE_NR];	///< ÿ��ͨ��֧�ֵĸ�����ͼ��ֱ���
};

// ����Э��
enum SDK_CommProtocol
{
	SDK_CONSOLE = 0,
	SDK_KEYBOARD,
	SDK_COM_TYPES,
};
/// ����Э��
struct SDK_COMMFUNC
{
	//ÿ��Э�������64���ַ����
	int nProNum;
	char vCommProtocol[100][32];
};

/// ��̨Э��
struct SDK_PTZPROTOCOLFUNC
{
	//ÿ��Э�������64���ַ����
	int nProNum;
	char vPTZProtocol[100][NET_MAX_PTZ_PROTOCOL_LENGTH];
};

/// �����ڵ�������
struct SDK_BlindDetectFunction
{
	int iBlindConverNum;	///< �����ڵ�����
};

/// ��������������
struct SDK_MotionDetectFunction
{
	int iGridRow;
	int iGridColumn;
};

/// ֧�ֵ�DDNS����
struct SDK_DDNSServiceFunction
{
	int  nTypeNum;
	char vDDNSType[NET_MAX_DDNS_TYPE][64];
};

/// ֧������
struct SDK_MultiLangFunction
{
	//ÿ��Э�������64���ַ����
	int nLangNum;
	char vLanguageName[128][64];
};

/// ֧�ֵ���Ƶ��ʽ
struct SDK_MultiVstd
{
	//ÿ��Э�������3���ַ����
	int nVstdNum;
	char vVstdName[3][64];
};

/// ���빦��
enum SDK_EncodeFunctionTypes
{
	SDK_ENCODE_FUNCTION_TYPE_DOUBLE_STREAM,		///< ˫��������
	SDK_ENCODE_FUNCTION_TYPE_COMBINE_STREAM,	///< ��ϱ��빦��
	SDK_ENCODE_FUNCTION_TYPE_SNAP_STREAM,		///< ץͼ����
	SDK_ENCODE_FUNCTION_TYPE_WATER_MARK,		///< ˮӡ����
	SDK_ENCODE_FUNCTION_TYPE_NR,
};

/// ��������
enum SDK_AlarmFucntionTypes
{
	SDK_ALARM_FUNCTION_TYPE_MOTION_DETECT,	///< ��̬���
	SDK_ALARM_FUNCTION_TYPE_BLIND_DETECT,	///< �����ڵ�
	SDK_ALARM_FUNCTION_TYPE_LOSS_DETECT,	///< ������ʧ
	SDK_ALARM_FUNCTION_TYPE_LOCAL_ALARM,	///< ���ر���
	SDK_ALARM_FUNCTION_TYPE_NET_ALARM,		///< ���籨��
	SDK_ALARM_FUNCTION_TYPE_IP_CONFLICT,	///< IP��ַ��ͻ
	SDK_ALARM_FUNCTION_TYPE_NET_ABORT,		///< �����쳣
	SDK_ALARM_FUNCTION_TYPE_STORAGE_NOTEXIST,	///< �洢�豸������
	SDK_ALARM_FUNCTION_TYPE_STORAGE_LOWSPACE,	///< �洢�豸��������
	SDK_ALARM_FUNCTION_TYPE_STORAGE_FAILURE,	///< �洢�豸����ʧ��
	SDK_ALARM_FUNCTION_TYPE_VIDEOANALYSE,///<��Ƶ����
	SDK_ALARM_FUNCTION_TYPE_NR
};

/// ���������
enum SDK_NetServerTypes
{
	SDK_NET_SERVER_TYPES_IPFILTER,		///< �׺�����
	SDK_NET_SERVER_TYPES_DHCP,			///< DHCP����
	SDK_NET_SERVER_TYPES_DDNS,			///< DDNS����
	SDK_NET_SERVER_TYPES_EMAIL,			///< Email����
	SDK_NET_SERVER_TYPES_MULTICAST,		///< �ಥ����
	SDK_NET_SERVER_TYPES_NTP,			///< NTP����
	SDK_NET_SERVER_TYPES_PPPOE,
	SDK_NET_SERVER_TYPES_DNS,
	SDK_NET_SERVER_TYPES_ARSP,			///< ����ע�����
	SDK_NET_SERVER_TYPES_3G,            ///< 3G����
	SDK_NET_SERVER_TYPES_MOBILE=10,        ///< �ֻ����
	SDK_NET_SERVER_TYPES_UPNP,			    ///< UPNP
	SDK_NET_SERVER_TYPES_FTP,			    ///< FTP
	SDK_NET_SERVER_TYPES_WIFI,          ///<WIFI
	SDK_NET_SERVER_TYPES_ALARM_CENTER,  ///< �澯����
	SDK_NET_SERVER_TYPES_NETPLAT_MEGA,  ///< ���Ż�ͨ
	SDK_NET_SERVER_TYPES_NETPLAT_XINWANG,  ///< ����
	SDK_NET_SERVER_TYPES_NETPLAT_SHISOU,  ///< ����
	SDK_NET_SERVER_TYPES_NETPLAT_VVEYE,  ///< ������
	SDK_NET_SERVER_TYPES_RTSP,     //RTSP
	SDK_NET_SERVER_TYPES_PHONEMSG=20,     //�ֻ���Ϣ��������
	SDK_NET_SERVER_TYPES_PHONEMULTIMEDIAMSG,     //�ֻ���Ϣ��������
	SDK_NET_SERVER_TYPES_DAS,          //����ע��
	SDK_NET_SERVER_TYPES_LOCALSDK_PLATFORM,          //����ƽ̨��Ϣ����
	SDK_NET_SERVER_TYPES_GOD_EYE,///<���۽Ӿ�����ϵͳ
	SDK_NET_SERVER_TYPES_NAT,		///NAT��͸��MTU����
	SDK_NET_SERVER_TYPES_VPN,     ///VPN
	SDK_NET_SERVER_TYPES_NR,
    
};

/// Ԥ������
enum SDK_PreviewTypes
{
	SDK_PREVIEW_TYPES_TOUR,		///< ��Ѳ
	SDK_PREVIEW_TYPES_TALK,		///< GUI��������
	SDK_PREVIEW_TYPES_NR
};

///��������
enum SDK_CommTypes
{
	SDK_COMM_TYPES_RS485,			///<485����
	SDK_COMM_TYPES_RS232,			///<232����
	SDK_COMM_TYPES_NR
};

//���뷨����
enum SDK_InPutMethod
{
	SDK_NO_SUPPORT_CHINESE,		//��֧����������
	SDK_NO_SUPPORT_NR
};

//�����б�ǩ��ʾ
enum SDK_TipShow
{
	SDK_NO_BEEP_TIP_SHOW,  //������ʾ
	SDK_NO_FTP_TIP_SHOW,  //FTP��ʾ
	SDK_NO_EMAIL_TIP_SHOW,  //EMAIL��ʾ
	SDK_NO_TIP_SHOW_NR
};
///���ع���
enum SDK_MobileCar
{
	SDK_MOBILEDVR_STATUS_EXCHANGE,
	SDK_MOBILEDVR_DELAY_SET,
	SDK_MOBILEDVR_CARPLATE_SET,
	SDK_MOBILEDVR_GPS_TIMING,
	SDK_MOBILEDVR_NR
};

///��������
enum SDK_OtherFunction
{
	SDK_OTHER_DOWNLOADPAUSE,		//¼��������ͣ����
	SDK_OTHER_USB_SUPPORT_RECORD,	//USB֧��¼����
	SDK_OTHER_SD_SUPPORT_RECORD,		//SD֧��¼����
	SDK_OTHER_ONVIF_CLIENT_SUPPORT,	//�Ƿ�֧��ONVIF�ͻ���
	SDK_OTHER_NET_LOCALSEARCH_SUPPORT,	//�Ƿ�֧��Զ������
	SDK_OTHER_MAXPLAYBACK_SUPPORT, //�Ƿ�֧�����ط�ͨ������ʾ
	SDK_OTHER_NVR_SUPPORT, //�Ƿ���רҵNVR
	SDK_OTHER_NR
};

///֧�ֵ�ϵͳ����
struct SDK_SystemFunction
{
	bool vEncodeFunction[SDK_ENCODE_FUNCTION_TYPE_NR];	///< ���빦��SDK_EncodeFunctionTypes
	bool vAlarmFunction[SDK_ALARM_FUNCTION_TYPE_NR];		///< ��������AlarmFucntionTypes
	bool vNetServerFunction[SDK_NET_SERVER_TYPES_NR];	///< ���������NetServerTypes
	bool vPreviewFunction[SDK_PREVIEW_TYPES_NR];		///< Ԥ������PreviewTypes
	bool vCommFunction[SDK_COMM_TYPES_NR];			///<��������SDK_CommTypes
	bool vInputMethodFunction[SDK_NO_SUPPORT_NR];  //<���뷨����SDK_InPutMethod>
	bool vTipShowFunction[SDK_NO_TIP_SHOW_NR];               //������ǩ��ʾSDK_TipShow>
	bool vMobileCarFunction[SDK_MOBILEDVR_NR];//���ع���
	bool	vOtherFunction[SDK_OTHER_NR];				//��������OtherFunction
};

struct SDK_COMMATTRI
{
	int	iDataBits;	// ����λȡֵΪ5,6,7,8 
	int	iStopBits;	// ֹͣλ
	int	iParity;	// У��λ
	int	iBaudRate;	// ʵ�ʲ�����
};

// ��������
struct SDK_CONFIG_COMM_X
{
	char iProtocolName[32];	// ����Э��:��Console�� 
	int iPortNo;		// �˿ں� 
	SDK_COMMATTRI aCommAttri;		// �������� 
};

struct SDK_CommConfigAll
{
	SDK_CONFIG_COMM_X vCommConfig[SDK_COM_TYPES];
};


// ��̨����
struct SDK_STR_CONFIG_PTZ
{
	char sProtocolName[NET_MAX_PTZ_PROTOCOL_LENGTH];	// Э������ 	
	int	ideviceNo;				// ��̨�豸��ַ��� 	
	int	iNumberInMatrixs;		// �ھ����е�ͳһ���	
	int iPortNo;				// ���ڶ˿ں�	[1, 4] 	
	SDK_COMMATTRI dstComm;			// �������� 	
};

//����ͨ����̨Э��
struct SDK_STR_PTZCONFIG_ALL
{
	SDK_STR_CONFIG_PTZ ptzAll[NET_MAX_CHANNUM];
};

//RS485
struct SDK_STR_RS485CONFIG_ALL
{
	SDK_STR_CONFIG_PTZ ptzAll[NET_MAX_CHANNUM];
};


struct SDK_CONFIG_WORKSHEET
{	
	SDK_TIMESECTION	tsSchedule[NET_N_WEEKS][NET_N_TSECT];	/*!< ʱ��� */
};

/// ¼��ģʽ����
enum SDK_RecordModeTypes
{
	SDK_RECORD_MODE_CLOSED,		///< �ر�¼��
	SDK_RECORD_MODE_MANUAL,		///< �ֶ�¼��
	SDK_RECORD_MODE_CONFIG,		///< ������¼��
	SDK_RECORD_MODE_NR,		
};

///< ¼������
struct SDK_RECORDCONFIG
{
	int iPreRecord;			///< Ԥ¼ʱ�䣬Ϊ��ʱ��ʾ�ر�	
	bool bRedundancy;		///< ���࿪��
	bool bSnapShot;			///< ���տ���	
	int iPacketLength;		///< ¼�������ȣ����ӣ�[1, 255]
	int iRecordMode;		///< ¼��ģʽ��0 �رգ�1 ��ֹ 2 ����
	SDK_CONFIG_WORKSHEET wcWorkSheet;			///< ¼��ʱ���	
	unsigned int typeMask[NET_N_WEEKS][NET_N_TSECT];		///< ¼����������
};

//¼�����ýṹ��
struct SDK_RECORDCONFIG_ALL
{
	SDK_RECORDCONFIG vRecordConfigAll[NET_MAX_CHANNUM];
};

///< ͼƬ����
struct SDK_SnapshotConfig
{
	int iPreSnap;			///< ԤץͼƬ��
	bool bRedundancy;		///< ���࿪��
	int iSnapMode;		///< ¼��ģʽ����RecordModeTypes
	SDK_CONFIG_WORKSHEET wcWorkSheet;				///< ¼��ʱ���	
	unsigned int typeMask[NET_N_WEEKS][NET_N_TSECT];	///< ¼���������룬��enum RecordTypes
};

struct SDK_SnapshotConfigAll
{
	SDK_SnapshotConfig vSnapshotConfigAll[NET_MAX_CHANNUM];
};


//������ؽṹ��
//  ��̨�����ṹ
struct SDK_PtzLinkConfig
{
	int iType;		// ���������� 
	int iValue;		// ���������Ͷ�Ӧ��ֵ 
};

#define CHANNELNAME_MAX_LEN 64  //ͨ��������󳤶�

struct SDK_EventHandler
{
	unsigned int	dwRecord;				// ¼������ 
	int		iRecordLatch;			// ¼����ʱ��10��300 sec  	
	unsigned int	dwTour;					// ��Ѳ���� 	
	unsigned int	dwSnapShot;				// ץͼ���� 
	unsigned int	dwAlarmOut;				// �������ͨ������ 
	unsigned int	dwMatrix;				// �������� 
	int		iEventLatch;			// ������ʼ��ʱʱ�䣬sΪ��λ 
	int		iAOLatch;				// ���������ʱ��10��300 sec  
	SDK_PtzLinkConfig PtzLink[NET_MAX_CHANNUM];		// ��̨������ 
	SDK_CONFIG_WORKSHEET schedule;		// ¼��ʱ���

	bool	bRecordEn;				// ¼��ʹ�� 
	bool	bTourEn;				// ��Ѳʹ�� 
	bool	bSnapEn;				// ץͼʹ�� 	
	bool	bAlarmOutEn;			// ����ʹ�� 
	bool	bPtzEn;

	// ��̨����ʹ�� 
	bool	bTip;					// ��Ļ��ʾʹ�� 	
	bool	bMail;					// �����ʼ� 	
	bool	bMessage;				// ������Ϣ���������� 	
	bool	bBeep;					// ���� 	
	bool	bVoice;					// ������ʾ 		
	bool	bFTP;					// ����FTP���� 
	bool	bMatrixEn;				// ����ʹ�� 
	bool	bLog;					// ��־ʹ��
	bool	bMessagetoNet;			// ��Ϣ�ϴ�������ʹ�� 

	bool    bShowInfo;              // �Ƿ���GUI�Ϻͱ�������ʾ������Ϣ
	unsigned int    dwShowInfoMask;         // Ҫ������ʾ������Ϣ��ͨ������
	char    pAlarmInfo[CHANNELNAME_MAX_LEN];//Ҫ��ʾ�ı�����Ϣ

	bool    bShortMsg;              //���Ͷ���
	bool    bMultimediaMsg;         //���Ͳ���
};

enum SDK_ANALYSE_MODE
{
	SDK_ANALYSE_TRIPWIRE = 0, //������
	SDK_ANALYSE_PERIMETER     //�ܽ���
};
/** �ܽ���ģʽ */
enum SDK_URP_PERIMETER_MODE_E
{
	SDK_URP_PMODE_INTRUSION = 0,			/**< ���� */
	SDK_URP_PMODE_ENTER,				/**< ���� */
	SDK_URP_PMODE_EXIT				/**< �뿪 */
};

/** ���߽���� */
#define SDK_MAX_BOUNDARY_POINT_NUM 8
#define SDK_MAX_POINT_NUM 4
#define SDK_NAME_BUF_SIZE 16

/** ����� */
struct URP_SDK_POINT_S
{
	short X;					/**< x */
	short Y;					/**< y */
};

/** �߽ṹ */
struct SDK_URP_LINE_S
{
	URP_SDK_POINT_S stStartPt;			/**< �߶���� */
	URP_SDK_POINT_S stEndPt;			/**< �߶��յ� */
};

struct SDK_URP_POLYGON_REGION_S
{
	int iValid;
	int iPointNum;
	URP_SDK_POINT_S astPoint[SDK_MAX_BOUNDARY_POINT_NUM];
};

/** URP�ܽ���߽�� */
struct SDK_URP_PERIMETER_LIMIT_BOUNDARY_S
{
	int      iBoundaryPtNum;					/**< �߽����Ŀ */
	URP_SDK_POINT_S   astBoundaryPts[SDK_MAX_BOUNDARY_POINT_NUM];	/**< �߽�㼯�� */
};

/** URP�ܽ������Ʋ��� */
struct SDK_URP_PERIMETER_LIMIT_PARA_S
{
	int iMinDist;						/**< ��С���ؾ��� */
	int iMinTime;						/**< ���ʱ�� */
	int	iDirectionLimit;					/**< �Ƿ����������� */
	int iForbiddenDirection;					/**< ��ֹ����Ƕ�(��λ: �Ƕ�) */
	SDK_URP_PERIMETER_LIMIT_BOUNDARY_S stBoundary;			/**< �ܽ�߽����� */
};

/** URP�ܽ��������� */
struct SDK_URP_PERIMETER_RULE_PARA_S
{
	int iTypeLimit;						/**< Ŀ���������� */
	int iTypeHuman;						/**< Ŀ�������(�޶�����ʱʹ��) */
	int iTypeVehicle;						/**< Ŀ����𣺳� */
	int iMode;						/**< �ܽ�ģʽ */
	SDK_URP_PERIMETER_LIMIT_PARA_S stLimitPara;				/**< �ܽ����Ʋ��� */
} ;

// ----------------------------------------------------------------------
//  �������ݽṹ
/** URP������������� */
#define SDK_URP_MAX_TRIPWIRE_CNT 8

/** URP�����߽ṹ */
struct SDK_URP_TRIPWIRE_S
{
	int     iValid;						/**< �Ƿ���Ч */
	int    iIsDoubleDirection;				/**< ��ʾ�ð����Ƿ�Ϊ˫�����(0: ��, 1: ��) */
	int     iForbiddenDirection;				/**< �����߽�ֹ����*/
	SDK_URP_LINE_S	stLine;						/**< ������λ�� */
};

/** URP���������Ʋ��� */
struct SDK_URP_TRIPWIRE_LIMIT_PARA_S
{
	int iMinDist;						/**< ��С���ؾ��� */
	int iMinTime;						/**< ���ʱ�� */
};

/** URP�����߹������ */
struct SDK_URP_TRIPWIRE_RULE_PARA_S
{
	int iTypeLimit;						/**< Ŀ���������� */
	int iTypeHuman;						/**< Ŀ�������(�޶�����ʱʹ��) */
	int iTypeVehicle;						/**< Ŀ����𣺳� */
	SDK_URP_TRIPWIRE_S astLines[SDK_URP_MAX_TRIPWIRE_CNT];		/**< ������ */
	SDK_URP_TRIPWIRE_LIMIT_PARA_S stLimitPara;				/**< ���������Ʋ��� */
};


/** URPĿ����������ȼ��� */
enum SDK_SENSITIVITY_LEVEL_E
{
	SDK_HIGH_LEVEL_SENSITIVITY,         /**< �������� */
	SDK_MIDDLE_LEVEL_SENSITIVITY,		/**< �������� */
	SDK_LOW_LEVEL_SENSITIVITY			/**< �������� */
};

/** �㷨ģ�� */

enum SDK_MODULE_TYPE
{
	SDK_MODULE_TYPE_PEA = 0,
	SDK_MODULE_TYPE_OSC,
	SDK_MODULE_TYPE_AVD,
	SDK_MODULE_TYPE_ALL
};

// ----------------------------------------------------------------------
// ���濪ʼ���Զ���ṹ

struct SDK_PEA_RULE_S
{
	int iShowTrack;						/// �Ƿ���ʾ�켣
	int iShowRule;						/// �Ƿ���ʾ����
	int iLevel;						/// ���伶�������ȣ�0���߼���Ĭ��ֵ����1���м���2���ͼ���
	int iPerimeterEnable;					/// �ܽ����ʹ��
	SDK_URP_PERIMETER_RULE_PARA_S	stPerimeterRulePara;	/**< �ܽ��� */
	int iTripWireEnable;					/// �����߹���ʹ��
	SDK_URP_TRIPWIRE_RULE_PARA_S	stTripwireRulePara;	/**< �����߼�� */
};

/// �㷨�����ṹ
struct SDK_CAPS					/// ��1��ʾ��ͨ��֧�ֶ�Ӧ�������㷨����0��ʾ��֧�֡�
{
	int dwPEA;
	int dwAVD;
	int dwVFD;
	int dwAAI;
	int dwOSC;
	int iResv[3];
} ;

struct SDK_PEA_STATUS_S
{
	int dwPerimeter;					/// ���ּ��״̬�������˰��ߺ��ܽ�,��uintֵ�Ǹ�����������ͨ������״̬�����롣
	/// ��ͨ���ڵ�λ����ͨ���ڸ�λ��������1���ޱ�����0�������ڵ�ͨ����0��
	int iResv[4];
	int avdStatue;
};


//��Ʒ����-begin
#define NET_MAX_OSC_NUM 4
#define NET_MAX_POINT_NUM 4
#define NET_NAME_BUF_SIZE 16

struct SDK_URP_OSC_SPECL_REGIONS_S
{
	int nValid; //�Ƿ���Ч
	char chName[NET_NAME_BUF_SIZE]; //��������
	SDK_URP_POLYGON_REGION_S stOscRg; //Ҫ���������
	SDK_URP_POLYGON_REGION_S astSubRgA; //��������е���Ч������
	SDK_URP_POLYGON_REGION_S astSubRgB; //��������е���Ч������
	SDK_URP_POLYGON_REGION_S astSubRgC; //��������е���Ч������
};

struct SDK_URP_OSC_LMT_PARA_S
{
	int nTimeMin; //ʱ��Լ��
	int nSizeMin; //��С�ߴ�
	int nSizeMax; //���ߴ�
} ;

struct SDK_URP_OSC_RULE_PARA_S
{
	int nSceneType; //��������
	int nCameraType; //�������
	SDK_URP_OSC_SPECL_REGIONS_S astSpclRgs[NET_MAX_OSC_NUM]; //Ҫ������������
	SDK_URP_OSC_LMT_PARA_S stOscPara; //һЩԼ��
} ;

struct SDK_OSC_RULE_S
{
	int iShowTrack;				/// �Ƿ���ʾ�켣
	int iShowRule;						     /// �Ƿ���ʾ����
	int iLevel;						     /// ���伶��0���Ǿ�����Ĭ��ֵ����1���ͼ���2���м���3: �߼�
	int iAbandumEnable;		   	/// ��Ʒ����ʹ��
	SDK_URP_OSC_RULE_PARA_S stObjAbandumRulePara; /// ��Ʒ��������
	int iStolenEnable;			  	 /// ��Ʒ����ʹ��
	SDK_URP_OSC_RULE_PARA_S stObjStolenRulePara;   /// ��Ʒ��������
	int iNoParkingEnable;		 	  	 /// �Ƿ�ͣ��ʹ��
	SDK_URP_OSC_RULE_PARA_S stNoParkingRulePara;   /// �Ƿ�ͣ������
};


struct SDK_AVD_RULE_S
{
	int iLevel;						     /// �����ȵ�λ��1-5������ֵԽ��Խ����Խ�ױ���
	int itBrightAbnmlEnable;				///< �����쳣ʹ��
	int iClarityEnable;					///< �����ȼ��ʹ��
	int iNoiseEnable;					///< �������ʹ��
	int iColorEnable;					///< ƫɫ���ʹ��
	int iFreezeEnable;					///< ���涳����ʹ��
	int iNosignalEnable;					///< �ź�ȱʧ���ʹ��
	int iChangeEnable;					///< �����任���ʹ��
	int iInterfereEnable;					///< ��Ϊ���ż��ʹ�� 
	int iPtzLoseCtlEnable;					///< PTZʧ�ؼ��ʹ��
};

/** ����� */
struct SDK_URP_IMP_POINT_S
{
	short s16X;					/**< x */
	short s16Y;					/**< y */
};

struct SDK_CPC_RULE_S
{
	SDK_URP_IMP_POINT_S stRulePoint[4];	///��������Ҫ����4����	
	int s32Sizemin;			///��С���(0,height/2)
	int s32Sizemax;			///������(sizemin, height/2)
	int s32Countmax;			///���Ŀ����[1~15],
	int s32Sensitivity;		/// �����ȵ�λ��1-5������ֵԽ��Խ����Խ�ױ���	
	unsigned int u32Flag;				///��־λ�������жϽ�����Ĭ����0
	int s32EnterDirection;	///����ķ���
};

//�����㷨�Ĺ������ü���
struct SDK_RULECONFIG
{
	SDK_PEA_RULE_S stRulePea;   //�ܽ������������
	SDK_OSC_RULE_S stRuleOSC; //��Ʒ���ơ��������Ƿ�ͣ������
	SDK_AVD_RULE_S stRuleAVD;//�쳣��Ƶ�ź��������
	SDK_CPC_RULE_S stRuleCPC; //��ͷ�����㷨����
};


//һ��ͨ�������ܷ�������
struct SDK_ANALYSECONFIG
{
	bool         bEnable;      //�Ƿ�������Ƶ��������
	int          moduleType;   //ʹ�õ������㷨ģ��
	SDK_EventHandler hEvent;  //�����¼�
	SDK_RULECONFIG   stRuleConfig; //�����㷨�Ĺ�������
};

//����ͨ�������ܷ�������
struct SDK_ANALYSECONFIG_ALL
{
	SDK_ANALYSECONFIG vAnalyzeAll[NET_MAX_CHANNUM];
};

//���ܷ�������
struct SDK_ANALYZEABILITY
{
	UINT uIntelPEA; //����PEA֧�֣�λ��Ӧͨ��
	UINT uAlgorithmPEA;//PEA֧�ֵ��㷨���ͣ��������
	UINT uIntelOSC; //����OSC֧�֣�λ��Ӧͨ��
	UINT uAlgorithmOSC;//OSC֧�ֵ��㷨����
	UINT uIntelAVD; //����AVD֧�֣�λ��Ӧͨ��
	UINT uAlgorithmAVD;//AVD֧�ֵ��㷨����
	UINT uIntelCPC; //����������֧�֣�λ��Ӧͨ��
	UINT uAlgorithmCPC;//CPC֧�ֵ��㷨����
};



///< ��̬�������
struct SDK_MOTIONCONFIG
{
	bool bEnable;							// ��̬��⿪�� 
	int iLevel;								// ������ 
	unsigned int mRegion[NET_MD_REGION_ROW];			// ����ÿһ��ʹ��һ�������ƴ� 	
	SDK_EventHandler hEvent;					// ��̬������� 
};

/// ȫͨ����̬�������
struct SDK_MOTIONCONFIG_ALL
{
	SDK_MOTIONCONFIG vMotionDetectAll[NET_MAX_CHANNUM];
};

///< �ڵ��������
struct SDK_BLINDDETECTCONFIG
{
	bool	bEnable;		///< �ڵ���⿪��
	int		iLevel;			///< �����ȣ�1��6
	SDK_EventHandler hEvent;	///< �ڵ��������
};

/// ȫͨ���ڵ��������
struct SDK_BLINDDETECTCONFIG_ALL
{
	SDK_BLINDDETECTCONFIG vBlindDetectAll[NET_MAX_CHANNUM];
};

///< �����¼��ṹ
struct SDK_VIDEOLOSSCONFIG
{
	bool bEnable;			///< ʹ��
	SDK_EventHandler hEvent;	///< �������
};

/// ����ͨ���Ļ���ʱ��ṹ
struct SDK_VIDEOLOSSCONFIG_ALL
{
	SDK_VIDEOLOSSCONFIG vGenericEventConfig[NET_MAX_CHANNUM];
};

///< ������������
struct SDK_ALARM_INPUTCONFIG
{
	bool	bEnable;		///< �������뿪��
	int		iSensorType;	///< ���������ͳ��� or ����
	SDK_EventHandler hEvent;	///< ��������
};

///< ����ͨ���ı�����������
struct SDK_ALARM_INPUTCONFIG_ALL
{
	SDK_ALARM_INPUTCONFIG vAlarmConfigAll[NET_MAX_CHANNUM];
};

///< ��·����
struct SDK_NETALARMCONFIG
{
	bool bEnable;			///< ʹ��
	SDK_EventHandler hEvent;	///< �������
};

/// ����ͨ������·�����ṹ
struct SDK_NETALARMCONFIG_ALL
{
	SDK_NETALARMCONFIG vNetAlarmConfig[NET_MAX_CHANNUM];
};

///< ���ر����������
struct SDK_AlarmOutConfig
{
	int nAlarmOutType;		///< �����������: ����,�ֶ�,�ر�
	int nAlarmOutStatus;    ///< ����״̬: 0:�� 1;�պ�
};

///< Ӳ�����������¼��ṹ
struct SDK_StorageLowSpaceConfig
{
	bool bEnable;
	int iLowerLimit;		///< Ӳ��ʣ����������, �ٷ���
	SDK_EventHandler hEvent;	///< �������
	bool bRecordTimeEnable;
	int iRecordTime;		///<¼������
};

///< ����ͨ���ı����������
struct SDK_AlarmOutConfigAll
{
	SDK_AlarmOutConfig vAlarmOutConfigAll[NET_MAX_CHANNUM];
};

/// ����ͨ���Ľ�������ַ����V2�汾
struct SDK_AbilitySerialNo
{
	char serialNo[NET_MAX_SERIALNO_LENGTH];
	char productType[NET_MAX_SERIALNO_LENGTH];
};

///< ��������Ϣ�ṹ
struct SDK_DriverInformation 
{
	int		iDriverType;		///< ����������
	bool	bIsCurrent;			///< �Ƿ�Ϊ��ǰ������
	unsigned int	uiTotalSpace;		///< ��������MBΪ��λ
	unsigned int	uiRemainSpace;		///< ʣ��������MBΪ��λ
	int		iStatus;			///< �����־���ļ�ϵͳ��ʼ��ʱ������
	int		iLogicSerialNo;				///< �߼����
	SDK_SYSTEM_TIME  tmStartTimeNew;		///< ��¼��ʱ��εĿ�ʼʱ��
	SDK_SYSTEM_TIME	 tmEndTimeNew;			///< ��¼��ʱ��εĽ���ʱ��
	SDK_SYSTEM_TIME	 tmStartTimeOld;		///< ��¼��ʱ��εĿ�ʼʱ��
	SDK_SYSTEM_TIME	 tmEndTimeOld;			///< ��¼��ʱ��εĽ���ʱ��
};
enum
{
	SDK_MAX_DRIVER_PER_DISK = 2,	///< ÿ���������ķ�����
	SDK_MAX_DISK_PER_MACHINE = 8,	///< ���֧��8��Ӳ��
};
//Ӳ�̹���
struct SDK_STORAGEDISK
{
	int		iPhysicalNo;
	int		iPartNumber;		// ������
	SDK_DriverInformation diPartitions[SDK_MAX_DRIVER_PER_DISK];
};

struct SDK_StorageDeviceInformationAll
{
	int iDiskNumber;
	SDK_STORAGEDISK vStorageDeviceInfoAll[SDK_MAX_DISK_PER_MACHINE];
};

/// �洢�豸��������
enum SDK_StorageDeviceControlTypes
{
	SDK_STORAGE_DEVICE_CONTROL_SETTYPE,		///< ��������
	SDK_STORAGE_DEVICE_CONTROL_RECOVER,		///< �ָ�����
	SDK_STORAGE_DEVICE_CONTROL_PARTITIONS,	///< ��������
	SDK_STORAGE_DEVICE_CONTROL_CLEAR,		///< �������
	SDK_STORAGE_DEVICE_CONTROL_NR,
};

/// ���������������
enum SDK_StorageDeviceClearTypes
{
	SDK_STORAGE_DEVICE_CLEAR_DATA,			///< ���¼������
	SDK_STORAGE_DEVICE_CLEAR_PARTITIONS,	///< �������
	SDK_STORAGE_DEVICE_CLEAR_NR,
};

/// ����������
enum SDK_FileSystemDriverTypes
{
	SDK_DRIVER_READ_WRITE	= 0,	///< ��д������
	SDK_DRIVER_READ_ONLY	= 1,	///< ֻ��������
	SDK_DRIVER_EVENTS		= 2,	///< �¼�������
	SDK_DRIVER_REDUNDANT	= 3,	///< ����������
	SDK_DRIVER_SNAPSHOT		= 4,	///< ����������
	SDK_DRIVER_TYPE_NR		= 5,	///< ���������͸���
	SDK_DRIVER_UNUSED		= 0xff,	///< û��ʹ�õ��������ṹ
};

/// �洢�豸����
struct SDK_StorageDeviceControl
{
	int iAction;	///< ��enum SDK_StorageDeviceControlTypes
	int iSerialNo;	///< �������к�
	int iPartNo;    ///< ������
	int iType;		///< enum SDK_StorageDeviceClearTypes����SDK_FileSystemDriverTypes
	int iPartSize[2/*MAX_DRIVER_PER_DISK*/];	///< ���������Ĵ�С
};

/// �豸����
enum SDK_DeviceType
{
	SDK_DEVICE_TYPE_DVR,	///< ��ͨDVR�豸
	SDK_DEVICE_TYPE_NVS,	///< NVS�豸
	SDK_DEVICE_TYPE_IPC,	///< IPC�豸
	SDK_DEVICE_TYPE_HVR,	///<���dvr
	SDK_DEVICE_TYPE_IVR,	///<����dvr
	SDK_DEVICE_TYPE_MVR,	///<����dvr
	SDK_DEVICE_TYPE_NR
};

/// �豸��Ϣ
typedef struct _H264_DVR_DEVICEINFO
{
	char sSoftWareVersion[64];	///< ����汾��Ϣ
	char sHardWareVersion[64];	///< Ӳ���汾��Ϣ
	char sEncryptVersion[64];	///< ���ܰ汾��Ϣ
	struct SDK_SYSTEM_TIME tmBuildTime;///< �������ʱ��
	char sSerialNumber[64];			///< �豸���к�	
	int byChanNum;				///< ��Ƶ����ͨ����
	int iVideoOutChannel;		///< ��Ƶ���ͨ����
	int byAlarmInPortNum;		///< ��������ͨ����
	int byAlarmOutPortNum;		///< �������ͨ����
	int iTalkInChannel;			///< �Խ�����ͨ����
	int iTalkOutChannel;		///< �Խ����ͨ����
	int iExtraChannel;			///< ��չͨ����	
	int iAudioInChannel;		///< ��Ƶ����ͨ����
	int iCombineSwitch;			///< ��ϱ���ͨ���ָ�ģʽ�Ƿ�֧���л�
	int iDigChannel;		///<����ͨ����
	unsigned int uiDeviceRunTime;	///<ϵͳ����ʱ��
	enum SDK_DeviceType deviceTye;	///�豸����
	char sHardWare[64];		///<�豸�ͺ�
}H264_DVR_DEVICEINFO,*LPH264_DVR_DEVICEINFO;

//��������ص�����
struct H264_DVR_ACTIVEREG_INFO
{
	char deviceSarialID[64];//�豸���кţ��������64λ��ֵ0
	H264_DVR_DEVICEINFO deviceInfo;//�豸��Ϣ
};
///< �Զ�ά������
struct SDK_AutoMaintainConfig
{
	int iAutoRebootDay;				///< �Զ�������������
	int iAutoRebootHour;			///< ��������ʱ��	[0, 23]	
	int iAutoDeleteFilesDays;		///< �Զ�ɾ���ļ�ʱ��[0, 30]
};

/// Ĭ����������
enum SDK_DefaultConfigKinds
{
	SDK_DEFAULT_CFG_GENERAL,			// ��ͨ����
	SDK_DEFAULT_CFG_ENCODE,				// ��������
	SDK_DEFAULT_CFG_RECORD,				// ¼������
	SDK_DEFAULT_CFG_NET_SERVICE,		// �������
	SDK_DEFAULT_CFG_NET_COMMON,			// ͨ������
	SDK_DEFAULT_CFG_ALARM,				// ����
	SDK_DEFAULT_CFG_PTZCOMM,			// ��̨������
	SDK_DEFAULT_CFG_USERMANAGER,		// �û�����
	SDK_DEFAULT_CFG_PREVIEW,			// Ԥ������
	SDK_DEFAULT_CFG_CAMERA_PARAM,            // ��������ͷ����
	SDK_DEFAULT_CFG_END,
};

/// �ָ���Ĭ����������
struct SDK_SetDefaultConfigTypes
{
	bool vSetDefaultKinds[SDK_DEFAULT_CFG_END];
}; 


typedef struct H264_DVR_CLIENTINFO{
	int nChannel;	//ͨ����
	int nStream;	//0��ʾ��������Ϊ1��ʾ������
	int nMode;		//0��TCP��ʽ,1��UDP��ʽ,2���ಥ��ʽ,3 - RTP��ʽ��4-����Ƶ�ֿ�(TCP)
	int nComType;	//ֻ����ϱ���ͨ����Ч, ��ϱ���ͨ����ƴͼģʽ
	void* hWnd;
	H264_DVR_CLIENTINFO()
	{
		hWnd=0;
	}
	
	
}*LPH264_DVR_CLIENTINFO;


enum SDK_File_Type
{
	SDK_RECORD_ALL = 0,
	SDK_RECORD_ALARM = 1, //�ⲿ����¼��
	SDK_RECORD_DETECT,	  //��Ƶ���¼��
	SDK_RECORD_REGULAR,	  //��ͨ¼��
	SDK_RECORD_MANUAL,	  //�ֶ�¼��
	SDK_PIC_ALL = 10,
	SDK_PIC_ALARM,		  //�ⲿ����¼��
	SDK_PIC_DETECT,		  //��Ƶ���¼��
	SDK_PIC_REGULAR,      //��ͨ¼��
	SDK_PIC_MANUAL,       //�ֶ�¼��
	SDK_TYPE_NUM
};

//��ѯ¼������
struct H264_DVR_FINDINFO
{
	int nChannelN0;			//ͨ����
	int nFileType;			//�ļ�����, ��SDK_File_Type
	H264_DVR_TIME startTime;	//��ʼʱ��
	H264_DVR_TIME endTime;	//����ʱ��
	char szFileName[32];		//�ļ�����Ϊ�յĻ���ϵͳ������ֵ��ϵͳ����
	void *hWnd;
	H264_DVR_FINDINFO()
	{
		hWnd=NULL;
	}
};

//¼���ļ����ؽṹ��
struct H264_DVR_FILE_DATA 
{
	int ch;						//ͨ����
	int size;					//�ļ���С
	char sFileName[108];		///< �ļ���
	struct SDK_SYSTEM_TIME stBeginTime;	///< �ļ���ʼʱ��
	struct SDK_SYSTEM_TIME stEndTime;		///< �ļ�����ʱ��
	void *hWnd;
	H264_DVR_FILE_DATA()
	{
		hWnd=NULL;
	}
};

//�طŶ���
enum SEDK_PlayBackAction
{
	SDK_PLAY_BACK_PAUSE,		/*<! ��ͣ�ط� */
	SDK_PLAY_BACK_CONTINUE,		/*<! �����ط� */
	SDK_PLAY_BACK_SEEK,			/*<! �طŶ�λ��ʱ��sΪ��λ */
	SDK_PLAY_BACK_FAST,	        /*<! ���ٻط� */
	SDK_PLAY_BACK_SLOW,	        /*<! ���ٻط� */
	SDK_PLAY_BACK_SEEK_PERCENT, /*<! �طŶ�λ�ٷֱ� */
};

//��ʱ��β�ѯ
struct SDK_SearchByTime
{
	int nHighChannel;			///< 33~64¼��ͨ��������
	int nLowChannel;			///< 1~32¼��ͨ��������
	int nFileType;              ///< �ļ�����, ��SDK_File_Type
	struct SDK_SYSTEM_TIME stBeginTime;	    ///< ��ѯ��ʼʱ��
	struct SDK_SYSTEM_TIME stEndTime;		///< ��ѯ����ʱ��
	int    iSync;               ///< �Ƿ���Ҫͬ��
};

//ÿ��ͨ����¼����Ϣ
struct SDK_SearchByTimeInfo
{
	int iChannel;			    ///< ¼��ͨ����
	///< ¼���¼��720���ֽڵ�5760λ����ʾһ���е�1440����
	///< 0000:��¼�� 0001:F_COMMON 0002:F_ALERT 0003:F_DYNAMIC 0004:F_CARD 0005:F_HAND
	unsigned char cRecordBitMap[720];
};

struct SDK_SearchByTimeResult
{
	int nInfoNum;										 ///< ͨ����¼���¼��Ϣ����
	struct SDK_SearchByTimeInfo ByTimeInfo[NET_MAX_CHANNUM];    ///< ͨ����¼���¼��Ϣ
};

//������Ϣ
typedef struct SDK_ALARM_INFO
{
	int nChannel;
	int iEvent;
	int iStatus;
	struct SDK_SYSTEM_TIME SysTime;
}SDK_AlarmInfo;

/// ��־��ѯ����
struct SDK_LogSearchCondition
{
	int nType;	///< ��־����
	int iLogPosition;			///< ���ϴβ�ѯ�Ľ���ʱ����־ָ��
	struct SDK_SYSTEM_TIME stBeginTime;	///< ��ѯ��־��ʼʱ��
	struct SDK_SYSTEM_TIME stEndTime;		///< ��ѯ��־����ʱ��
};


struct SDK_LogItem
{
	char sType[24];	///< ��־����
	char sUser[32];	///< ��־�û�
	char sData[68];	///< ��־����
	struct SDK_SYSTEM_TIME stLogTime;	///< ��־ʱ��
	int iLogPosition;			///< ���ϴβ�ѯ�Ľ���ʱ����־ָ��
};

//��־������Ϣ
struct SDK_LogList
{
	int iNumLog;
	struct SDK_LogItem Logs[NET_MAX_RETURNED_LOGLIST];
};

/// �����Խ���ʽ
struct SDK_AudioInFormatConfigAll
{
	struct SDK_AudioInFormatConfig vAudioInFormatConfig[SDK_AUDIO_ENCODE_TYPES_NR];
};

/// �澯״̬
struct SDK_DVR_ALARMSTATE
{
	int iVideoMotion; ///< �ƶ����״̬,�������ʾͨ����,bit0����ͨ��һ,�Դ����� 1: �и澯 0: �޸澯
	int iVideoBlind; ///< ��Ƶ�ڵ�״̬,�������ʾͨ����,bit0����ͨ��һ,�Դ����� 1: �и澯 0: �޸澯
	int iVideoLoss;	///< ��Ƶ��ʧ״̬,�������ʾͨ����,bit0����ͨ��һ,�Դ����� 1: �и澯 0: �޸澯
	int iAlarmIn;	///< �澯����״̬,�������ʾͨ����,bit0����ͨ��һ,�Դ����� 1: �и澯 0: �޸澯
	int iAlarmOut;	///< �澯���״̬,�������ʾͨ����,bit0����ͨ��һ,�Դ����� 1: �и澯 0: �޸澯
};

// ͨ��״̬
struct SDK_DVR_CHANNELSTATE
{
	bool bRecord; ///< �Ƿ�����¼��
	int iBitrate;	///< ��ǰ����
};

// DVR����״̬
struct SDK_DVR_WORKSTATE
{
	struct SDK_DVR_CHANNELSTATE vChnState[NET_MAX_CHANNUM];
	struct SDK_DVR_ALARMSTATE vAlarmState;
};


/// ����ֵ, ����������
enum SDK_NetKeyBoardValue
{
	SDK_NET_KEY_0, SDK_NET_KEY_1, SDK_NET_KEY_2, SDK_NET_KEY_3, SDK_NET_KEY_4, SDK_NET_KEY_5, SDK_NET_KEY_6, SDK_NET_KEY_7, SDK_NET_KEY_8, SDK_NET_KEY_9,
	SDK_NET_KEY_10, SDK_NET_KEY_11, SDK_NET_KEY_12, SDK_NET_KEY_13, SDK_NET_KEY_14, SDK_NET_KEY_15, SDK_NET_KEY_16, SDK_NET_KEY_10PLUS,
	SDK_NET_KEY_UP = 20,     // �ϻ�����̨����
	SDK_NET_KEY_DOWN,        // �»�����̨����
	SDK_NET_KEY_LEFT,        // �������̨����
	SDK_NET_KEY_RIGHT,       // �һ�����̨����
	SDK_NET_KEY_SHIFT, 
	SDK_NET_KEY_PGUP,        // ��һҳ
	SDK_NET_KEY_PGDN,        // ��һҳ
	SDK_NET_KEY_RET,         // ȷ��
	SDK_NET_KEY_ESC,         // ȡ�����˳�
	SDK_NET_KEY_FUNC,        // �л����뷨
	SDK_NET_KEY_PLAY,        // ����/��ͣ
	SDK_NET_KEY_BACK,        // ����
	SDK_NET_KEY_STOP,        // ֹͣ
	SDK_NET_KEY_FAST,        // ���
	SDK_NET_KEY_SLOW,        // ����
	SDK_NET_KEY_NEXT,        // ��һ���ļ�
	SDK_NET_KEY_PREV,        // ��һ���ļ�
	SDK_NET_KEY_REC = 40,    // ¼������
	SDK_NET_KEY_SEARCH,      // ¼���ѯ
	SDK_NET_KEY_INFO,        // ϵͳ��Ϣ
	SDK_NET_KEY_ALARM,       // �澯���
	SDK_NET_KEY_ADDR,        // ң������ַ����
	SDK_NET_KEY_BACKUP,      // ����
	SDK_NET_KEY_SPLIT,       // ����ָ�ģʽ�л���ÿ��һ���л�����һ�����ģʽ
	SDK_NET_KEY_SPLIT1,      // ������
	SDK_NET_KEY_SPLIT4,      // �Ļ���
	SDK_NET_KEY_SPLIT8,      // �˻���
	SDK_NET_KEY_SPLIT9,      // �Ż���
	SDK_NET_KEY_SPLIT16,     // 16����
	SDK_NET_KEY_SHUT,        // �ػ�
	SDK_NET_KEY_MENU,        // �˵�
	SDK_NET_KEY_SPLIT25,	// 25����	
	SDK_NET_KEY_SPLIT36,     // 36����
	SDK_NET_KEY_PTZ = 60,    // ������̨����ģʽ
	SDK_NET_KEY_TELE,        // �䱶��
	SDK_NET_KEY_WIDE,        // �䱶��
	SDK_NET_KEY_IRIS_SMALL,  // ��Ȧ��
	SDK_NET_KEY_IRIS_LARGE,  // ��Ȧ��
	SDK_NET_KEY_FOCUS_NEAR,  // �۽�Զ
	SDK_NET_KEY_FOCUS_FAR,   // �۽���
	SDK_NET_KEY_BRUSH,       // ��ˢ
	SDK_NET_KEY_LIGHT,       // �ƹ�
	SDK_NET_KEY_SPRESET,     // ����Ԥ�õ�
	SDK_NET_KEY_GPRESET,     // ת��Ԥ�õ�
	SDK_NET_KEY_DPRESET,     // ���Ԥ�õ� 
	SDK_NET_KEY_PATTERN,     // ģʽ
	SDK_NET_KEY_AUTOSCAN,    // �Զ�ɨ�迪ʼ����
	SDK_NET_KEY_AUTOTOUR,    // �Զ�Ѳ��
	SDK_NET_KEY_AUTOPAN,     // ��ɨ��ʼ/����
};

/// ����״̬
enum SDK_NetKeyBoardState
{
	SDK_NET_KEYBOARD_KEYDOWN,	// ��������
	SDK_NET_KEYBOARD_KEYUP,		// �����ɿ�
};

struct SDK_NetKeyBoardData
{
	int iValue;
	int iState;
};

// ������Ϣ��ȡ
struct SDK_UpgradeInfo
{
	char szSerial[64];
	char szHardware[64];
	char szVendor[64];
	unsigned int uiLogoArea[2];
};

/// ���籨��
struct SDK_NetAlarmInfo
{
	int iEvent;  //Ŀǰδʹ��
	int iState;   //ÿbit��ʾһ��ͨ��,bit0:��һͨ��,0-�ޱ��� 1-�б���, ��������
};

enum SERIAL_TYPE
{
	RS232 = 0,
	RS485 = 1,
};


enum MEDIA_PACK_TYPE
{
	FILE_HEAD =	0,	    		// �ļ�ͷ
	VIDEO_I_FRAME = 1,			// ��ƵI֡
	VIDEO_B_FRAME =	2,			// ��ƵB֡
	VIDEO_P_FRAME = 3,			// ��ƵP֡
	VIDEO_BP_FRAME = 4,			// ��ƵBP֡
	VIDEO_BBP_FRAME	= 5,		// ��ƵB֡B֡P֡
	VIDEO_J_FRAME = 6,			// ͼƬ֡
	AUDIO_PACKET = 10,			// ��Ƶ��
};

typedef struct
{
	int		nPacketType;				// ������,��MEDIA_PACK_TYPE
	char*	pPacketBuffer;				// ��������ַ
	unsigned int	dwPacketSize;				// ���Ĵ�С

	// ����ʱ��
	int		nYear;						// ʱ��:��		
	int		nMonth;						// ʱ��:��
	int		nDay;						// ʱ��:��
	int		nHour;						// ʱ��:ʱ
	int		nMinute;					// ʱ��:��
	int		nSecond;					// ʱ��:��
	unsigned int 	dwTimeStamp;					// ���ʱ���λ����λΪ����
	unsigned int	dwTimeStampHigh;        //���ʱ���λ����λΪ����
	unsigned int   dwFrameNum;             //֡���
	unsigned int   dwFrameRate;            //֡��
	unsigned short uWidth;              //ͼ����
	unsigned short uHeight;             //ͼ��߶�
	unsigned int       Reserved[6];            //����
} PACKET_INFO_EX;


struct SDK_OEMInfo
{
	int nOEMID;                     //OEM ID
	char sCompanyName[NET_MAX_USERNAME_LENGTH]; //��˾��
	char sTel[NET_MAX_USERNAME_LENGTH];         //�绰
	char sAddr[NET_MAX_USERNAME_LENGTH];        //��ַ
};
typedef struct __TransComChannel//͸������
{
	enum SERIAL_TYPE TransComType;//SERIAL_TYPE
	unsigned int baudrate;
	unsigned int databits;
	unsigned int stopbits;
	unsigned int parity;
} TransComChannel;
enum SDK_State_Type
{
	DEV_STATE_DDNS=0,	
};

//���������.....

//�ع�����
struct SDK_ExposureCfg
{
	int  level;    //�ع�ȼ�
	unsigned int leastTime;//�Զ��ع�ʱ�����޻��ֶ��ع�ʱ�䣬��λ΢��
	unsigned int mostTime; //�Զ��ع�ʱ�����ޣ���λ΢��
};

//��������
struct SDK_GainCfg
{
	int gain;    //�Զ���������(�Զ���������)��̶�����ֵ
	int autoGain;//�Զ������Ƿ����ã�0:������  1:����
};

//�������������
struct SDK_CameraParam
{
	unsigned int whiteBalance;         //��ƽ��
	unsigned int dayNightColor;        //��ҹģʽ��ȡֵ�в�ɫ���Զ��л��ͺڰ�
	int elecLevel;             //�ο���ƽֵ
	unsigned int apertureMode;          //�Զ���Ȧģʽ
	unsigned int BLCMode;               //���ⲹ��ģʽ
	struct SDK_ExposureCfg exposureConfig;//�ع�����
	struct SDK_GainCfg     gainConfig;    //��������

	unsigned int PictureFlip;		//ͼƬ���·�ת
	unsigned int PictureMirror;	//ͼƬ���ҷ�ת(����)
	unsigned int RejectFlicker;	//�չ�Ʒ�������
	unsigned int EsShutter;		//���������Ź���

	int ircut_mode;		//IR-CUT�л� 0 = �����ͬ���л� 1 = �Զ��л�

	int dnc_thr;			//��ҹת����ֵ
	int ae_sensitivity;	//ae����������

	int Day_nfLevel;		//noise filter �ȼ���0-5,0���˲���1-5 ֵԽ���˲�Ч��Խ����
	int Night_nfLevel;
	int Ircut_swap;		//ircut ������= 0        ����= 1
};

//�������������
struct SDK_AllCameraParam
{
	struct SDK_CameraParam vCameraParamAll[NET_MAX_CHANNUM];   //���е�ͨ��
};

//�ع�������
struct SDK_CameraAbility
{
	int  count;      //֧���ع��ٶ�����
	unsigned int speeds[CAMERAPARA_MAXNUM]; //�ع��ٶ�
	int  status;     //����״̬  >= 0 ����    < 0 �쳣
	int  elecLevel;  //�ο���ƽֵ
	int  luminance;  //ƽ������
	char pVersion[64];//xm 2a�汾
	char reserve[32];//����
};

//���ز��ſ���
enum SDK_LoalPlayAction
{
	SDK_Local_PLAY_PAUSE,		/*<! ��ͣ���� */
	SDK_Local_PLAY_CONTINUE,		/*<! ������������ */
	SDK_Local_PLAY_FAST,	        /*<! ���ٲ��� */
	SDK_Local_PLAY_SLOW,	        /*<! ���ٲ��� */	
	
};

//��������
struct SDK_NetShortMsgCfg
{
	bool bEnable;       //�����ֻ����ŵĹ����Ƿ�����
	char pDesPhoneNum[MAX_RECIVE_MSG_PHONE_COUNT][16];
	int  sendTimes;     //��Ҫ��ÿ���ֻ����Ͷ��ٴζ���
};
//�ֻ���������
struct SDK_NetMultimediaMsgCfg
{
	bool bEnable;				// �����ֻ����ŵĹ����Ƿ�����
	char pDesPhoneNum[MAX_RECIVE_MSG_PHONE_COUNT][16]; //���ղ��ŵ��ֻ��ţ���֧��3���ֻ���
	char pGateWayDomain[40];	// ���ص�ַ��������IP
	int  gateWayPort;			// ���ض˿�
	char pMmscDomain[40];		// ���ŷ�������ַ��IP������
	int  mmscPort;				// ���ŷ������˿ں�
};

struct SDK_DASSerInfo
{
	bool enable;
	char serAddr[NET_NAME_PASSWORD_LEN];
	int  port;
	char userName[NET_NAME_PASSWORD_LEN];
	char passwd[NET_NAME_PASSWORD_LEN];
	char devID[NET_NAME_PASSWORD_LEN];
};
enum UploadDataType
{
	VEHICLE=0
};
//�������************************************************************************
//����״̬
enum SDK_CAR_STATUS_TYPE
{
	SDK_CAR_WORKING,             //�Ƿ�������
	SDK_CAR_LIGHT_LEFT_TURN,     //��ת���Ƿ����
	SDK_CAR_LIGHT_RIGHT_TURN,    //��ת���Ƿ����
	SDK_CAR_DOOR_LEFT_FRONT,     //��ǰ���Ƿ��
	SDK_CAR_DOOR_RIGHT_FRONT,    //��ǰ���Ƿ��
	SDK_CAR_DOOR_LEFT_BACK,      //������Ƿ��
	SDK_CAR_DOOR_RIGHT_BACK,     //�Һ����Ƿ��
	SDK_CAR_DOOR_BACK,           //�����Ƿ��
	SDK_CAR_BRAKE,               //�Ƿ��ɲ��
	SDK_CAR_URGENCY_ALARM,       //��������
	SDK_CAR_STATUS_NR, //״̬������Ŀ
};

//�ⲿ��������
enum SDK_IO_INPUT_TYPE
{
	SDK_LOCAL_ALARM_INPUT,//���ر�������
	SDK_RS232_INPUT,      //ͨ��232��������
	SDK_RS485_INPUT,      //ͨ��485��������
};

//�ⲿ��Ϣ�����복��״̬�Ķ�Ӧ��ϵ
struct SDK_CarStatusExchange
{
	int  statusType; //��һ�ֳ���״̬��������ת��
	bool bExist;     //�Ƿ��и���״̬����Ϣ���룬���ݳ�����ʵ�������������
	bool bEnable;    //�Ƿ������״̬
	int  inputType;  //����״̬��Ӧ����Ϣ�������ͣ���IO_INPUT_TYPEö�ٵ�ֵ��ȡ
	int  addr;       //��ַ�������Ǳ��ر��������һ��Ӧ0������ڶ���Ӧ1
	int  sensorType; //����(NO)�򳣱�(NC)����inputType�Ǳ��ر�������ʱ��Ч
};

struct SDK_CarStatusExchangeAll
{
	struct SDK_CarStatusExchange exchangeAll[16];
};

struct SDK_CarStatusNum  
{
	int iCarStatusNum;
};

struct SDK_CarDelayTimeConfig
{
	bool bStartDelay;
	bool bCloseDelay;

	int timeStartDelay;		//��λ:����
	int timeCloseDelay;		//��λ:����
};
enum SDK_network_media {
	SDK_NM_WIRED = 0,  //��������
	SDK_NM_WLAN,       //Wifi
	SDK_NM_3G,         //3G����
	SDK_NM_NR,    //������
};
//�����������ȼ�
typedef struct SDK_tagNetLinkOrder
{
	int netType;  //ȡֵ��network_media���͵�
	int netOrder; //����ԽС�����ȼ�Խ��,0��������ȼ�
}SDK_NetLinkOrder;
//�������ȼ�
struct SDK_NetOrderConfig
{
	bool         bEnable;        //�Ƿ������������ȼ�
	int          netCount;       //����������Ŀ
	SDK_NetLinkOrder pNetOrder[SDK_NM_NR];//�������ȼ�
	SDK_NetLinkOrder pReserve[7-SDK_NM_NR];//���µ���������Ԥ��
};
///�������ȼ�����
struct SDK_NetOrderFunction
{
	bool bNetOrder;
};
#define  MAX_CAR_INFOR 24
/// �����ƺ�
struct  SDK_CarPlates
{
	char sPlateName[MAX_CAR_INFOR];
};
enum NET_ISP
{
	ISP_AUTO, //�Զ�ѡ��
	ISP_TELCOM, //����
	ISP_UNIONCOM, //��ͨ
	ISP_CNC,//��ͨ
	ISP_MOBILE,//�ƶ�����ͨ
	ISP_USER,//�Զ���
};
//����ƽ̨��Ϣ����
typedef struct SDK_LocalSdkNetPlatformConfig
{
	int Enable;
	int nISP;                          ///�μ�NET_ISP
	char sServerName[32];
	char ID[32];
	char sUserName[32];
	char sPassword[32];
	CONFIG_IPAddress HostIP;		///< host ip
	int port;
}SDK_LOCALSDK_NET_PLATFORM_CONFIG;

//GPSУʱ����
struct SDK_GPSTimingConfig
{
	bool bEnable;      // �Ƿ�����
	int  timeChange;   // �����UTCʱ����Ҫ�ı���٣���λ:��
	int  updatePeriod; // ��������  ��λ:����
};
//���۽Ӿ�����ϵͳ
struct	SDK_GodEyeConfig
{
	bool	bEnable; 
	char MainSeverName[NET_NAME_PASSWORD_LEN]; //������
	int MainPort;	//���˿�
	char ExSeverName[NET_NAME_PASSWORD_LEN]; //��������
	int ExPort;	//���ö˿�
};
enum  SDK_DigManagerShowStatus
{
	SDK_DM_SHOW_NONE,
	SDK_DM_SHOW_ALL,
};


//����ͨ����ʾ״̬
struct SDK_DigitalManagerShow
{
	int  nDigitalManagerShowSta;
};

//NAT���䣬MTUֵ
struct SDK_NatConfig
{
    bool bEnable;
    int nMTU;             // ��Χ (100,1400)
};

enum SubConnType
{
	conn_realTimePlay=1,
	conn_talk,
	conn_playback
};
//*** */���豸ע��
enum SocketStyle
{
	TCPSOCKET=0,
	UDPSOCKET,	
	SOCKETNR
};


enum SDK_NatStatusType
{
	SDK_NAT_STATUS_DISENABLE,//δ����
	SDK_NAT_STATUS_PROBING,  //����̽��DNS	
	SDK_NAT_STATUS_CONNECTING,//�������ӷ�����
	SDK_NAT_STATUS_CONNECTED,//���ӳɹ�
};
//nat״̬��Ϣ
struct SDK_NatStatusInfo
{
	int	iNatStatus;
	char NatInfoCode[64];
};

/// ����ˮӡ����
struct SDK_WaterMarkConfig
{
	bool	bEnable;		//  ����ˮӡ
	char  sKey[8];
	char sUserData[16];
};

struct SDK_WaterMarkConfigAll
{
	struct SDK_WaterMarkConfig vWaterMarkConfigAll[NET_MAX_CHANNUM];
};

///��������̬����
struct SDK_EncodeStaticParam
{
	int profile;
	int level;
	int reserved1[4];
};


struct SDK_EncodeStaticParamAll
{
	struct SDK_EncodeStaticParam	EncodeStaticParamAll[NET_MAX_CHANNUM];	
};

enum
{
	sdk_maxCPCDataItem = 2048,
	sdk_maxCPCDataReturn = 512,
	sdk_itemtotalLength = 96,	//2λͨ���ţ�20λ��ʼʱ�䣬20λ����ʱ�䣬������13λ������,������
	sdk_maxpack=4,// sdk_maxCPCDataItem/sdk_maxCPCDataReturn
};

struct SDK_CPCDataItem		//����ͳ����ṹ��
{
	int channel;			//��Ӧ��ͨ����
	struct SDK_SYSTEM_TIME startTime;	//��ʼʱ��
	struct SDK_SYSTEM_TIME endTime;	//����ʱ��
	int inNum;			//��������
	int outNum;			//�뿪����
};

struct SDK_CPCData
{
	int nItemNum;
	struct SDK_CPCDataItem CPCData[sdk_maxCPCDataReturn];
};

struct SDK_CPCDataAll
{
	int nCount;
	struct SDK_CPCData nCpcData[sdk_maxpack] ;
};

/// ¼��洢�豸���ͣ��������͵�һ�ֻ��߶���
struct SDK_RecordStorageType
{
	bool SATA_as;
	bool USB_as;
	bool SD_as;
	bool DVD_as;
};

//VPN�ṹ��
struct SDK_VPNConfig
{
	bool Enable;		///< �Ƿ���
	CONFIG_IPAddress ServiceIp;						///< ������IP��ַ
	char UserName[NET_NAME_PASSWORD_LEN];		///< �û���
	char Password[NET_NAME_PASSWORD_LEN];		///< ����	
	CONFIG_IPAddress addr;		///< ���ź��õ�IP��ַ
};
//��ǰVGA�ֱ���
struct SDK_VGAresolution
{
	int nHeight;
	int nWidth;
};
//֧��VGA�ֱ����б�
struct SDK_VGAResolutionAbility
{
	int vVGANum;
	char vVGAList[VGA_MAXNUM][VGA_NAME_LENGTH];
};

//�豸�б�(�豸������)
struct SDK_NetDevList
{
	int vNetDevNum;
	struct SDK_CONFIG_NET_COMMON vNetDevList[DEV_LIST_SHOW];
};

//���ݿ���ƽ̨
/***�û���Ϣ***/ 

struct SDK_CONFIG_KAINENG_USERINFO
{
	char szUserName[16];           //�û�������5�����ֻ�ʮ����ĸ;
	char szUserAddr[128];           //�û���ַ��30�����ֻ�60����ĸ;
	char szUserAttr[16];           //�û����ʣ�5�����ֻ�ʮ����ĸ��

	char szBelongUnit[64];         //��ܵ�λ��15�����ֻ�30����ĸ;
	char szBelongUnitPhone[16];    //��ܵ�λ�绰��14λ���֣��ֻ���"-" �����;

	char szPoliceUnit[64];         //������λ��15�����ֻ�30����ĸ;
	char szPoliceUnitAddr[128];     //������λ��ַ��30�����ֻ�60����ĸ;
	char szPoliceUnitPhone[16];    //������λ�绰��14λ���֣��ֻ���"-"�����;

	char szPolicePhone[4][16];
	char szPolOrCenPhone[2][16];   //����/���ĵ绰1��2��14λ���֣��ֻ���"-" �����;

	char szFTP[64];                //FTP��50���ַ���
	char szEMail[64];              //E-Mail: 50���ַ���
	CONFIG_IPAddress PoliceIP;            //�Ӿ�����IP��ַ
	int nHttpPort;                 //�Ӿ�����IP�˿�
};

/***�豸��Ϣ***/
//���ò�������ö��
typedef enum SDK_GuardType
{
	 SDK_FORBID = 0,      //��ֹ
	 SDK_TEN_MIN,         //ʮ����
	 SDK_THIRTY_MIN,      //��ʮ����
	 SDK_SIXTY_MIN,       //��ʮ����
	 SDK_MAX,
}SDK_GUARDTYEP;

typedef struct 
{
	struct SDK_SYSTEM_TIME StartTime;
	struct SDK_SYSTEM_TIME EndTime;
}SDK_GUARDTIME;

struct SDK_CONFIG_KAINENG_DEVINFO
{
	int ProbeNum1;            //��������������������ÿ��������� 1�ŵ�10��̽����ѡ�� ��Ĭ������Ϊ <1��̽����>
	int ProbeNum2;            //��������������������ÿ��������� 1�ŵ�10��̽����ѡ�� ��Ĭ������Ϊ <1��̽����>

	int AlarmTime;                //���ѿ���ʱ�䣺��λ���ִ�000-999  Ĭ��Ϊ��180
	int nGuardType;               //���ò���, ����GUARDTYEP
	int nGuardArea;               //���߷������ͣ���λ���֣�ÿλ��С0���3����00000000 ��33333333;  Ĭ�ϣ�00000000��
	int  nProbeType;               //������������ ��ѡ��3��Ϊ <��ֹ>  <AND>  <OR>  Ĭ��Ϊ <��ֹ>

	char szRemConAddr[4][8];       //ң����1-4�ŵ�ַ ����λ16�����ַ�����գ���0-9  A-F����Ĭ��Ϊ�գ�
	char szProbeAddr[10][8];       //̽����1-10�ŵ�ַ����λ16�����ַ�����գ���0-9  A-F����Ĭ��Ϊ�գ�
	char szLinDevAddr[6][8];       //�����豸��1-6��ַ����λ16�����ַ�����գ���0-9  A-F����Ĭ��Ϊ�գ�

	//��ʱ������
	SDK_GUARDTIME szGuardTime[3];      //��ʱ������
};

/***��������***/
//��������ö��
typedef enum SDK_KNAlarmLink
{
	SDK_LINK_EMPTY = 0,
	SDK_LINK_SWITH,
	SDK_LINK_10S,
	SDK_LINK_30S,
	SDK_LINK_60S,
	SDK_LINK_200S,
	SDK_LINK_600S,
	SDK_LINK_METHOD_1,
	SDK_LINK_METHOD_2,
	SDK_LINK_MAX,
}SDK_KNALARMLINK;

struct SDK_CONFIG_KAINENG_LINKINFO
{
	int eLink[10][7];        //��������,ȡֵΪ��������ö��
};

//���ܶ��ƣ�3��������Ϣȫ�����
struct SDK_CONFIG_KAINENG_INFO
{
	struct SDK_CONFIG_KAINENG_USERINFO UserInfo;
	struct SDK_CONFIG_KAINENG_DEVINFO DevInfo;
	struct SDK_CONFIG_KAINENG_LINKINFO LinkInfo;
};

//�����豸Э��
enum SDK_TransferProtocol_V2
{
	SDK_TRANSFER_PROTOCOL_NETIP,
	SDK_TRANSFER_PROTOCOL_ONVIF,
	SDK_TRANSFER_PROTOCOL_NR_V2=4
};
 
struct SDK_EncodeStaticParamV2
{
	int profile; //��ǰ��̬����
	int reserved1[4]; //�����ֶ�
};

/*
==	DVR��̬����������������3��
==	0 ��֧�� 1 ֧��baseline 2 ֧��main profile 4 ֧��high profile
==  �������
*/
struct SDK_EncStaticParamAbility
{
	unsigned int encStaticParam;
};
// �����ϴ�ͨ���ص�����ԭ��
typedef void (CALL_METHOD *fUploadDataCallBack) (long lLoginID, long UploadType, char *pBuffer, unsigned long dwBufSize, unsigned long dwUser);

// ͸�����ڻص�����ԭ��
typedef void (CALL_METHOD *fTransComCallBack) (long lLoginID, long lTransComType, char *pBuffer, unsigned long dwBufSize, unsigned long dwUser);
//�������Ͽ��ص�ԭ��
typedef void (CALL_METHOD *fDisConnect)(long lLoginID, char *pchDVRIP, long nDVRPort, unsigned long dwUser);

//ԭʼ���ݻص�ԭ��
typedef int(CALL_METHOD *fRealDataCallBack) (long lRealHandle, long dwDataType, unsigned char *pBuffer,long lbufsize,long dwUser);
typedef int(CALL_METHOD *fRealDataCallBack_V2) (long lRealHandle, const PACKET_INFO_EX *pFrame, unsigned int dwUser);

// �ط�¼����Ȼص�ԭ��
typedef void(CALL_METHOD *fDownLoadPosCallBack) (long lPlayHandle, long lTotalSize, long lDownLoadSize, long dwUser);

//��Ϣ������,����ע�ᣩ�ص�ԭ��
typedef bool (CALL_METHOD *fMessCallBack)(long lLoginID, char *pBuf,
									   unsigned long dwBufLen, long dwUser);

//�����豸����ص�ԭ��
typedef void(CALL_METHOD *fUpgradeCallBack) (long lLoginID, long lUpgradechannel,
										   int nTotalSize, int nSendSize, long dwUser);

// �����Խ�����Ƶ���ݻص�����ԭ��
typedef void (CALL_METHOD *pfAudioDataCallBack)(long lVoiceHandle, char *pDataBuf, 
											 long dwBufSize, char byAudioFlag, long dwUser);


//���ز��Ž����ص�ԭ��
typedef void (CALL_METHOD * fLocalPlayFileCallBack)(long lPlayHand, long nUser);

//��Ϣ֡�ص�����
typedef void (CALL_METHOD  *InfoFramCallBack)(long lPlayHand, long nType, LPCSTR pBuf,long nSize, long nUser);



//�����Ӷ��߻ص�
typedef void (CALL_METHOD *fSubDisConnectCallBack)(long lLoginID, SubConnType type, long nChannel, long dwUser);

#ifdef WIN32
//RigisterDraw�ص�ԭ��
typedef void (CALL_METHOD * fPlayDrawCallBack)(long lPlayHand,HDC hDc,long nUser);
#else//linuxû�õ�����ص�
typedef void (CALL_METHOD * fPlayDrawCallBack)(long lPlayHand,void* hDc,long nUser);
#endif // WIN32  

//*** */SDK��ʼ��
H264_DVR_API long CALL_METHOD H264_DVR_Init(fDisConnect cbDisConnect, unsigned long dwUser);

//*** */SDK�˳�����
H264_DVR_API bool CALL_METHOD H264_DVR_Cleanup();

//��ȡ������Ϣ
H264_DVR_API long CALL_METHOD H264_DVR_GetLastError();

// ���������豸��ʱʱ��ͳ��Դ���
//nWaitTime:��λms������ʱĬ��5000ms,
//nTryTimes:����,������ʱĬ��3��
H264_DVR_API bool CALL_METHOD H264_DVR_SetConnectTime(long nWaitTime, long nTryTimes);


H264_DVR_API long CALL_METHOD H264_DVR_Login(char *sDVRIP, unsigned short wDVRPort, char *sUserName, char *sPassword,
							   LPH264_DVR_DEVICEINFO lpDeviceInfo, int *error,SocketStyle socketTyle=TCPSOCKET);

//���豸ע����չ�ӿ�
//���ӵ�½���� 0==web 1 ==�������� 2 == ��������
H264_DVR_API long CALL_METHOD H264_DVR_LoginEx(char *sDVRIP, unsigned short wDVRPort, char *sUserName, char *sPassword,
							   LPH264_DVR_DEVICEINFO lpDeviceInfo, int nType, int *error);

//*** */���豸ע��
H264_DVR_API long CALL_METHOD H264_DVR_Logout(long lLoginID);

//���ý���DVR��Ϣ�ص�, ���籨����Ϣ,��ID������
H264_DVR_API bool CALL_METHOD H264_DVR_SetDVRMessCallBack(fMessCallBack cbAlarmcallback, unsigned long lUser);

//���������ϴ�ͨ��
H264_DVR_API long CALL_METHOD H264_DVR_SetupAlarmChan(long lLoginID);
H264_DVR_API bool CALL_METHOD H264_DVR_CloseAlarmChan(long lLoginID);


//Զ�������豸�ӿ� �������ͼ�SDK_CONFIG_TYPE
H264_DVR_API long  CALL_METHOD H264_DVR_GetDevConfig(long lLoginID, unsigned long dwCommand, int nChannelNO, char * lpOutBuffer, unsigned long dwOutBufferSize, unsigned long* lpBytesReturned,int waittime = 1000);
H264_DVR_API long  CALL_METHOD H264_DVR_SetDevConfig(long lLoginID, unsigned long dwCommand, int nChannelNO, char * lpInBuffer, unsigned long dwInBufferSize, int waittime = 1000);
//�����������豸���ã�Ŀǰֻ֧�ֶ��������ý�������
H264_DVR_API long CALL_METHOD H264_DVR_SetConfigOverNet(unsigned long dwCommand, int nChannelNO, char * lpInBuffer, unsigned long dwInBufferSize, int waittime = 1000);

//��־��ѯ
H264_DVR_API bool CALL_METHOD H264_DVR_FindDVRLog(long lLoginID, SDK_LogSearchCondition *pFindParam, SDK_LogList *pRetBuffer, long lBufSize, int waittime = 2000);

//��ʵʱԤ��
H264_DVR_API long CALL_METHOD H264_DVR_RealPlay(long lLoginID, LPH264_DVR_CLIENTINFO lpClientInfo);
H264_DVR_API bool CALL_METHOD H264_DVR_StopRealPlay(long lRealHandle,void*hWnd=NULL);
H264_DVR_API long CALL_METHOD H264_DVR_PauseRealPlay(long lRealHandle, bool bPause);

//���ûص��������û��Լ�����ͻ����յ�������
H264_DVR_API bool CALL_METHOD H264_DVR_SetRealDataCallBack(long lRealHandle,fRealDataCallBack cbRealData, long dwUser);
H264_DVR_API bool CALL_METHOD H264_DVR_SetRealDataCallBack_V2(long lRealHandle,fRealDataCallBack_V2 cbRealData, long dwUser);

//����ص�����,�ú�����Ҫ��H264_DVR_StopRealPlayǰ����
H264_DVR_API bool CALL_METHOD H264_DVR_DelRealDataCallBack(long lRealHandle,fRealDataCallBack cbRealData, long dwUser);
H264_DVR_API bool CALL_METHOD H264_DVR_DelRealDataCallBack_V2(long lRealHandle,fRealDataCallBack_V2 cbRealData, long dwUser);

//¼���ѯ
//lLoginID		��½���
//lpFindInfo	��ѯ����
//lpFileData	���ҵ���¼�����ݣ��ⲿ���ڴ�
//lMaxCount		���¼����Ŀ
//findcount		���ҵ���¼����Ŀ
//waittime		��ѯ��ʱʱ��
H264_DVR_API long CALL_METHOD H264_DVR_FindFile(long lLoginID, H264_DVR_FINDINFO* lpFindInfo, H264_DVR_FILE_DATA *lpFileData, int lMaxCount, int *findcount, int waittime = 5000);

//¼���ѯ
//lLoginID		��½���
//lpFindInfo	��ѯ����
//lpFileData	���ҵ���¼�����ݣ��ⲿ���ڴ�
//waittime		��ѯ��ʱʱ��
H264_DVR_API long CALL_METHOD H264_DVR_FindFileByTime(long lLoginID, SDK_SearchByTime* lpFindInfo, SDK_SearchByTimeResult *lpFileData, int waittime = 10000);

//¼��ط�
//lLoginID		��½���
//sPlayBackFileName	�طŵ�¼������
//cbDownLoadPos ���Ȼص�		�û�֪ͨ�û��豸�Ƿ��Ѿ������ݷ������
								//�ͻ������ʵʱ��ʾ���ȣ�Ӧ�ô����������ȡʱ��������
								//���粿�ֲ���������������Ե�ǰ�������ݴ�С/�ܴ�С��������ȵĻ����Ǻ�׼��Ӧ���Ե�ǰʱ�䣬���ݿ�ʼʱ��ͽ���ʱ�����������
//fDownLoadDataCallBack ���ݻص�
//dwDataUser	�ص�����
H264_DVR_API long CALL_METHOD H264_DVR_PlayBackByName(long lLoginID, H264_DVR_FILE_DATA *sPlayBackFile, fDownLoadPosCallBack cbDownLoadPos, fRealDataCallBack fDownLoadDataCallBack, long dwDataUser);
H264_DVR_API long CALL_METHOD H264_DVR_PlayBackByName_V2(long lLoginID, H264_DVR_FILE_DATA *sPlayBackFile, fDownLoadPosCallBack cbDownLoadPos, fRealDataCallBack_V2 fDownLoadDataCallBack, long dwDataUser);

//��ʱ�����¼��ط�
H264_DVR_API long CALL_METHOD H264_DVR_PlayBackByTime(long lLoginID, H264_DVR_FINDINFO* lpFindInfo, fDownLoadPosCallBack cbDownLoadPos, fRealDataCallBack fDownLoadDataCallBack, long dwDataUser);
//H264_DVR_API long CALL_METHOD H264_DVR_PlayBackByTimeEx(long lLoginID, int nChannelID, LPH264_DVR_TIME lpStartTime,
//											LPH264_DVR_TIME lpStopTime, fRealDataCallBack fDownLoadDataCallBack, long dwDataUser,
//											fDownLoadPosCallBack cbDownLoadPos, long dwPosUser);

H264_DVR_API long CALL_METHOD H264_DVR_PlayBackByTimeEx(long lLoginID, H264_DVR_FINDINFO* lpFindInfo,fRealDataCallBack fDownLoadDataCallBack, long dwDataUser,
											fDownLoadPosCallBack cbDownLoadPos, long dwPosUser);

H264_DVR_API bool CALL_METHOD H264_DVR_StopPlayBack(long lPlayHandle);

//�طſ��Ʒ�,��ͣ,��λ�ȵ�
H264_DVR_API bool CALL_METHOD H264_DVR_PlayBackControl(long lPlayHandle, long lControlCode,long lCtrlValue);

//¼������,�û����Բ�ʹ�ûص����Լ�ͨ��H264_DVR_GetDownloadPos��ȡ����
H264_DVR_API long CALL_METHOD H264_DVR_GetFileByName(long lLoginID,H264_DVR_FILE_DATA *sPlayBackFile,char *sSavedFileName, 
											fDownLoadPosCallBack cbDownLoadPos = NULL, long dwDataUser = NULL,fRealDataCallBack fDownLoadDataCallBack = NULL);
H264_DVR_API long CALL_METHOD H264_DVR_GetFileByTime(long lLoginID, H264_DVR_FINDINFO* lpFindInfo, char *sSavedFileDIR, bool bMerge = false,
											fDownLoadPosCallBack cbDownLoadPos = NULL, long dwDataUser = NULL ,fRealDataCallBack fDownLoadDataCallBack =NULL );

H264_DVR_API bool CALL_METHOD H264_DVR_StopGetFile(long lFileHandle);
//��ȡ���ؽ���
H264_DVR_API int CALL_METHOD H264_DVR_GetDownloadPos(long lFileHandle);

//����
H264_DVR_API long CALL_METHOD H264_DVR_Upgrade(long lLoginID, char *sFileName, int nType = 0, fUpgradeCallBack cbUpgrade = NULL, long dwUser = 0);
//����״̬ 1 �ɹ��� 2 �������� 3 ʧ��
H264_DVR_API int CALL_METHOD H264_DVR_GetUpgradeState(long lUpgradeHandle);
H264_DVR_API long CALL_METHOD H264_DVR_CloseUpgradeHandle(long lUpgradeHandle);


//��̨���ƣ�Ĭ���ǿ�����̨���ٶ�4���û�Ҳ�������������ٶ�
H264_DVR_API bool CALL_METHOD H264_DVR_PTZControl(long lLoginID,int nChannelNo, long lPTZCommand, bool bStop = false, long lSpeed = 4);

//��չ��̨���ƣ�����Ԥ�õ����ã�Ѳ��·�ߣ����ٶ�λ�ȵ�
H264_DVR_API bool CALL_METHOD H264_DVR_PTZControlEx(long lLoginID,int nChannelNo, long lPTZCommand, 
									long lParam1, long lParam2, long lParam3, bool bStop = false);

//�豸���ƣ�����ֵ true �ɹ��� false ʧ��
H264_DVR_API bool CALL_METHOD H264_DVR_ControlDVR(long lLoginID, int type, int waittime = 2000);
	

//�����������ڵ��豸
H264_DVR_API bool CALL_METHOD H264_DVR_SearchDevice(char* szBuf, int nBufLen, int* pRetLen, int nSearchTime);


//�����Խ����������ݴ����ӿ�
H264_DVR_API long CALL_METHOD H264_DVR_StartVoiceCom_MR(long lLoginID, pfAudioDataCallBack pVcb, long dwDataUser);
H264_DVR_API bool CALL_METHOD H264_DVR_VoiceComSendData(long lVoiceHandle, char *pSendBuf, long lBufSize);
H264_DVR_API bool CALL_METHOD H264_DVR_StopVoiceCom(long lVoiceHandle);

//���öԽ���Ƶ���뷽ʽ���û����Բ����ã�Ĭ��ΪG711A����
H264_DVR_API bool CALL_METHOD H264_DVR_SetTalkMode(long lLoginID, SDK_AudioInFormatConfig* pTalkMode);

// lRecordType¼��ģʽ, ��SDK_RecordModeTypes
H264_DVR_API bool CALL_METHOD H264_DVR_StartDVRRecord(long lLoginID, int nChannelNo ,long lRecordType);
H264_DVR_API bool CALL_METHOD H264_DVR_StopDVRRecord(long lLoginID, int nChannelNo);

H264_DVR_API bool CALL_METHOD H264_DVR_SetSystemDateTime(long lLoginID, SDK_SYSTEM_TIME *pSysTime);
H264_DVR_API bool CALL_METHOD H264_DVR_GetDVRWorkState(long lLoginID, SDK_DVR_WORKSTATE *pWorkState);

H264_DVR_API bool CALL_METHOD H264_DVR_ClickKey(long lLoginID, SDK_NetKeyBoardData *pKeyBoardData);

// ���̹���
H264_DVR_API int CALL_METHOD H264_DVR_StorageManage(long lLoginID, SDK_StorageDeviceControl *pStorageCtl);

H264_DVR_API bool CALL_METHOD H264_DVR_SendNetAlarmMsg(long lLoginID, SDK_NetAlarmInfo *pAlarmInfo);

// ץͼ
H264_DVR_API bool CALL_METHOD H264_DVR_CatchPic(long lLoginID, int nChannel, char *sFileName);
H264_DVR_API bool CALL_METHOD H264_DVR_CatchPicInBuffer(long lLoginID, int nChannel, char *pBuffer, int nBufLen, int *pPicLen);

H264_DVR_API bool CALL_METHOD H264_DVR_StartAlarmCenterListen(int nPort, fMessCallBack cbAlarmCenter, unsigned long dwDataUser);
H264_DVR_API bool CALL_METHOD H264_DVR_StopAlarmCenterListen();

//͸��232,485
H264_DVR_API bool CALL_METHOD H264_DVR_SerialWrite(long lLoginID, SERIAL_TYPE nType, char *pBuffer, int nBufLen);
H264_DVR_API bool CALL_METHOD H264_DVR_SerialRead(long lLoginID, SERIAL_TYPE nType, char *pBuffer, int nBufLen, int *pReadLen);

H264_DVR_API int CALL_METHOD H264_DVR_GetDDNSInfo(SearchMode &searchmode, DDNS_INFO *pDevicInfo, int maxDeviceNum, int &nretNum);

//nStream: 0��ʾ��������Ϊ1��ʾ������
H264_DVR_API bool CALL_METHOD H264_DVR_MakeKeyFrame(long lLoginID, int nChannel, int nStream);

//nStream: 0��ʾ��������Ϊ1��ʾ������
H264_DVR_API bool CALL_METHOD H264_DVR_GetOEMInfo(long lLoginID, SDK_OEMInfo *pOEMInfo);
// ����͸������ͨ��
H264_DVR_API bool CALL_METHOD H264_DVR_OpenTransComChannel(long lLoginID, TransComChannel *TransInfo, fTransComCallBack cbTransCom, unsigned long lUser);
//�ر�͸������ͨ��
H264_DVR_API bool CALL_METHOD H264_DVR_CloseTransComChannel(long lLoginID, SERIAL_TYPE nType);
//��ȡ״̬��Ϣ
H264_DVR_API bool CALL_METHOD H264_DVR_GetDeviceState(long lLoginID, SDK_State_Type type, char *pState);
//��ȡuiͼƬ:type 1:ΪȫͼƬ 2:�����ͼƬ
H264_DVR_API bool CALL_METHOD H264_DVR_CatchPicUI(long lLoginID,char *saveFileName,int type=1);

#ifdef WIN32

//��ͨ����Ƶ //Ԥ�����طţ����ز��� һ������
H264_DVR_API bool CALL_METHOD H264_DVR_OpenSound(long lHandle);
//�ر�ͨ����Ƶ //Ԥ�����طţ����ز��� һ������
H264_DVR_API bool CALL_METHOD H264_DVR_CloseSound(long lHandle);
//ץ����ͼƬ //Ԥ�����طţ����ز��� һ������
H264_DVR_API bool CALL_METHOD H264_DVR_LocalCatchPic(long lHandle,char*szSaveFileName);
//�򿪶Խ�
H264_DVR_API long CALL_METHOD H264_DVR_StartLocalVoiceCom(long lLoginID);
//��ʼ����¼��
H264_DVR_API bool CALL_METHOD H264_DVR_StartLocalRecord(long lRealHandle,char*szSaveFileName,long type=0);
//�رձ���¼��
H264_DVR_API bool CALL_METHOD H264_DVR_StopLocalRecord(long lRealHandle);

//���ű����ļ�
H264_DVR_API long CALL_METHOD H264_DVR_StartLocalPlay(char*pFileName,void* hWnd,fPlayDrawCallBack drawCallBack=NULL,long user=NULL);
//�رձ��ز���
H264_DVR_API bool CALL_METHOD H264_DVR_StopLocalPlay(long lPlayHandle);
//��ȡ����λ�����ڻطţ��ͱ��ز���
H264_DVR_API float CALL_METHOD H264_DVR_GetPlayPos(long lPlayHandle);
//���ò���λ�ã��ٷֱȣ����ڻطţ��ͱ��ز���
H264_DVR_API bool CALL_METHOD H264_DVR_SetPlayPos(long lPlayHandle,float fRelativPos);
//���ſ��ƣ����ţ�ֹͣ���ָ����췢�����ţ�
H264_DVR_API bool CALL_METHOD H264_DVR_LocalPlayCtrl(long lPlayHandle,SDK_LoalPlayAction action,long lCtrlValue);
//���ò��Ž����ص�
H264_DVR_API bool CALL_METHOD H264_DVR_SetFileEndCallBack(long lPlayHandle,fLocalPlayFileCallBack callBack,long user);
//������Ϣ֡�ص�
H264_DVR_API bool CALL_METHOD H264_DVR_SetInfoFrameCallBack(long lPlayHandle,InfoFramCallBack callback,long user);

//������ɫ���� //Ԥ�����طţ����ز��� һ������
H264_DVR_API bool CALL_METHOD H264_DVR_LocalGetColor(long lHandle, DWORD nRegionNum, LONG *pBrightness, LONG *pContrast, LONG *pSaturation, LONG *pHue);
H264_DVR_API bool CALL_METHOD H264_DVR_LocalSetColor(long lHandle, DWORD nRegionNum, LONG nBrightness, LONG nContrast, LONG nSaturation, LONG nHue);
#endif
//���ñ���ip
H264_DVR_API bool CALL_METHOD H264_DVR_SetLocalBindAddress(char*szIP);

//�ϱ�����
H264_DVR_API bool CALL_METHOD H264_DVR_StartUploadData(long lLoginID,UploadDataType upLoadType,fUploadDataCallBack callBack,long lUser);

H264_DVR_API bool CALL_METHOD H264_DVR_StopUploadData(long lLoginID,UploadDataType upLoadType);

//����ע��
H264_DVR_API bool CALL_METHOD H264_DVR_StartActiveRigister(int nPort, fMessCallBack cbFunc, unsigned long dwDataUser);
H264_DVR_API bool CALL_METHOD H264_DVR_StopActiveRigister();

//�������쳣�Ͽ�
H264_DVR_API long CALL_METHOD H264_DVR_SetSubDisconnectCallBack(fSubDisConnectCallBack callBack,DWORD userData);

//���ñ���ʱ��
H264_DVR_API long CALL_METHOD H264_DVR_SetKeepLifeTime(long lLoginID,unsigned int perKeeplifeTime,unsigned int detectDisconTime);

//MAC �õ�����
H264_DVR_API bool CALL_METHOD H264_DVR_CheckEncrypt(char *pMac);

//�豸���������豸�����豸��ͬһ�����������豸
H264_DVR_API bool CALL_METHOD H264_DVR_SearchDeviceEX(long lLoginID,SDK_NetDevList *pDevlist,SDK_TransferProtocol_V2 transferProtocol = SDK_TRANSFER_PROTOCOL_NETIP,int waittime = 15000);

#endif