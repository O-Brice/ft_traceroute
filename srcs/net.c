#include "ft_traceroute.h"

void print_usage(void)
{
    printf("Usage: ./ft_traceroute [OPTION...] HOST...\n");
    printf("Print the route packet trace to network host.\n\n");
    printf("--help              Give this help list\n");
    printf("-m Max_ttl          Set the max number of hops (max TTL to be reached). Default is 30\n");
    printf("-q nqueries         Set the number of probes per each hop. Default is 3\n");
    printf("-p port             Set the destination port to use. It is either initial udp port value\n for \"default\" method (incremented by each probe, default is 33434),\n or initial seq for \"icmp\" (incremented as well, default from 1),\nor some constant destination port for other methods (with default of 80 \nfor \"tcp\", 53 for \"udp\", etc.).\n");
    printf("-w wait             Wait for a probe no more than wait seconds (default 1)\n");
    printf("-f first_ttl        Start from the first_ttl hop (instead from 1)\n");
    exit(EXIT_SUCCESS);
}

void resolve_hostname(t_traceroute *data, const char *host)
{
    struct addrinfo hints, *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int ret = getaddrinfo(host, NULL, &hints, &result);
    if (ret != 0)
    {
        fprintf(stderr, "ft_traceroute: unknown host %s\n", host);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *sa = (struct sockaddr_in *)result->ai_addr;
    memcpy(&data->target_sa, sa, sizeof(struct sockaddr_in));
    
    char *ip_temp = inet_ntoa(sa->sin_addr);
    if (ip_temp)
    {
        strncpy(data->ip_str, ip_temp, INET_ADDRSTRLEN);
        data->ip_str[INET_ADDRSTRLEN - 1] = '\0';
    }

    freeaddrinfo(result);
}

void parse_args(t_traceroute *data, int argc, char **argv)
{
    if (getuid() != 0)
    {
        fprintf(stderr, "ft_traceroute: root privileges are required.\n");
        exit(EXIT_FAILURE);
    }
    if (argc < 2)
    {
        print_usage();
    }

    data->max_ttl = 30;
    data->nqueries = 3;
    data->base_port = 33434;
    data->wait_time = 1;
    data->first_ttl = 1;
    data->hostname = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            print_usage();
        }
        else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc)
        {
            data->max_ttl = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc)
        {
            data->nqueries = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
        {
            data->base_port = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc)
        {
            data->wait_time = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            data->first_ttl = atoi(argv[++i]);
        }
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "ft_traceroute: invalid option -- '%s'\n", argv[i] + 1);
            print_usage();
        }
        else
            if (data->hostname != NULL)

            {
                fprintf(stderr, "ft_traceroute: too many arguments\n");
                print_usage();
            }
                data->hostname = argv[i];
    }

    if (!data->hostname)
    {
        fprintf(stderr, "ft_traceroute: missing host operand\n");
        print_usage();
    }

    resolve_hostname(data, data->hostname);
}

void init_sockets(t_traceroute *data)
{
    data->udp_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (data->udp_sockfd < 0)
    {
        perror("ft_traceroute: UDP socket creation failed");
        exit(EXIT_FAILURE);
    }

    data->icmp_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (data->icmp_sockfd < 0)
    {
        perror("ft_traceroute: ICMP socket creation failed");
        close(data->udp_sockfd);
        exit(EXIT_FAILURE);
    }
}