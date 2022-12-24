/*
 * Copyright (c) 2003,2004 Armin Wolfermann
 * Copyright (c) 2021 Henry Zhang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
 
#include <config.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dnsproxy.h"

struct static_answer {
    char* domain;
    int32_t ip;
};

static struct static_answer *static_answers = NULL;
static unsigned int static_answers_alloc = 0;
static unsigned int static_answers_count = 0;

int
add_static_answer(char *s, char* ip_str)
{
    unsigned int desired_alloc_size;
    char *s_dup;
    int32_t ip;
    if (s == NULL || NULL == ip_str) // why would i even need this?
        return 0;

    if(static_answers_count <= static_answers_alloc)
    {
        desired_alloc_size = static_answers_alloc ? static_answers_alloc*2u : 1u;
        if(desired_alloc_size > static_answers_alloc)
        {
            if(0 == static_answers_alloc)
            {
                static_answers = (struct static_answer *)malloc(desired_alloc_size * sizeof(struct static_answer));
                if(NULL == static_answers)
                {
                    DPRINTF(("add_static_answer failed, domain=%s, malloc failed\n", s));
                    return 0;
                }
            }
            else
            {
                static_answers = (struct static_answer *)realloc(static_answers, desired_alloc_size * sizeof(struct static_answer));
                if(NULL == static_answers)
                {
                    DPRINTF(("add_static_answer failed, domain=%s, realloc failed\n", s));
                    return 0;
                }
            }
        }
        else
        {
            DPRINTF(("add_static_answer failed, domain=%s, my counter only %lu bit width\n", s, 8*sizeof(static_answers_alloc)));
            return 0;
        }
        static_answers_alloc = desired_alloc_size;
    }

    s_dup = strdup(s);
    ip = inet_addr(ip_str);
    if(NULL != s_dup)
    {
        if(-1 != ip)
        {
            static_answers[static_answers_count].domain = s_dup;
            static_answers[static_answers_count++].ip = ip;
        }
        else
        {
            DPRINTF(("add_static_answer failed, domain=%s, ip=%s, inet_addr failed\n", s, ip_str));
            return 0;
        }        
    }
    else
    {
        DPRINTF(("add_static_answer failed, domain=%s, ip=%s, strdup failed\n", s, ip_str));
        return 0;
    }

    DPRINTF(("add_static_answer %s : %s\n", s, ip_str));

    return 1;
}

uint32_t
is_static_answer(char* s)
{
    unsigned int i;
    if(0u == static_answers_count)
    {
        // nothing added
        return 0;
    }
    DPRINTF(("is_static_answer(%s)\n", s));
    for(i=0u; i<static_answers_count; ++i)
    {
        // case insensitive not my thing
        if(!strcmp(s, static_answers[i].domain))
        {
            return static_answers[i].ip;
        }
    }
    return 0;
}

int
deinit_static_answer()
{
    // unimplemented, bite me not
    return 1;
}
