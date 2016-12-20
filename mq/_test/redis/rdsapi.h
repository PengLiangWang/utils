#ifndef _REDIS_API_H
#define _REDIS_API_H

#include "hiredis.h"
#include <string.h>
#include <stdio.h>

struct rds_handler_s
{
	redisContext	*c;
	redisReply		*r;
	char			hostaddr[20];
	int				hostport;
	struct timeval	timeout;
	char			corr_id[65];
	char			*valuestring;
};

typedef struct rds_handler_s rds_handler_t;

#define RDS_KEY_VALUE	"value"
#define RDS_KEY_TIMESMP	"timestamp"



#endif
