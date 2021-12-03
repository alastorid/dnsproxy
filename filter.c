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

struct allowed_query {
	char* domain;
};

static struct allowed_query *allowed_queries = NULL;
static unsigned int allowed_queries_alloc = 0;
static unsigned int allowed_queries_count = 0;

int
add_allowed_query(char *s)
{
  unsigned int desired_alloc_size;
  char *s_dup;
  if (s == NULL) // why would i even need this?
		return 0;
  
  if(allowed_queries_count <= allowed_queries_alloc)
  {
    desired_alloc_size = allowed_queries_alloc? 1u : allowed_queries_alloc*2u;
    if(desired_alloc_size > allowed_queries_alloc)
    {
      if(0 == allowed_queries_alloc)
      {
        allowed_queries = (struct allowed_query *)malloc(desired_alloc_size * sizeof(struct allowed_query));
        if(NULL == allowed_queries)
        {
          DPRINTF(("add_allowed_query failed, domain=%s, malloc failed\n", s));
          return 0;
        }
      }
      else
      {
        allowed_queries = (struct allowed_query *)realloc(allowed_queries, desired_alloc_size * sizeof(struct allowed_query));
        if(NULL == allowed_queries)
        {
          DPRINTF(("add_allowed_query failed, domain=%s, realloc failed\n", s));
          return 0;
        }
      }
      allowed_queries_alloc = desired_alloc_size;
    }
    else
    {
      DPRINTF(("add_allowed_query failed, domain=%s, my counter only %d bit width\n", s, 8*sizeof(allowed_queries_alloc)));
      return 0;
    } 
  }
  s_dup = strdup(s);
  if(NULL != s_dup)
  {
    allowed_queries[allowed_queries_count++].domain = s_dup;
  }
  else
  {
    DPRINTF(("add_allowed_query failed, domain=%s, strdup failed\n", s))    
    return 0;
  }
  
	DPRINTF(("add_allowed_query %s\n", s))

	return 1;
}

int
is_allowed_query(char* s)
{
  unsigned int i;
  if(0u == allowed_queries_count)
  {
    // nothing added
    return 1;
  }
  //DPRINTF(("is_internal(%s)\n", s));
  for(i=0u; i<allowed_queries_count; ++i)
  {
    // case insensitive not my thing 
    if(!strcmp(s, allowed_queries[i].domain))
    {
      return 1;
    }    
  }	
	return 0;
}

int
deinit_allowed_query()
{
  // unimplemented, bite me not
  return 1;
}
