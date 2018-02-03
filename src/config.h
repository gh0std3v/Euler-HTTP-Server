#ifndef __config_h__
#define __config_h__

#include "debug.h"
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 1000
#define MAX_PARAM_LEN 100
#define MAX_IP_ADDR_LEN 300

typedef struct basic_information {

	char root_path[MAX_PATH_LEN];
	char addr[MAX_IP_ADDR_LEN];
	unsigned int port : 16;

} SERVER_INFO;

char *read_file(const char *file_path, int *size) {

	FILE *fp = fopen(file_path, "r");
	char *buffer = NULL;
	int to_read = 0;

	if (fp) {

		fseek(fp, 0, SEEK_END);
		to_read = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if (size)
			*size = to_read;

		buffer = malloc(to_read);

		if (buffer) {

			bzero(buffer, to_read);

			if (fread(buffer, 1, to_read, fp) == -1) {

				log_error(errno);
				if (buffer) free(buffer);
				buffer = NULL;
			}

		} else {

			log_error(errno);
			buffer = NULL;
		}


		fclose(fp);

	} else {

		log_debug("%s\n", file_path);
		log_error(errno);
  		if (buffer) free(buffer);
  		buffer = NULL;
	}

	 return buffer;

}

char **parse_config_lines(char *config_data) {

	int i = 0;
	char *line = NULL, *ptr = NULL, **lines = calloc(BUFSIZ, BUFSIZ + 1);

	if (lines) {

		line = strtok_r(config_data, "\n", &ptr);

		while (line) {

			if (strcmp(line, "") && line[0] != '#') {

				lines[i++] = line;
				line = strtok_r(NULL, "\n", &ptr);

			} else {

				line = strtok_r(NULL, "\n", &ptr);
			}
		}

		lines = realloc(lines, i);

	} else {

		log_error(errno);
		lines = NULL;
	}

	return lines;
}

char **split_parameters(char *line) {

	char *value = NULL, *ptr = NULL, **parameters = calloc(2, MAX_PARAM_LEN);
	int i = 0;

	if (!parameters) {

		log_error(errno);
		return NULL;
	}

	while (i < 2) {

		if (i) value = strtok_r(NULL, "= ", &ptr);
		else value = strtok_r(line, "= ", &ptr);

		parameters[i++] = value;
	}

	return parameters;

}

SERVER_INFO *parse_config_file() {

	SERVER_INFO *si = malloc(sizeof(SERVER_INFO));

	if (si) {

		char *config_file_data = NULL, **config_lines = NULL;
		bzero(si, sizeof(SERVER_INFO));

		config_file_data = read_file("server.conf", NULL);

		if (config_file_data) {

			config_lines = parse_config_lines(config_file_data);

			if (config_lines) {

				int i = 0;

				for (i = 0; config_lines[i] != NULL && i < BUFSIZ; i++) {

					char **parameters = split_parameters(config_lines[i]);

					if (parameters) {

						if (!strcmp(parameters[0], "root")) {

							if (strlen(parameters[1]) < MAX_PATH_LEN)
								strncpy(si->root_path, parameters[1], strlen(parameters[1]));
							else
								log_warn("Your root directory path exceeds 1000 characters. Move it to a different directory.\n");

						} else if (!strcmp(parameters[0], "addr")) {

							if (strlen(parameters[1]) < MAX_PATH_LEN)
								strncpy(si->addr, parameters[1], strlen(parameters[1]));
							else
								log_warn("Your IP address exceeds 300 characters. Check that you typed it correctly.\n");

						} else if (!strcmp(parameters[0], "port")) {

							if ((si->port = strtol(parameters[1], NULL, 10)) == -1) log_error(errno);
						}

						free(parameters);

					}
				}

				free(config_lines);
				free(config_file_data);

			} else {

				log_error(errno);
				free(config_file_data);
				free(si);
				si = NULL;
			}

		} else {

			log_error(errno);
			free(si);
			si = NULL;
		}

	} else {

		log_error(errno);
		si = NULL;
	}

	return si;

}


#endif
