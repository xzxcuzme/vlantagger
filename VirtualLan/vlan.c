#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>
#include "filter.h"

#define SIZE_BUFF 1500
#define IFACE "/var/run/iface.txt"
#define PID_FILE "/var/run/vlantaggerd.pid"

static int running = 1;
static int ruls = 0;

struct vlan_header{
    short int TPID;
    short int TCI;
};

void error_exit(char err[])
{
    perror(err);
    exit(EXIT_FAILURE);
}

void pid_handler()
{
    FILE *pid_file;
    pid_t pid = getpid();

    pid_file = fopen(PID_FILE, "w");

    if (pid_file == NULL)
    {
        perror("Error occured while opening vlantaggerd.pid file");
        exit(EXIT_FAILURE);
    }

    fprintf(pid_file, "%d", pid);

    if (fclose (pid_file) == EOF)
    {
        perror("Error close while opening vlantaggerd.pid file");
        exit(EXIT_FAILURE);
    }
    return;
}

void signal_handler(int signum)
{
    syslog (LOG_NOTICE, "Vlan_tagger catch signal w/ id %d", signum);
    running = 0;
    syslog (LOG_NOTICE, "Vlan_tagger set running to %d", running);
    return;
}

void signal_rule(int signum)
{
    syslog (LOG_NOTICE, "Vlan_tagger catch signal w/ id %d", signum);
    ruls = 1;
    syslog (LOG_NOTICE, "Vlan_tagger update rules");
    return;
}

void iface_initialization(char *iface_in,
                          char *iface_out)
{
    FILE *file;

    if ((file = fopen(IFACE, "r")) == NULL)
    {
        error_exit("fopen");
    }

    for (int i = 0; i < 2; i++)
    {

        if (!i)
        {
            fscanf(file, "%s", iface_in);

        } else {
            fscanf(file, "%s", iface_out);

        }
    }
    fclose(file);
}

void vlan_tagger(char *buff_in,
                 char *buff_out,
                 uint16_t ID)
{
    struct vlan_header vlanhdr;
    struct iphdr ip; //не используется?
    uint16_t PCP = 0;
	uint16_t CFI = 0;

    for(int i = 0; i < 12; i++)
    {
        buff_out[i] = buff_in[i];
    }

    for(int i = (SIZE_BUFF - 4); i >= 12; i--)
    {
        buff_out[i + 4] = buff_in[i];
    }

    PCP = PCP << 13;
	CFI = CFI << 12;
    vlanhdr.TCI = htons(PCP|CFI|ID);
    vlanhdr.TPID = htons(0x8100);
    memcpy(buff_out + 12, &vlanhdr, sizeof(struct vlan_header));
}

void addr_initialization(char *iface,
                         struct sockaddr_ll *addr,
                         int size_addr)
{
    printf("%s\n", iface);
    memset((void*)addr, 0, size_addr);
    addr->sll_family     = AF_PACKET;
    addr->sll_protocol   = htons(ETH_P_ALL);
    addr->sll_ifindex    = if_nametoindex(iface);
}

int vlan_tegger()
{
    int sock, ip;
    int scale = 1;
    char x[6] = {0};
    uint16_t vlan;
    char buff_in[SIZE_BUFF] = {0};
    char buff_out[SIZE_BUFF] = {0};
    struct filter *filters;
    struct sockaddr_ll addr_in, addr_out;
    socklen_t size_addr = sizeof(struct sockaddr_ll);
    char iface_in[10] = {0};
    char iface_out[10] = {0};

    filters = malloc(sizeof(struct filter));
    iface_initialization(iface_in, iface_out);
    filters = struct_initialization(filters, &scale);

    sock = socket(AF_PACKET,SOCK_RAW, htons(ETH_P_ALL));

    if (sock == -1)
    {
        error_exit("socket");
    }

    addr_initialization(iface_in, &addr_in, size_addr);
    addr_initialization(iface_out, &addr_out, size_addr);

    //обрабатываем сигнал обновления правил и завершения работы с выходом из цикла
    sigset_t set;
    sigemptyset(&set);

    struct sigaction action;
    struct sigaction action_rules;

    memset(&action, 0, sizeof(action));
    memset(&action_rules, 0, sizeof(action_rules));

    action.sa_handler = signal_handler;
    action_rules.sa_handler = signal_rule;

    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGUSR1, &action_rules, NULL);

    while (running)
    {
        if (ruls)
        {
            filters = struct_initialization(filters, &scale);
            ruls = 0;
        }

        if (recvfrom(sock, buff_in, SIZE_BUFF, 0, (struct sockaddr *)&addr_in, &size_addr) == -1)
        {
            if (running == 1)
            {
                error_exit("recvfrom");
            } else {
                exit(EXIT_SUCCESS); //running == 0
            }
        }

        ip = *(int*)(buff_in + 26);
        ip = (ip << 24) | (ip >> 24) | ((ip >> 8) & 0x0000FF00) | ((ip << 8) & 0x00FF0000) ;
        sprintf(x, "%X", ip);
        vlan = filter_ip(filters, scale, x);

        if (vlan)
        {
            printf("%s\n", x);
            vlan_tagger(buff_in, buff_out, vlan);

            if ((sendto(sock, buff_out, SIZE_BUFF, 0, (struct sockaddr *)&addr_out, size_addr) == -1) && addr_out.sll_ifindex)
            {
                error_exit("sendto");
            } else {
                syslog(LOG_ERR,"Enter the output interface");
            }
        }
    }
    unlink(PID_FILE);
    exit(EXIT_SUCCESS);
}