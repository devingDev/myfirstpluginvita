/*
    coderx3
*/

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/power.h>
#include <taihen.h>

#include "log.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static SceUID threadOne;
int NET_PARAM_MEM_SIZE = 4 * 1024 * 1024;

static const char SERVER_IP[] = "192.168.178.176";
static const int SERVER_PORT = 9055;
static const int NETBUFSIZE = 1024*1024;

static int _mysocket = 0;


int socketPrintf(char *text, ...)
{
	va_list list;
	char string[512];

	va_start(list, text);
	vsprintf(string, text, list);
	va_end(list);

	return sceNetSend(_mysocket, string, strlen(string), 0);
}

int init_mynetsocket(const char *ip,int port)
{
	LOG("DEFINE NETSIZE");
	#define NET_INIT_SIZE 1*1024*1024
	LOG("SCENETSOCKADDRIN");
	SceNetSockaddrIn server;
	LOG("SERVERSINLEN");
	server.sin_len = sizeof(server);
	LOG("SERVERSINFAMILY");
	server.sin_family = SCE_NET_AF_INET;
	
	LOG("SCENETINETPTON");
	sceNetInetPton(SCE_NET_AF_INET, ip, &server.sin_addr);
	LOG("SERVERSINPORT");
	server.sin_port = sceNetHtons(port);
	LOG("MEMSETSERVERSINZERO");
	memset(server.sin_zero, 0, sizeof(server.sin_zero));
	
	LOG("_MYSOCKET");
	_mysocket = sceNetSocket("myplugin", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
	LOG("_MYSOCKET : %X", _mysocket);
	LOG("SCENETCONNECT");
	int ret = sceNetConnect(_mysocket, (SceNetSockaddr *)&server, sizeof(server));
	LOG("RET : %X", ret);
	if(ret < 0)
		return ret;
	else
	{
		LOG("SOCKETPRINTF");
		socketPrintf("Hello from Vita");
	}
	return 0;
}

static int threadOneFunc(unsigned int args, void* argp)
{
	LOG("LOADING NET MODULE");
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	LOG("SCENETINITPARAM");
	SceNetInitParam netInitParam;
	LOG("MEMORY SET");
	netInitParam.memory = malloc(NET_PARAM_MEM_SIZE);
	LOG("MEMSET");
	memset(netInitParam.memory, 0, NET_PARAM_MEM_SIZE);
	LOG("PARAMSIZE");
	netInitParam.size = NET_PARAM_MEM_SIZE;
	LOG("FLAGS");
	netInitParam.flags = 0;	
	LOG("SCENETINIT");
	sceNetInit(&netInitParam);
	LOG("SCENETCTLINIT");
	sceNetCtlInit();
/*	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) return 0;
	struct hostent *server = gethostbyname(SERVER_IP);
	if(server == NULL){
		return 0;
	}
	struct sockaddr_in serveraddr;
	bzero((char*)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = SERVER_PORT;
	socklen_t serverlen = sizeof(serveraddr);
	char buf[NETBUFSIZE];
	bzero(buf, NETBUFSIZE);
	strcpy(buf, "Hello from my first plugin!");
	int ret = send(sockfd, buf, strlen(buf), 0);
	bzero(buf, NETBUFSIZE);
	ret = recv(sockfd, buf, NETBUFSIZE, 0);

	close(sockfd);
*/
	LOG("INIT_MYNETSOCKET");
	init_mynetsocket(SERVER_IP , SERVER_PORT);
	
	LOG("SCESYSMODULEUNLOAD NET");
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
	
	return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	LOG("STARTED")
	LOG("CREATING THREAD");
    threadOne = sceKernelCreateThread("threadOne", &threadOneFunc, 0x40, 1024*1024*4, 0, 0, NULL);
	LOG("CREATED THREAD");
	LOG("STARTING THREAD");
    threadOne = sceKernelStartThread(threadOne, 0, NULL);
	LOG("STARTED THREAD");

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{

	LOG("ENDED")
	LOG(" ");
    return SCE_KERNEL_STOP_SUCCESS;
}

