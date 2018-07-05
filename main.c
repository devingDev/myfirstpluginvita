/*
 *  ShellBat plugin
 *  Copyright (c) 2017 David Rosca
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:

 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.

 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
#include "log.h"

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <psp2/sysmodule.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/power.h>
#include <taihen.h>

static SceUID threadOne;
int NET_PARAM_MEM_SIZE = 4 * 1024 * 1024;

static const char SERVER_IP[] = "192.168.178.176";
static const int SERVER_PORT = 9055;
static const int NETBUFSIZE = 1024*1024;

static int threadOneFunc(unsigned int args, void* argp)
{
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	SceNetInitParam netInitParam;
	netInitParam.memory = malloc(NET_PARAM_MEM_SIZE);
	memset(netInitParam.memory, 0, NET_PARAM_MEM_SIZE);
	netInitParam.size = NET_PARAM_MEM_SIZE;
	netInitParam.flags = 0;	
	sceNetInit(&netInitParam);
	sceNetCtlInit();
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
	
	return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
    LOG("Starting module");


    threadOne = sceKernelCreateThread("threadOne", &threadOneFunc, 0x40, 1024*1024*4, 0, 0, NULL);
    sceKernelStartThread(threadOne, 0, NULL);

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
    LOG("Stopping module");

    return SCE_KERNEL_STOP_SUCCESS;
}
