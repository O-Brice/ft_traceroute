#include "ft_traceroute.h"

static double calcul_rtt(struct timeval *start, struct timeval *end)
{
    double s = (double)(end->tv_sec - start->tv_sec) * 1000.0;
    double us = (double)(end->tv_usec - start->tv_usec) / 1000.0;
    return (s + us);
}

static int parse_icmp_response(char *buffer, ssize_t bytes, uint16_t expected_port, int *target_reached)
{
    struct iphdr *ip = (struct iphdr *)buffer;
    int ip_len = ip->ihl * 4;

    if (bytes < ip_len + (ssize_t)sizeof(struct icmphdr))
        return (0);

    struct icmphdr *icmp = (struct icmphdr *)(buffer + ip_len);
    struct iphdr *orig_ip = (struct iphdr *)(buffer + ip_len + sizeof(struct icmphdr));
    int orig_ip_len = orig_ip->ihl * 4;
    
    struct udphdr *orig_udp = (struct udphdr *)(buffer + ip_len + sizeof(struct icmphdr) + orig_ip_len);

    if (ntohs(orig_udp->dest) != expected_port)
        return (0);

    if (icmp->type == ICMP_DEST_UNREACH)
    {
        *target_reached = 1;
        return (1);
    }
    else if (icmp->type == ICMP_TIME_EXCEEDED)
    {
        return (1);
    }

    return (0);
}

void send_probe(t_traceroute *data, struct timeval *send_time)
{
    char payload[32];

    memset(payload, 42, sizeof(payload));
    data->target_sa.sin_port = htons(data->udp_port);

    if (gettimeofday(send_time, NULL) < 0)
    {
        perror("ft_traceroute: gettimeofday failed");
        exit(EXIT_FAILURE);
    }

    if (sendto(data->udp_sockfd, payload, sizeof(payload), 0, 
               (struct sockaddr *)&data->target_sa, sizeof(data->target_sa)) < 0)
    {
        perror("ft_traceroute: sendto failed");
    }
    data->udp_port++;
}

int receive_probe(t_traceroute *data, struct timeval *send_time, char *last_ip, uint16_t expected_port, int *target_reached)
{
    fd_set          rfds;
    struct timeval  tv;
    struct timeval  recv_time;
    struct sockaddr_in recv_sa;
    socklen_t       addr_len = sizeof(recv_sa);
    char            buffer[512];
    char            current_ip[INET_ADDRSTRLEN];
    
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(data->icmp_sockfd, &rfds);

        tv.tv_sec = data->wait_time;
        tv.tv_usec = 0;

        int ret = select(data->icmp_sockfd + 1, &rfds, NULL, NULL, &tv);
        if (ret < 0)
        {
            perror("ft_traceroute: select failed");
            return (-1);
        }
        else if (ret == 0)
        {
            printf(" *");
            return (0);
        }

        ssize_t bytes_received = recvfrom(data->icmp_sockfd, buffer, sizeof(buffer), 
                                          0, (struct sockaddr *)&recv_sa, &addr_len);
        if (bytes_received < 0)
        {
            perror("ft_traceroute: recvfrom failed");
            return (-1);
        }

        gettimeofday(&recv_time, NULL);

        if (parse_icmp_response(buffer, bytes_received, expected_port, target_reached))
        {
            char *ip_temp = inet_ntoa(recv_sa.sin_addr);
            if (ip_temp)
            {
                strncpy(current_ip, ip_temp, sizeof(current_ip));
                current_ip[sizeof(current_ip) - 1] = '\0';
            }
            else
            {
                strcpy(current_ip, "unknown");
            }

            if (strcmp(last_ip, current_ip) != 0)
            {
                printf(" %s", current_ip);
                strncpy(last_ip, current_ip, INET_ADDRSTRLEN);
                last_ip[INET_ADDRSTRLEN - 1] = '\0';
            }
            double rtt = calcul_rtt(send_time, &recv_time);
            printf("  %.3f ms", rtt);
            return (1);
        }
    }
}

void run_traceroute(t_traceroute *data)
{
    struct timeval send_times[data->nqueries > 0 ? data->nqueries : 1];
    int target_reached = 0;

    data->udp_port = data->base_port;

    for (data->curttl = data->first_ttl; data->curttl <= data->max_ttl && !target_reached; data->curttl++)
    {
        if (setsockopt(data->udp_sockfd, IPPROTO_IP, IP_TTL, &data->curttl, sizeof(data->curttl)) < 0)
        {
            perror ("ft_traceroute: setsockopt IP_TTL failed");
            exit(EXIT_FAILURE);
        }
        printf("%2d", data->curttl);
        fflush(stdout);

        char last_ip[INET_ADDRSTRLEN] = "";

        for(int i = 0; i < data->nqueries && !target_reached; i++)
        {
            uint16_t current_probe_port = data->udp_port;

            send_probe(data, &send_times[i]);
            receive_probe(data, &send_times[i], last_ip, current_probe_port, &target_reached);
            fflush(stdout);
        }
        printf("\n");
    } 
}