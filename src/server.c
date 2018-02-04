#include "tcp_server.h"

/*

	OK, SO HERE'S THE PLAN:

4. Build the infrastructure EVEN MORE!!!!!
	- Change the way we serve data to the client to increase
	performance and efficiency.
		

*/


int main(int argc, char *argv[]) {


	SERVER_INFO *serv_info = NULL;
	int epoll_fd = 0, s = 0;		

	serv_info = parse_config_file();

	if (serv_info) {

		int all_fields_filled = (strcmp(serv_info->root_path, "") && strcmp(serv_info->addr, "") && serv_info->port > 0) ? 1 : 0;

		if (all_fields_filled) {

			uint32_t addr = convert_IP(AF_INET, serv_info->addr);
			
			if (addr >= 0) {

				s = create_tcp_server(addr, serv_info->port);
				epoll_fd = epoll_create1(0);
				
				if (s > 0 && epoll_fd > 0) {

					listen(s, 20000);
					log_info("Using root directory %s\n", serv_info->root_path);
					log_info("Listening on %s:%d\n", serv_info->addr, serv_info->port);
					main_loop(serv_info->root_path, epoll_fd, s, 20000);

					
					close(s);
					log_info("Terminated server\n");

					close(epoll_fd);
					log_info("Closed epoll-related file descriptor\n");

					free(serv_info);

				} else {

					if (s) close(s);

					if (epoll_fd) close(epoll_fd);
					else log_error(errno);

					free(serv_info);
					return -1;
				}

			} else {

				free(serv_info);
				return -1;
			}

		} else {

			free(serv_info);
			return -1;
		}

	} else {

		return -1;
	}

	return 0;

}
