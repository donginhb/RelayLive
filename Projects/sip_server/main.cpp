// sever.cpp : 定义控制台应用程序的入口点。
//
#include "util.h"
#include "utilc_api.h"
#include "MiniDump.h"
#include "ipc.h"
#include "SipServer.h"
#include "script.h"
#include <stdio.h>
#include <map>
#include <sstream>

std::map<string, SipServer::DevInfo*> g_mapDevs;
CriticalSection                       g_csDevs;
bool                                  _useScript = false;   //是否启用lua脚本

// 每天执行一次清理数据库操作
void on_clean_everyday(time_t t) {
    struct tm * timeinfo = localtime(&t);
    if(_useScript)
        Script::CleanDev(timeinfo->tm_hour);
}

// 查询目录得到设备信息应答
void on_device(SipServer::DevInfo* dev) {
	if(_useScript)
        Script::InsertDev(dev);

	MutexLock lock(&g_csDevs);
	if(g_mapDevs.count(dev->strDevID) == 0)
	    g_mapDevs.insert(make_pair(dev->strDevID, dev));
	else {
	    delete g_mapDevs[dev->strDevID];
	    g_mapDevs[dev->strDevID] = dev;
	}
}

// 更新设备在线状态
void on_update_status(string strDevID, string strStatus) {
    if(_useScript)
	    Script::UpdateStatus(strDevID, strStatus);

	MutexLock lock(&g_csDevs);
	auto fit = g_mapDevs.find(strDevID);
	if(fit != g_mapDevs.end()) {
		fit->second->strStatus = strStatus;
	}
}

// 更新设备gps
void on_update_postion(string strDevID, string log, string lat) {
    if(_useScript)
	    Script::UpdatePos(strDevID, lat, log);

	MutexLock lock(&g_csDevs);
	auto fit = g_mapDevs.find(strDevID);
	if(fit != g_mapDevs.end()) {
		fit->second->strLongitude = log;
		fit->second->strLatitude = lat;
	}
}

int main()
{
    /** Dump设置 */
    CMiniDump dump("sipServer.dmp");

    /** 创建日志文件 */
    char path[MAX_PATH];
    sprintf_s(path, MAX_PATH, ".\\log\\sipServer.txt");
    Log::open(Log::Print::both, Log::Level::debug, path);
    Log::debug("version: %s %s", __DATE__, __TIME__);

    /** 加载配置文件 */
    if (!Settings::loadFromProfile(".\\config.txt"))
        Log::error("Settings::loadFromProfile failed");
    else
        Log::debug("Settings::loadFromProfile ok");

    /** 进程间通信 */
    IPC::Init();

	/** 数据库脚本 */
    string use = Settings::getValue("Script", "use", "false");
    if(use == "yes" || use == "1")
        _useScript = true;
    if(_useScript)
	    Script::Init();

    /** 初始化SIP服务器 */
    if (!SipServer::Init())
    {
        Log::error("SipInstance init failed");
        return -1;
    }
    Log::debug("SipInstance::Init ok");
    
    sleep(INFINITE);
    return 0;
}

std::string GetDevsJson() {
	MutexLock lock(&g_csDevs);
	stringstream ss;
	ss << "{\"root\":[";
	bool first = true;
	for(auto c:g_mapDevs){  
		if(!first) {
			ss << ",";
		} else {
			first = false;
		}
		ss << FormatDevInfo(c.second, true);
	}
	ss << "]}";
	return ss.str();
}