#pragma once
#include "ring_buff.h"
#include "util.h"
#include <string>
#include <list>

namespace Server
{
    struct pss_live;
    enum MediaType;

    class CLiveWorker
    {
    public:
        CLiveWorker();
        ~CLiveWorker();

        bool Play();

        void push_flv_frame(char* pBuff, int nLen);
        int get_flv_frame(char **buff);   /** ����˻�ȡ��Ƶ���� */
		void next_flv_frame();

		void close();
    private:
        void cull_lagging_clients();

    public:
        pss_live             *m_pPss;           //< ���ӻỰ
        std::string           m_strUrl;         //< ����ý��url
        std::string           m_strType;        // Ŀ��ý������ flv mp4 h264
        std::string           m_strHw;          // Ŀ��ý��ֱ��� �ձ�ʾ����
        std::string           m_strMIME;        //< mime type
        bool                  m_bWebSocket;     //< false:http����true:websocket

        std::string           m_strPath;        //< ���Ŷ������ַ
        std::string           m_strClientName;  //< ���Ŷ˵�����
        std::string           m_strClientIP;    //< ���Ŷ˵�ip
        std::string           m_strError;       //< sip���������صĲ�������ʧ��ԭ��

    private:
        ring_buff_t          *m_pFlvRing;       //< Ŀ���������ݶ���
        std::string           m_SocketBuff;     //< socket���͵����ݻ���
		bool                  m_bConnect;       //< �ͻ�������״̬
    };

    /** ֱ�� */
    CLiveWorker* CreatLiveWorker(std::string strURL, std::string strType, std::string strHw, bool isWs, pss_live *pss, string clientIP);

    void InitFFmpeg();

};