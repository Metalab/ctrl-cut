/*
 * printer_connection.c
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "printer_connection.h"

static char buf[102400];

/**
 * Connect to a printer.
 *
 * @param host The hostname or IP address of the printer to connect to.
 * @param timeout The number of seconds to wait before timing out on the
 * connect operation.
 * @return A socket descriptor to the printer.
 */
extern int
printer_connect(const char *host, const int timeout)
{
    int socket_descriptor = -1;
    int i;

    for (i = 0; i < timeout; i++) {
        struct addrinfo *res;
        struct addrinfo *addr;
        struct addrinfo base = { 0, PF_UNSPEC, SOCK_STREAM };
        int error_code = getaddrinfo(host, "printer", &base, &res);

        /* Set an alarm to go off if the program has gone out to lunch. */
        alarm(SECONDS_PER_MIN);

        /* If getaddrinfo did not return an error code then we attempt to
         * connect to the printer and establish a socket.
         */
        if (!error_code) {
            for (addr = res; addr; addr = addr->ai_next) {
                socket_descriptor = socket(addr->ai_family, addr->ai_socktype,
                                           addr->ai_protocol);
                if (socket_descriptor >= 0) {
                    if (!connect(socket_descriptor, addr->ai_addr,
                                 addr->ai_addrlen)) {
                        break;
                    } else {
                        close(socket_descriptor);
                        socket_descriptor = -1;
                    }
                }
            }
            freeaddrinfo(res);
        }
        if (socket_descriptor >= 0) {
            break;
        }

        /* Sleep for a second then try again. */
        sleep(1);
    }
    if (i >= timeout) {
        fprintf(stderr, "Cannot connect to %s\n", host);
        return -1;
    }
    /* Disable the timeout alarm. */
    alarm(0);
    /* Return the newly opened socket descriptor */
    return socket_descriptor;
}

/**
 * Disconnect from a printer.
 *
 * @param socket_descriptor the descriptor of the printer that is to be
 * disconnected from.
 * @return True if the printer connection was successfully closed, false otherwise.
 */
extern bool
printer_disconnect(int socket_descriptor)
{
    int error_code;
    error_code = close(socket_descriptor);
    /* Report on possible errors to standard error. */
    if (error_code == -1) {
        switch (errno) {
        case EBADF:
            perror("Socket descriptor given was not valid.");
            break;
        case EINTR:
            perror("Closing socket descriptor was interrupted by a signal.");
            break;
        case EIO:
            perror("I/O error occurred during closing of socket descriptor.");
            break;
        }
    }

    /* Return status of disconnect operation to the calling function. */
    if (error_code) {
        return false;
    } else {
        return true;
    }
}

/**
 *
 */
extern bool
printer_send(const char *host, printer_job pjob)
{
    char localhost[HOSTNAME_NCHARS] = "";
    unsigned char lpdres;
    int socket_descriptor = -1;

    gethostname(localhost, sizeof(localhost));
    {
        char *d = strchr(localhost, '.');
        if (d) {
            *d = 0;
        }
    }

    /* Connect to the printer. */
    socket_descriptor = printer_connect(host, PRINTER_MAX_WAIT);

    // talk to printer
    sprintf(buf, "\002%s\n", pjob.queue);
    write(socket_descriptor, (char *)buf, strlen(buf));
    read(socket_descriptor, &lpdres, 1);
    if (lpdres) {
        fprintf (stderr, "Bad response from %s, %u\n", host, lpdres);
        return false;
    }
    sprintf(buf, "H%s\n", localhost);
    sprintf(buf + strlen(buf) + 1, "P%s\n", pjob.user);
    sprintf(buf + strlen(buf) + 1, "J%s\n", pjob.title);
    sprintf(buf + strlen(buf) + 1, "ldfA%s%s\n", pjob.name, localhost);
    sprintf(buf + strlen(buf) + 1, "UdfA%s%s\n", pjob.name, localhost);
    sprintf(buf + strlen(buf) + 1, "N%s\n", pjob.title);
    sprintf(buf + strlen(buf) + 1, "\002%d cfA%s%s\n", (int)strlen(buf), pjob.name, localhost);
    write(socket_descriptor, buf + strlen(buf) + 1, strlen(buf + strlen(buf) + 1));
    read(socket_descriptor, &lpdres, 1);
    if (lpdres) {
        fprintf(stderr, "Bad response from %s, %u\n", host, lpdres);
        return false;
    }
    write(socket_descriptor, (char *)buf, strlen(buf) + 1);
    read(socket_descriptor, &lpdres, 1);
    if (lpdres) {
        fprintf(stderr, "Bad response from %s, %u\n", host, lpdres);
        return false;
    }
    {
        {
            struct stat file_stat;
            if (fstat(fileno(pjob.pjl_file), &file_stat)) {
                perror(buf);
                return false;
            }
            sprintf((char *) buf, "\003%u dfA%s%s\n", (int) file_stat.st_size, pjob.name, localhost);
        }
        write(socket_descriptor, (char *)buf, strlen(buf));
        read(socket_descriptor, &lpdres, 1);
        if (lpdres) {
            fprintf(stderr, "Bad response from %s, %u\n", host, lpdres);
            return false;
        }
        {
            int l;
            while ((l = fread((char *)buf, 1, sizeof (buf), pjob.pjl_file)) > 0) {
                write(socket_descriptor, (char *)buf, l);
            }
        }
    }
    // dont wait for a response...
    printer_disconnect(socket_descriptor);
    return true;
}
