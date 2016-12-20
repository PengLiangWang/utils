#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tibems/tibems.h>
#include <tibems/emsadmin.h>

char*                           servURL    = "tcp://192.168.1.21:7222";
char*                           userName     = NULL;
char*                           password     = NULL;
char*                           pk_password  = NULL;
tibems_bool                     useTopic     = TIBEMS_FALSE;
tibems_int                      receive      = 1;  /* 初始化接受数据状态 */
tibemsAcknowledgeMode           ackMode      = TIBEMS_AUTO_ACKNOWLEDGE;

tibemsConnectionFactory			factory = NULL;
tibemsConnection                connection   = NULL;
tibemsSession                   session      = NULL;
tibemsMsgProducer               producer  = NULL;
tibemsMsgConsumer               consumer  = NULL;
tibemsDestination               destination  = NULL;

tibemsSSLParams                 sslParams    = NULL;

tibemsErrorContext              errorContext = NULL;

int tibems_send(char *dest, char *buffer)
{
	tibems_status			status;
	tibemsTextMsg           msg;
 
	status = tibemsErrorContext_Create(&errorContext);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create ErrorContext fail\n");
		return -1;
	}

	factory = tibemsConnectionFactory_Create();
	if(factory == NULL)
	{
		fprintf(stderr, "Create ConnectFactory fail\n");
		return -1;
	}

	status = tibemsConnectionFactory_SetServerURL(factory, servURL);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Set Server URL fail\n");
		return -1;
	}

	if(sslParams)
    {
        status = tibemsConnectionFactory_SetSSLParams(factory, sslParams);
        if (status != TIBEMS_OK) 
        {
            fprintf(stderr, "Error setting ssl params");
        }
        status = tibemsConnectionFactory_SetPkPassword(factory, pk_password);
        if (status != TIBEMS_OK) 
        {
            fprintf(stderr, "Error setting pk password");
        }
    }
	
	status = tibemsConnectionFactory_CreateConnection(factory, &connection, userName, password);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Connection Fail\n");
		return -1;
	}
	
	if(useTopic)
		status = tibemsTopic_Create(&destination, dest);
	else
		status = tibemsQueue_Create(&destination, dest);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Queue/Topic Fail\n");
		return -1;
	}

	status = tibemsConnection_CreateSession(connection, &session, TIBEMS_FALSE, TIBEMS_AUTO_ACKNOWLEDGE);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Session Fail\n");
		return -1;
	}

	status = tibemsSession_CreateProducer(session, &producer, destination);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Producer Fail\n");
		return -1;
	}

	/* Create Text Message */
	status = tibemsTextMsg_Create(&msg);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create TextMessage\n");
		return -1;		
	}

	#if 0
	/* 设置唯一关联属性 */
	tibemsMsg_SetCorrelationID(msg, corrid);
	tibemsMsg_SetMessageID(msg, messageId);
	tibemsMsg_SetExpiration(msg, expirate);  /* 消息有效时间设置 */
	#endif

	status = tibemsTextMsg_SetText(msg, buffer);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Set Text Message Fail\n");
		return -1;
	}

	status = tibemsMsgProducer_Send(producer, msg);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Producer Send Fail\n");
		return -1;
	}

	status = tibemsMsg_Destroy(msg);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "destroy msg fail\n");
		return -1;
	}

	status = tibemsDestination_Destroy(destination);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "destroy destination fail\n");
		return -1;
	}

	status = tibemsConnection_Close(connection);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "close connecttion fail\n");
		return -1;
	}

	tibemsErrorContext_Close(errorContext);

	return 0;
}

/* 监听异常处理 */
void onException(tibemsConnection conn, tibems_status reason, void* closure)
{
    if (reason == TIBEMS_SERVER_NOT_CONNECTED)
    {
        printf("CONNECTION EXCEPTION: Server Disconnected\n");
        receive = 0;
    }
}

