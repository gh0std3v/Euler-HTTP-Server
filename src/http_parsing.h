#ifndef __http_parser__
#define __http_parser__

#include "config.h"

#define HTTP_OK_TOKEN "HTTP/1.1 200 OK\r\nServer: Euler 1.0\r\nConnection: close\r\n\r\n"
#define SEND_HTTP_NOT_FOUND "HTTP/1.1 404 Not Found\r\nServer: Euler 1.0\r\nConnection: close\r\n\r\n<h1 align='center'>That resource cannot be found</h1>"
#define SEND_HTTP_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\nServer: Euler 1.0\r\nConnection: close\r\n\r\n<h1 align'='center'>Your request is invalid!</h1>"
#define SEND_HTTP_INTERNAL_SERVER_ERROR "HTTP/1.1 500 Internal Server Error\r\nServer: Euler 1.0\r\nConnection: close\r\n\r\n<h1 align='center'>We couldn't process your request...</h1>"

int count_lines(char *buffer) {

	char *ptr = NULL, *line = NULL;
	int size = 0;

	line = strtok_r(buffer, "\n", &ptr);
	if (line) ++size;

	while (line) {

		line = strtok_r(NULL, "\n", &ptr);
		++size;
	}

	return size;

}


char **parse_header_lines(char *buffer) {

	int line_count = count_lines(buffer);
	char **lines = calloc(line_count, BUFSIZ + 1);

	if (lines) {

		char *ptr = NULL, *line = NULL;	
		int i = 0;
		line = strtok_r(buffer, "\n", &ptr);

		while (line) {	

			if (!strcmp(line, "\r")) 
					break;

			lines[i] = line;
			++i;
			line = strtok_r(NULL, "\n", &ptr);
		}

	} else {

		log_error(errno);
		lines = NULL;
	}

	return lines;

}

char **split_key_value(char *line) {

	char **array = calloc(2, BUFSIZ + 1);

	if (array) {

		bzero(array, (BUFSIZ + 1) * 2);
		int i = 0;
		char *ptr = NULL;

		array[0] = strtok_r(line, ":", &ptr);
	
		if (array[0]) {

			i = strlen(array[0]);
			array[1] = line + i + 1;	
		} 

	} else {

		log_error(errno);
		array = NULL;
	}

	return array;

}

char *cat_headers_to_data(char *data, int size) {

	int size2 = size + 59;
	char *new_data = malloc(size2);

	if (new_data) {

		bzero(new_data, size2);
		strncpy(new_data, HTTP_OK_TOKEN, 59);
		strncat(new_data, data, strlen(data));

	} else {

		new_data = NULL;

	}

	return new_data;

}

char *parse_request(char *root, char *request) {

	int size = BUFSIZ + 1;
	char *file_path = malloc(size);

	if (file_path) {

		bzero(file_path, size);
		if ((strlen(root) + 1) < size)  {

			strncpy(file_path, root, strlen(root));
			strncat(file_path, "/", 1);

		} else {

			free(file_path);
			return NULL;
		}

		char path[MAX_PATH_LEN];
		int i = 0;

		bzero(path, MAX_PATH_LEN);
		for (i = 5; request[i] != ' ' && i < MAX_PATH_LEN && i < strlen(request); i++) 
					strncat(path, &request[i], 1);

		if (!strcmp(path, ""))
			strncat(path, "/index.html", 11);

		strncat(file_path, path, strlen(path));

		file_path = realloc(file_path, strlen(file_path) + 1);

	} else {

		file_path = NULL;
	}

	return file_path;

}

char *parse_content(char *root, char *buffer, int *is_not_found) {

	char **headers = NULL, *content = NULL, *full_content = NULL, *path = NULL;

	headers = parse_header_lines(buffer);	

	if (headers && headers[0]) {

		path = parse_request(root, headers[0]);

		if (path) {

			int size = 0;
			content = read_file(path, &size);

			if (content) {

				full_content = cat_headers_to_data(content, size);
				free(content);
				free(path);
				free(headers);

			} else {

				log_error(errno);

				if (errno == ENOENT)
					*is_not_found = 1;

				free(path);
				free(headers);
			}

		} else {

			log_error(errno);
			free(headers);
		}

	} 

	return full_content;

}


#endif
