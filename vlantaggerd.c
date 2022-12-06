#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>
#include <stddef.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "VirtualLan/vlan.h"

#define PID_FILE "/var/run/vlantaggerd.pid"

static int running = 1;
FILE *rules;

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

int main(int argc, char const *argv[])
{
	pid_handler(); //создаем PID файл
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

	openlog ("vlan_tagger", LOG_PID, LOG_DAEMON);

	syslog (LOG_NOTICE, "Vlan_tagger started.");

	while (running)
	{
		syslog (LOG_NOTICE, "Vlan_tagger listen IP packet"); //можно писать в лог сам пакет
		//тут будет основная программа
		vlan_tegger();
		syslog (LOG_NOTICE, "Vlan_tagger tagged IP packet to VLAN"); //написать какой влан присвоили пакету
	}

	syslog (LOG_NOTICE, "Vlan_tagger terminated.");
	closelog();
	unlink(PID_FILE);
	exit(EXIT_SUCCESS);
}