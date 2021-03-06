/*
 * Copyright (c) 2011-2013 Kilian Klimek <kilian.klimek@googlemail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "main.h"
#include "client.h"
#include "misc.h"
#include "child_config.h"

static char get_opts[] = "adDefgGhHikosuU";

static struct option get_longopts[] = {
	{ "age",	no_argument,		NULL,	'a' },
	{ "dir",	no_argument,		NULL,	'd' },
	{ "dump",	no_argument,		NULL,	'D' },
	{ "stderr",	no_argument,		NULL,	'e' },
	{ "fatal",	no_argument,		NULL,	'f' },
	{ "gid",	no_argument,		NULL,	'g' },
	{ "groupname",	no_argument,		NULL,	'G' },
	{ "help",	no_argument,		NULL,	'h' },
	{ "heartbeat",	no_argument,		NULL,	'H' },
	{ "instances",	no_argument,		NULL,	'i' },
	{ "killsig",	no_argument,		NULL,	'k' },
	{ "stdout",	no_argument,		NULL,	'o' },
	{ "status",	no_argument,		NULL,	's' },
	{ "uid",	no_argument,		NULL,	'u' },
	{ "username",	no_argument,		NULL,	'U' },
	{ NULL,		0,			NULL,	0 }
};

static void
help_get(void)
{
	printf("Usage: %s get [Options] <name>\n", program_name);
	printf("\n");
	printf("Options:\n");
	printf("\t-a, --age        print age.\n");
	printf("\t-d, --dir        print dir.\n");
	printf("\t-D, --dump       print raw reply.\n");
	printf("\t-e, --stderr     print stderr.\n");
	printf("\t-f, --fatal      print fatal_cb.\n");
	printf("\t-g, --gid        print gid processes are started with.\n");
	printf("\t-h, --help       help.\n");
	printf("\t-H, --heartbeat  print heartbeat command.\n");
	printf("\t-i, --instances  print number of instances.\n");
	printf("\t-k, --killsig    print signal used to kill processes.\n");
	printf("\t-o, --stdout     print stdout.\n");
	printf("\t-s, --status     print status.\n");
	printf("\t-u, --uid        print uid processes are started with.\n");
	printf("\n");
	exit(EXIT_FAILURE);
}

int
cmd_get(int argc, char **argv)
{
	int			ch,
				sock,
				dump = 0,
				get_stdout = 0,
				get_stderr = 0,
				get_age = 0,
				get_dir = 0,
				get_uid = 0,
				get_gid = 0,
				get_status = 0,
				get_instances = 0,
				get_killsig = 0,
				get_heartbeat = 0,
				get_fatal = 0,
				get_username = 0,
				get_groupname = 0;

	char			*msg;

	char			*buf;
	size_t			buf_siz;

	json_object		*obj,
				*n;

	if (argc < 2)
		help_get();

	while ((ch = getopt_long(argc, argv, get_opts, get_longopts, NULL)) != -1) {
		switch (ch) {
		case 'a':
			get_age = 1;
			break;
		case 'd':
			get_dir = 1;
			break;
		case 'D':
			dump = 1;
			break;
		case 'e':
			get_stderr = 1;
			break;
		case 'f':
			get_fatal = 1;
			break;
		case 'g':
			get_gid = 1;
			break;
		case 'G':
			get_groupname = 1;
			break;
		case 'h':
			help_get();
			break;
		case 'H':
			get_heartbeat = 1;
			break;
		case 'i':
			get_instances = 1;
			break;
		case 'k':
			get_killsig = 1;
			break;
		case 'o':
			get_stdout = 1;
			break;
		case 's':
			get_status = 1;
			break;
		case 'u':
			get_uid = 1;
			break;
		case 'U':
			get_username = 1;
			break;
		default:
			help_get();
			break;
		}
	}

	argv += optind;
	argc -= optind;

	if (argc != 1)
		help_get();

	obj = json_object_new_object();
	n = json_object_new_string(argv[0]);
	json_object_object_add(obj, "name", n);

	msg = xstrdup(json_object_to_json_string(obj));

	json_object_put(obj);

	if ((sock = sock_connect()) == -1) {
		die("Failed to connect server");
	}

	if (sock_send_command(sock, "GETC", msg) == -1) {
		fprintf(stderr, "failed to send command.\n");
		return EXIT_FAILURE;
	}

	free(msg);

	if ((buf = read_reply(sock, &buf_siz)) == NULL) {
		fprintf(stderr, "failed to read reply.\n");
		return EXIT_FAILURE;
	}

	if (dump) {
		printf("%s\n", buf);
		return EXIT_SUCCESS;
	}

	if ((obj = json_tokener_parse(buf)) == NULL) {
		free(buf);
		fprintf(stderr, "failed.\n");
		return EXIT_FAILURE;
	}

	if (is_error(obj)) {
		free(buf);
		fprintf(stderr, "failed.\n");
		return EXIT_FAILURE;
	}

	free(buf);

	if (!json_object_is_type(obj, json_type_object)) {
		fprintf(stderr, "failed.\n");
		return EXIT_FAILURE;
	}

#define GETSTR(X,Y)	if (Y) { \
				if ((n = json_object_object_get(obj, X)) == NULL) { \
					fprintf(stderr, "failed.\n"); \
					return EXIT_FAILURE; \
				} \
				if (!json_object_is_type(n, json_type_string)) { \
					fprintf(stderr, "failed.\n"); \
					return EXIT_FAILURE; \
				} \
				printf("%s\n", json_object_get_string(n)); \
			}

#define GETINT(X,Y)	if (Y) { \
				if ((n = json_object_object_get(obj, X)) == NULL) { \
					fprintf(stderr, "failed.\n"); \
					return EXIT_FAILURE; \
				} \
				if (!json_object_is_type(n, json_type_int)) { \
					fprintf(stderr, "failed.\n"); \
					return EXIT_FAILURE; \
				} \
				printf("%d\n", json_object_get_int(n)); \
			}

	GETSTR("stdout", get_stdout);
	GETSTR("stderr", get_stderr);
	GETSTR("dir", get_dir);
	GETSTR("heartbeat", get_heartbeat);
	GETSTR("fatal_cb", get_fatal);
	GETSTR("username", get_username);
	GETSTR("groupname", get_groupname);
	GETINT("age", get_age);
	GETINT("uid", get_uid);
	GETINT("gid", get_gid);
	GETINT("status", get_status);
	GETINT("instances", get_instances);
	GETINT("killsig", get_killsig);
	return EXIT_SUCCESS;
}
