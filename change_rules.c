#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>

#define PID_FILE "/var/run/vlantaggerd.pid"
#define PID_MAX 10

void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
    	syslog (LOG_NOTICE, "Vlan_tagger catch signal w/ id %d", signum);
    }
}

//запускать с sudo, а то не сработает

int main(int argc, char const *argv[])
{
	openlog ("vlan_tagger", LOG_PID, LOG_DAEMON);

	char line[PID_MAX];
	FILE *pid_file;

    if ((pid_file = fopen(PID_FILE, "r")) == NULL)
    {
        perror("Error occured while opening PID_FILE file");
        return 1;
    }

    while ((fgets(line, sizeof(line), pid_file)) != NULL);

	pid_t pid = strtoul(line, NULL, PID_MAX);
	signal(SIGUSR1, my_handler); //зайти в функцию my_handler, можно удалить
	kill(pid, SIGUSR1);

	if (fclose (pid_file) == EOF)
	{
		perror("close pid_file error");
		exit(EXIT_FAILURE);
	}
	//либо использовать системный вызов, это больше расходов
	//system("pkill -10 vlantaggerd");
	return 0;
}