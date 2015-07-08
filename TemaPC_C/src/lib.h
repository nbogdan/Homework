#ifndef LIB
#define LIB

#define HOST "127.0.0.1"
#define PORT1 10000
#define PORT2 10001

#define MSGSIZE		1400
#define PKGSIZE		1391
#define FRAMESIZE	1404
#define CORRECTORSIZE	11
#define PKGSIZE2	1380

#define TYPE_INIT 	1
#define TYPE_INFO	2
#define TYPE_END	3
#define TYPE_ACK	4
#define TYPE_NACK	5

#define MAX_VALUE 50000

typedef struct {
  int len;
  char payload[MSGSIZE];
} msg;

typedef struct {
  int packNum;
  int messageType;
  char checkSum;
  char payload[PKGSIZE];
} pkg;

typedef struct {
  char corrector[CORRECTORSIZE];	
  int packNum;
  int messageType;
  char payload[PKGSIZE2];
}  pkgbonus;
  
typedef struct cellista
{ msg info;
  struct cellista*urm;
} TCel, *TLista, **ALista; /* tipurile Celula, Lista si Adresa_Lista */


void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int send_message(const msg* m);
int recv_message(msg* r);
int recv_message_timeout(msg *m, int timeout);

#endif