int tibems_recv(char *dest, char *recvbuff)
{
	tibems_status               status      = TIBEMS_OK;
    tibemsMsg                   msg         = NULL;
    const char*                 txt         = NULL;
    tibemsMsgType               msgType     = TIBEMS_MESSAGE_UNKNOWN;
    char*                       msgTypeName = "UNKNOWN";

	status = tibemsErrorContext_Create(&errorContext);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create ErrorContext fail\n");
		return -1;
	}
	
	factory = tibemsConnectionFactory_Create();
	if(factory == NULL)
	{
		fprintf(stderr, "Create ConnectFactory fail\n");
		return -1;
	}

	status = tibemsConnectionFactory_SetServerURL(factory, servURL);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Set Server URL fail\n");
		return -1;
	}

	if(sslParams)
    {
        status = tibemsConnectionFactory_SetSSLParams(factory, sslParams);
        if (status != TIBEMS_OK) 
        {
            fprintf(stderr, "Error setting ssl params");
        }
        status = tibemsConnectionFactory_SetPkPassword(factory, pk_password);
        if (status != TIBEMS_OK) 
        {
            fprintf(stderr, "Error setting pk password");
        }
    }
	
	status = tibemsConnectionFactory_CreateConnection(factory, &connection, userName, password);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Connection Fail\n");
		return -1;
	}
	
	status = tibemsConnection_SetExceptionListener(connection, onException, NULL);
    if (status != TIBEMS_OK)
    {
        fprintf(stderr, "Error setting exception listener");
		return -1;
    }

	if(useTopic)
		status = tibemsTopic_Create(&destination, dest);
	else
		status = tibemsQueue_Create(&destination, dest);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Queue/Topic Fail\n");
		return -1;
	}

	status = tibemsConnection_CreateSession(connection, &session, TIBEMS_FALSE, TIBEMS_AUTO_ACKNOWLEDGE);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Session Fail\n");
		return -1;
	}

	/* Create Consumer */
	status = tibemsSession_CreateConsumer(session, &consumer, destination, NULL, TIBEMS_FALSE);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "Create Consumer Fail\n");
		return -1;
	}

	status = tibemsConnection_Start(connection);
	if(status != TIBEMS_OK)
	{
		fprintf(stderr, "start connection error\n");
		return -1;
	}

	/* 读取数据 */
	while(receive)
	{
		/* 没有接收到数据, 就阻塞 */
		//status = tibemsMsgConsumer_Receive(consumer, &msg);
		/* 非阻塞, 没有消息接受， 就返回 TIBESM_NOT_FOUND */
		//status = tibemsMsgConsumer_ReceiveNoWaiter(consumer, &msg);
		/* 非阻塞, 在规定时间内, 没有消息接受, 返回 TIBESM_TIMEOUT (ms) */
		status = tibemsMsgConsumer_ReceiveTimeout(consumer, &msg, 2000);
		if(status != TIBEMS_OK)
		{
			if(status == TIBEMS_INTR)
			{
				return ;
			}
			fprintf(stderr, "fail to recieve msg\n");
			return -1;
		}

		if(msg == NULL)
			break;

		/* 确认消息 */
		if( ackMode == TIBEMS_CLIENT_ACKNOWLEDGE || ackMode == TIBEMS_EXPLICIT_CLIENT_ACKNOWLEDGE ||
			ackMode == TIBEMS_EXPLICIT_CLIENT_DUPS_OK_ACKNOWLEDGE )
		{
			status = tibemsMsg_Acknowledge(msg);
			if(status != TIBEMS_OK)
			{
				fprintf(stderr, "acknowledge error\n");
				return -1;
			}
		}	
		
		/* check message type */
		status = tibemsMsg_GetBodyType(msg, &msgType);
		if(status != TIBEMS_OK)
		{
			fprintf(stderr, "get body type error\n");
			return -1;
		}

		switch(msgType)
		{
			case TIBEMS_MESSAGE:
               	msgTypeName = "MESSAGE";
               	break;

            case TIBEMS_BYTES_MESSAGE:
                msgTypeName = "BYTES";
                break;

            case TIBEMS_OBJECT_MESSAGE:
               	msgTypeName = "OBJECT";
                break;

            case TIBEMS_STREAM_MESSAGE:
                msgTypeName = "STREAM";
                break;

            case TIBEMS_MAP_MESSAGE:
                msgTypeName = "MAP";
                break;

            case TIBEMS_TEXT_MESSAGE:
                msgTypeName = "TEXT";
                break;

            default:
                msgTypeName = "UNKNOWN";
                break;
		}
			
		if (msgType != TIBEMS_TEXT_MESSAGE)
        {
           	printf("Received %s message:\n",msgTypeName);
            tibemsMsg_Print(msg);
        }
		else
		{
			/* get the message text */
            status = tibemsTextMsg_GetText(msg,&txt);
            if (status != TIBEMS_OK)
            {
                fprintf(stderr, "Error getting tibemsTextMsg text");
				return -1;
            }

            printf("Received TEXT message: %s\n", txt ? txt : "<text is set to NULL>");
		}
		
		#if 0 
		/* 获取唯一关联属性 */
		char	*corrId;
		tibemsMsg_GetCorrelationID(msg, &corrId);
		tibemsMsg_GetMessageID(msg, messageId);
		#endif
			
		status = tibemsMsg_Destroy(msg);
        if (status != TIBEMS_OK)
        {
            fprintf(stderr, "Error destroying tibemsMsg");
			return -1;
        }
		break;
	}

	/* destroy the destination */
    status = tibemsDestination_Destroy(destination);
    if (status != TIBEMS_OK)
    {
        fprintf(stderr, "Error destroying tibemsDestination");
    }

    /* close the connection */
    status = tibemsConnection_Close(connection);
    if (status != TIBEMS_OK)
    {
        fprintf(stderr, "Error closing tibemsConnection");
    }

    /* destroy the ssl params */
    if (sslParams) 
    {
        tibemsSSLParams_Destroy(sslParams);
    }

	return 0;
}

int main()
{
	int		ret;
#if 0	
	ret = tibems_send("test", "hello World..sdf");
	if( ret )
	{
		fprintf(stderr, "发送队列数据失败");
		return -1;
	}
#endif

	char buffer[256] = {'\0'};
	ret = tibems_recv("test", buffer);
	if(ret)
	{
		fprintf(stderr, "接受队列数据失败\n");
		return -1;
	}

	fprintf(stderr, "发送队列数据成功\n");

	return 0; 
}

