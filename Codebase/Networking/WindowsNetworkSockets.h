#include <winsock2.h>
#include <ws2tcpip.h>

/*
int WSAStartup(
  WORD version, // highest version of winsock
  WSADATA* data  // pointer to WSADATA struct
);

int WSACleanup(void);

int WSAGetLastError(void);
*/

struct addrinfo
{
	int ai_flags; // socket options
	int ai_family; // address family
	int ai_socktype; // socket type (SOCK_STREAM or SOCK_DGRAM)
	int ai_protocol; // protocol type
	size_t ai_addrlen; // ai_addr length
	char* ai_canonname;  // canonical name
	struct sockaddr* ai_addr;  // pointer to sockaddr struct
	struct addrinfo* ai_next;  // next addrinfo struct
};

int getaddrinfo(
  const char* name,  // host name or IP
  const char* service,  // service name or port
  const struct addrinfo* hints,  // socket info
  struct addrinfo* result  // result struct
);

SOCKET socket(
  int af, // address family
  int type, // socket type
  int protocol // protocol type
);

int bind(
  SOCKET s, // socket to bind to
  const struct sockaddr* name,  // local address info
  int namelen // length of * name
);

int connect(
  SOCKET s, // socket to connect to
  const struct sockaddr* name,  // remote address info
  int namelen // length of * name
);

//WSACleanup();