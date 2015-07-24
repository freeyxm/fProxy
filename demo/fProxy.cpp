//============================================================================
// Name        : fProxy.cpp
// Author      : freeyxm
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

#include "util/FServer.h"
#include "util/FServerTask.h"
#include "fcore/FUtil.h"
#include "fprotocol/FPSocks5.h"
#include "fprotocol/FPSocks52.h"
#include "fprotocol/FPHttp.h"
using namespace freeyxm;

void serv_process(FSocketTcp *socket);
void serv_process2(FSocketTcp *socket);
void serv_process_socket5(FSocketTcp *socket);
void serv_process_socket52(FSocketTcp *socket);
void serv_process_http(FSocketTcp *socket);

int main(void)
{

	setvbuf(stdout, (char*) NULL, _IOLBF, 0);

#if __win32__
	static WSADATA wsa_data;
	if (WSAStartup((WORD) (1 << 8 | 1), &wsa_data) != 0)
	{
		printf("WSAStartup error!\n");
		return -1;
	}
#endif

	FServer server("", 2012, 100);
	server.setProcessFun(serv_process_socket52);
	int ret = server.run();
	printf("ret = %d, done!\n", ret);

	return EXIT_SUCCESS;
}

void serv_process(FSocketTcp *socket)
{
	//DEBUG_PRINT("in serv_process ...\n");
	char buf[1024];
	int ret = socket->recv(buf, 1024);
	//DEBUG_PRINT("[%d] recv: %s\n", ret, buf);

	sprintf(buf, "hello, my socket handle is %d.", socket->getHandle());
	ret = socket->send(buf, strlen(buf));
	//DEBUG_PRINT("send ret: %d\n", ret);

	socket->close();
	delete socket;
}

void serv_process2(FSocketTcp *socket)
{
	DEBUG_MPRINT("in serv_process ...\n");
	char buf[1024];
	int ret = socket->recv(buf, 1024);
	DEBUG_MPRINT("recv ret: %d\n", ret);
	char *p = buf;

	for (int i = 0; i < 20; ++i)
	{
		printf(" %02x", (int) *(p + i));
	}
	printf("\n");

	printf("sizeof short = %ld\n", sizeof(short));

	DEBUG_MPRINT("vn: %d\n", (int) *p);DEBUG_MPRINT("cd: %d\n", (int) *(p + 1));DEBUG_MPRINT("port: %d\n", *(short*) (p + 2));DEBUG_MPRINT(
			"port: %02x%02x\n", (int) *(p + 2), (int) *(p + 3));

	//sprintf(buf, "hello, my socket handle is %d.\0", socket->getSocketHandle());
	//ret = socket->send(buf, strlen(buf));
	//DEBUG_PRINT("send ret: %d\n", ret);

	socket->close();
	delete socket;
}

void serv_process_socket5(FSocketTcp *socket)
{
	FP_Socks5 s5;
	s5.process(socket);
	delete socket;
}

void serv_process_socket52(FSocketTcp *socket)
{
	socks5::FP_Socks5_2 s5(socket);
	s5.run();
}

void serv_process_http(FSocketTcp *socket)
{
	FPHttp http;
	http.process(socket);
	delete socket;
}
