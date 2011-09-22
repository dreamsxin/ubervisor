/*
 * Copyright (c) 2011 Kilian Klimek <kilian.klimek@googlemail.com>
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

#include <json/json.h>

#include "misc.h"
#include "child_config.h"


char *
child_config_serialize(const struct child_config *cc)
{
	json_object		*obj,
				*t,
				*s;
	int			x;
	char			*ret;

#define ADD(X, Y)	if (Y != NULL) { \
				t = json_object_new_string(Y); \
				json_object_object_add(obj, X, t); \
			}

#define ADDINT(X, Y)	if (Y != -1) { \
				t = json_object_new_int(Y); \
				json_object_object_add(obj, X, t); \
			}

	obj = json_object_new_object();
	ADD("name", cc->cc_name);
	ADD("stdout", cc->cc_stdout);
	ADD("stderr", cc->cc_stderr);
	ADD("dir", cc->cc_dir);
	ADD("heartbeat", cc->cc_heartbeat);
	ADD("fatal_cb", cc->cc_fatal_cb);
	ADD("stdout_pipe", cc->cc_stdout_pipe);
	ADDINT("instances", cc->cc_instances);
	ADDINT("status", cc->cc_status);
	ADDINT("killsig", cc->cc_killsig);
	ADDINT("uid", cc->cc_uid);
	ADDINT("gid", cc->cc_gid);
	ADDINT("error", cc->cc_error);

	if (cc->cc_command != NULL) {
		t = json_object_new_array();
		x = 0;

		while (cc->cc_command[x] != NULL) {
			s = json_object_new_string(cc->cc_command[x]);
			json_object_array_add(t, s);
			x++;
		}
		json_object_object_add(obj, "args", t);
	}

	ret = xstrdup(json_object_to_json_string(obj));
	json_object_put(obj);

	return ret;
}

struct child_config *child_config_from_json(json_object *obj)
{
	struct child_config	*ret;
	json_object		*t,
				*s;
	int			i, len;

	ret = child_config_new();

#define GET(X, Y)	if ((t = json_object_object_get(obj, Y)) != NULL) { \
				if (!json_object_is_type(t, json_type_string)) { \
					json_object_put(obj); \
					return NULL; \
				} \
				X = xstrdup(json_object_get_string(t)); \
			}


#define GETINT(X, Y)	if ((t = json_object_object_get(obj, Y)) != NULL) { \
				if (!json_object_is_type(t, json_type_int)) { \
					json_object_put(obj); \
					return NULL; \
				} \
				X = json_object_get_int(t); \
			}

	GET(ret->cc_name, "name");
	if (ret->cc_name == NULL)
		return NULL;
	GET(ret->cc_stdout, "stdout");
	GET(ret->cc_stderr, "stderr");
	GET(ret->cc_dir, "dir");
	GET(ret->cc_heartbeat, "heartbeat");
	GET(ret->cc_fatal_cb, "fatal_cb");
	GET(ret->cc_stdout_pipe, "stdout_pipe");
	GETINT(ret->cc_instances, "instances");
	GETINT(ret->cc_status, "status");
	GETINT(ret->cc_killsig, "killsig");
	GETINT(ret->cc_uid, "uid");
	GETINT(ret->cc_gid, "gid");
	GETINT(ret->cc_error, "error");

	if ((t = json_object_object_get(obj, "args")) != NULL) {
		if (!json_object_is_type(t, json_type_array))
			return NULL;

		len = json_object_array_length(t);
		ret->cc_command = xmalloc(sizeof(char *) * (len + 1));

		for (i = 0; i < len; i++) {
			if ((s = json_object_array_get_idx(t, i)) == NULL)
				return NULL;
			ret->cc_command[i] = xstrdup(json_object_get_string(s));
		}
		ret->cc_command[i] = NULL;
	}

	json_object_put(obj);
	return ret;

}

struct child_config *child_config_unserialize(const char *buf)
{
	json_object		*obj;

	if ((obj = json_tokener_parse(buf)) == NULL)
		return NULL;
	if (is_error(obj))
		return NULL;
	if (!json_object_is_type(obj, json_type_object))
		return NULL;
	return child_config_from_json(obj);
}

void
child_config_free(struct child_config *cc)
{
	int		i;

#define FREE(X)		if (X) \
				free(X);

	FREE(cc->cc_name);
	FREE(cc->cc_stdout);
	FREE(cc->cc_stderr);
	FREE(cc->cc_dir);
	FREE(cc->cc_heartbeat);
	FREE(cc->cc_fatal_cb);
	FREE(cc->cc_stdout_pipe);
	if(cc->cc_command) {
		for (i = 0; cc->cc_command[i] != NULL; i++)
			free(cc->cc_command[i]);
		free(cc->cc_command);
	}
	free(cc);
}

struct child_config *child_config_new(void)
{
	struct child_config	*cc;

	cc = xmalloc (sizeof(struct child_config));
	memset(cc, '\0', sizeof(struct child_config));
	cc->cc_instances = -1;
	cc->cc_status = -1;
	cc->cc_killsig = -1;
	cc->cc_uid = -1;
	cc->cc_gid = -1;
	return cc;
}
