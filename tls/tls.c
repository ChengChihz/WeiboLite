#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"

int main(int argc, char **argv) {
 mbedtls_ssl_context ssl; mbedtls_ssl_config conf; mbedtls_net_context net;
 mbedtls_x509_crt ca; mbedtls_entropy_context entropy; mbedtls_ctr_drbg_context rng;
 unsigned char *request=NULL, buf[16384]; size_t used=0, off=0, total=0; int rc=0, result=1;
 const size_t limit=8*1024*1024; const char *stage="input"; fprintf(stderr,"tls_stage=input\n");
 if(argc!=4) { fprintf(stderr,"usage: weibomm-tls host port ca-file\n"); return 2; }
 signal(SIGPIPE,SIG_IGN); alarm(45);
 mbedtls_ssl_init(&ssl); mbedtls_ssl_config_init(&conf); mbedtls_net_init(&net);
 mbedtls_x509_crt_init(&ca); mbedtls_entropy_init(&entropy); mbedtls_ctr_drbg_init(&rng);
 request=malloc(limit); if(!request) goto done;
 while(!feof(stdin)) { size_t n=fread(request+used,1,limit-used,stdin); used+=n;
 if(ferror(stdin) || used==limit) goto done; }
 stage="random"; fprintf(stderr,"tls_stage=random\n");
 rc=mbedtls_ctr_drbg_seed(&rng,mbedtls_entropy_func,&entropy,(const unsigned char*)"weibomm-n9",10); if(rc) goto done;
 stage="ca"; fprintf(stderr,"tls_stage=ca\n"); rc=mbedtls_x509_crt_parse_file(&ca,argv[3]); if(rc) goto done;
 stage="config"; fprintf(stderr,"tls_stage=config\n"); rc=mbedtls_ssl_config_defaults(&conf,MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT); if(rc) goto done;
 mbedtls_ssl_conf_min_tls_version(&conf,MBEDTLS_SSL_VERSION_TLS1_2);
 mbedtls_ssl_conf_max_tls_version(&conf,MBEDTLS_SSL_VERSION_TLS1_2);
 mbedtls_ssl_conf_authmode(&conf,MBEDTLS_SSL_VERIFY_REQUIRED); mbedtls_ssl_conf_ca_chain(&conf,&ca,NULL);
 mbedtls_ssl_conf_rng(&conf,mbedtls_ctr_drbg_random,&rng); mbedtls_ssl_conf_read_timeout(&conf,15000);
 rc=mbedtls_ssl_setup(&ssl,&conf); if(rc) goto done;
 stage="hostname"; fprintf(stderr,"tls_stage=hostname\n"); rc=mbedtls_ssl_set_hostname(&ssl,argv[1]); if(rc) goto done;
 stage="connect"; fprintf(stderr,"tls_stage=connect\n"); rc=mbedtls_net_connect(&net,argv[1],argv[2],MBEDTLS_NET_PROTO_TCP); if(rc) goto done;
 mbedtls_ssl_set_bio(&ssl,&net,mbedtls_net_send,mbedtls_net_recv,mbedtls_net_recv_timeout);
 stage="handshake"; fprintf(stderr,"tls_stage=handshake\n"); do {rc=mbedtls_ssl_handshake(&ssl);}while(rc==MBEDTLS_ERR_SSL_WANT_READ || rc==MBEDTLS_ERR_SSL_WANT_WRITE); if(rc) goto done;
 if(mbedtls_ssl_get_verify_result(&ssl)!=0) {rc=-1;goto done;}
 fprintf(stderr,"tls=1.2 verified=1\n");
 stage="write"; fprintf(stderr,"tls_stage=write\n"); while(off<used) {rc=mbedtls_ssl_write(&ssl,request+off,used-off);if(rc==MBEDTLS_ERR_SSL_WANT_READ || rc==MBEDTLS_ERR_SSL_WANT_WRITE)continue; if(rc<=0)goto done;off+=rc;}
 stage="read"; fprintf(stderr,"tls_stage=read\n"); for(;;) {rc=mbedtls_ssl_read(&ssl,buf,sizeof(buf));
 if(rc==MBEDTLS_ERR_SSL_WANT_READ || rc==MBEDTLS_ERR_SSL_WANT_WRITE)continue;
 if(rc==0 || rc==MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY || rc==MBEDTLS_ERR_SSL_CONN_EOF) {result=0;break;}
 if(rc<0)goto done;total+=rc;if(total>32*1024*1024){rc=-2;goto done;}if(fwrite(buf,1,rc,stdout)!=(size_t)rc){rc=-3;goto done;}}
 done:
 if(result) fprintf(stderr,"tls_error stage=%s code=%d\n",stage,rc);
 if(request){memset(request,0,used);free(request);}mbedtls_ssl_free(&ssl);mbedtls_ssl_config_free(&conf);
 mbedtls_net_free(&net);mbedtls_x509_crt_free(&ca);mbedtls_ctr_drbg_free(&rng);mbedtls_entropy_free(&entropy);return result;
}
