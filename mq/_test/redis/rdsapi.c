#include "rdsapi.h"
#include "exutil.h"

void rds_handler_init(rds_handler_t *rds)
{
	memset(rds, 0, sizeof(rds_handler_t));
	rds->c = NULL;
	rds->r = NULL;
}

void rds_set_timeout(rds_handler_t *rds, int timeout)
{
	rds->timeout.tv_sec = timeout / 1000;
	rds->timeout.tv_usec = ( timeout % 1000 ) * 1000;
}

void rds_set_address(rds_handler_t *rds, char *hostaddr, int hostport)
{
	strncpy(rds->hostaddr, hostaddr, 20);
	rds->hostport = hostport;
}


static int rds_connect(rds_handler_t *rds)
{
	if(rds->timeout.tv_sec == 0 && rds->timeout.tv_usec == 0)
		rds->c = redisConnect(rds->hostaddr, rds->hostport);
	else
		rds->c = redisConnectWithTimeout(rds->hostaddr, rds->hostport, rds->timeout);
	
	if(rds->c == NULL || rds->c->err)
	{
		if(rds->c)
		{
			//elog(ll_error, "connect redis server error: %s", rds->c->err);
			printf("connect redis server error: %s\n", rds->c->errstr);
			redisFree(rds->c);
		}
		else
		{
			//elog(ll_error, "Connection error: can't allocate redis context");
			//set_error(22, "Connection error: can't allocate redis context");
			printf("Connection error: can't allocate redis context\n");
		}
		return -1;
	}
	
	return 0;
}

int rds_handler_recv_cycle(rds_handler_t *rds, char *dest, char *recvbuf, int size)
{
	char		corr_id[65] = {'\0'};
	int			count = 0;
	redisReply	*r;
	
	if(rds->c == NULL)
		if( rds_connect(rds) )
			return -1;
	
cycle:
	r = redisCommand(rds->c, "RPOP %s", dest);
	if(r->type == REDIS_REPLY_STRING)
	{
		memcpy(corr_id, (char *)r->str, 64);
	}
	else if(r->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(r);
		if(count++ == 10)
			return 1;
		
		usleep(500);
		goto cycle;
	}
	else
	{
		freeReplyObject(r);
		return -1;
	}

	freeReplyObject(r);
	
	r = redisCommand(rds->c, "HGET %s %s", corr_id, RDS_KEY_VALUE);
	if(r->type == REDIS_REPLY_STRING)
	{
		memcpy(recvbuf, (char *)r->str, size);	
	}
	else
	{
		freeReplyObject(r);
		return -1;
	}

	
	freeReplyObject(r);

	r = redisCommand(rds->c, "DEL %s", corr_id);
	
	freeReplyObject(r);

	return 0;
}

int rds_handler_send_cycle(rds_handler_t *rds, char *dest, char *corr_id, char *sendbuf)
{
	if(rds->c == NULL)
		if( rds_connect(rds) )
			return -1;

	rds->r = redisCommand(rds->c, "HSET %s %s %s", rds->corr_id, RDS_KEY_VALUE, sendbuf);

	rds->r = redisCommand(rds->c, "HSET %s %s %d", rds->corr_id, "timestamp", time(NULL));

	rds->r = redisCommand(rds->c, "LPUSH %s %s", dest, rds->corr_id);

	return 0;
}

int rds_handler_recv(rds_handler_t *rds, char *dest, char *corr_id, char *recvbuf)
{
	if( rds_connect(rds) )
		return -1;

#if 0
	rds->r = redisCommand(rds->c, "SUBSCRIBE %s", dest);
	
	fprintf(stderr, "type: %d, size: %d\n", rds->r->type, (int)rds->r->elements);

	int i;
	redisReply	*r;
	for(i=0; i<(int)rds->r->elements; i++)
	{
		r = rds->r->element[i];
		fprintf(stderr, "%d, %d, %s\n", r->type, r->element, r->str);
	}
#endif

	redisReply		*r;
	int		count = 0;
	char	push_key[100] = {'\0'};
	sprintf(push_key, "%s.%s", dest, corr_id);

waiting:
	r = redisCommand(rds->c, "RPOP %s", push_key);
	if(r->type == REDIS_REPLY_NIL)
	{
		if(++count < 100)
		{
			usleep(50000);
			goto waiting;
		}
		fprintf(stderr, "接受数据超时");
		return -1;
	}

	if(strcmp(r->str, corr_id) != 0)
	{
		fprintf(stderr, "获取队列数据异常");
		return -1;
	}

	freeReplyObject(r);

	r = redisCommand(rds->c, "HGET %s %s", corr_id, RDS_KEY_VALUE);
	
	strcpy(recvbuf, rds->r->str);

	freeReplyObject(r);

	r = redisCommand(rds->c, "DEL %s", corr_id);

	return 0;
}

int rds_handler_send(rds_handler_t *rds, char *dest, char *corr_id, char *sendbuf)
{
	redisReply		*r;

	if( rds_connect(rds) )
		return -1;

#if 0
	char	push_key[100] = {'\0'};

	r = redisCommand(rds->c, "HSET %s %s %s", corr_id, RDS_KEY_VALUE, sendbuf);
	freeReplyObject(r);

	r = redisCommand(rds->c, "HSET %s %s %d", corr_id, RDS_KEY_TIMESMP, time(NULL));
	freeReplyObject(r);

	sprintf(push_key, "%s.%s", dest, corr_id);
	
	r = redisCommand(rds->c, "LPUSH %s %s", push_key, corr_id);
	freeReplyObject(r);

	r = redisCommand(rds->c, "LPUSH %s %s", dest, push_key);
	freeReplyObject(r);
#endif

	r = redisCommand(rds->c, "HSET %s %s %s", corr_id, RDS_KEY_VALUE, sendbuf);
	freeReplyObject(r);

	r = redisCommand(rds->c, "HSET %s %s %d", corr_id, RDS_KEY_TIMESMP, time(NULL));
	freeReplyObject(r);

	r = redisCommand(rds->c, "LPUSH %s %s", dest, corr_id);
	freeReplyObject(r);

	redisFree(rds->c);

	return 0;
}

