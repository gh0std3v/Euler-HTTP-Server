#ifndef __tcp_server__
#define __tcp_server__

#include "done_signal/done.h"
#include "http_parsing.h"
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int make_nonblocking(int fd) {

	int ret = fcntl(fd, F_SETFL, O_NONBLOCK);

	if (ret) {


		log_error(errno);
		return -1;

	}

	return fd;
}

uint32_t convert_IP(const int type, const char *ip) {

	uint32_t new_ip = 0;
	struct sockaddr_in sa;

	bzero(&sa, sizeof(struct sockaddr_in));

	if (inet_pton(type, ip, (void *)&sa) < 0) {

			log_error(errno);
			return -1;

	}

	new_ip = (uint32_t) sa.sin_addr.s_addr;

	return new_ip;

}

struct sockaddr_in *set_sockaddr_flags(int type, uint32_t addr, int port) {

	struct sockaddr_in *sa = malloc(sizeof(struct sockaddr_in));

	if (sa) {

		bzero(sa, sizeof(struct sockaddr_in));

		sa->sin_family = type;
		sa->sin_addr.s_addr = htonl(addr);
		sa->sin_port = htons(port);

	} else {

		sa = NULL;
	}

	return sa;

}

int create_tcp_server(uint32_t addr, int port) {

	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	s = make_nonblocking(s);

	if (s < 0) {

		log_error(errno);
		return -1;

	}

	int optval = 1, size = sizeof(optval);

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, (socklen_t) size) == -1) {

		log_error(errno);
		close(s);
		return -1;
	}

	struct sockaddr_in *sa = set_sockaddr_flags(AF_INET, addr, port);

	if (!sa) {

		log_error(errno);
		close(s);
		return -1;
	}

	if (bind(s, (struct sockaddr *)sa, sizeof(struct sockaddr)) < 0) {

		log_error(errno);
		close(s);
		free(sa);
		return -1;
	}

	free(sa);

	return s;
}

void destroy_tcp_server(int sig) {

	set_done(1);

}

int handle_client(char *root, int c) {

	int len = strlen(SEND_HTTP_NOT_FOUND), len2 = strlen(SEND_HTTP_INTERNAL_SERVER_ERROR), bytes = 0;
	char buffer[BUFSIZ + 1], *data = NULL, *not_found_error = SEND_HTTP_NOT_FOUND, *internal_error = SEND_HTTP_INTERNAL_SERVER_ERROR;

	bzero(buffer, BUFSIZ + 1);

	while (1) {

		if ((bytes = read(c, buffer + bytes, BUFSIZ + 1)) < 0) {

			if (errno != EAGAIN && errno != EWOULDBLOCK) {

				log_error(errno);
				if (c) close(c);
				return -1;

			} else {

				int not_found = 0;

				data = parse_content(root, buffer, &not_found);

				if (!data) {

					log_warn("Couldn't get data (error code %d)\n", errno);

					if (not_found) {

						if (write(c, not_found_error, len) < 0) 
										log_error(errno);

					} else {

						if (write(c, internal_error, len2) < 0) 
										log_error(errno);
					}

					close(c);
					return -1;
				}

				break;
			}
		}
		
	}

	if (write(c, data, strlen(data)) < 0) {

		if (errno != EAGAIN && errno != EWOULDBLOCK) {

			log_error(errno);
			close(c);
			free(data);
			return -1;

		} 
	}

	close(c);
	free(data);

	return 0;

}

int main_loop(char *root_dir, int epoll_fd, int server, int backlog) {

	struct epoll_event ev, *events = calloc(backlog, sizeof(struct epoll_event));
	int ret = 0, done_val;

	if (!events) 
		return -1;

	bzero(&ev, sizeof(struct epoll_event));
	bzero(events, sizeof(struct epoll_event) * backlog);

	ev.data.fd = server;
	ev.events = EPOLLIN;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server, &ev) == -1) return -1;

	set_done(0);
	signal(SIGINT, destroy_tcp_server);

	while ((done_val = get_done()) == 0) {

		log_info("Waiting for clients...\n");

		int clients = epoll_wait(epoll_fd, events, backlog, -1), i = 0;

		if (clients == -1) {

			log_error(errno);
			ret = -1;
			break;
		}

		log_info("Got some clients\n");

		for (i = 0; i < clients; i++) {

			if (events[i].data.fd == server) {

				int client = accept(server, NULL, 0);
				client = make_nonblocking(client);

				if (client == -1) {

					log_error(errno);
					continue;

				}

				bzero(&ev, sizeof(struct epoll_event));
				ev.data.fd = client;
				ev.events = EPOLLIN | EPOLLET;

				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &ev)) 
								log_error(errno);

				log_info("Registered client with epoll_ctl\n");

			} else {

				handle_client(root_dir, events[i].data.fd);
			}
					
		}
	}

	free(events);
	log_info("Registered clients are freed.\n");

	return ret;

}

#endif
