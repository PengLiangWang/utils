#ifndef _MQ_HANDLE_H
#define _MQ_HANDLE_H

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

class MSQProducer
{
private:
	Connection* 		connection;
    Session* 			session;
    Destination* 		destination;
	MessageProducer* 	producer;
    bool 				useTopic;
    std::string 		brokerURI;
    std::string 		destURI;
    bool 				clientAck;
	int					diliveryMode;  /* 消息保存模式 */
private:
	void cleanup();

public:
	MSQProducer(const std::string& brokerURI, const std::string& destURI, bool useTopic, bool clientAck):
		connection(NULL),
        session(NULL),
        destination(NULL),
        producer(NULL),
        useTopic(useTopic),
        clientAck(clientAck),
        brokerURI(brokerURI),
        destURI(destURI)
	{ ; }

	~MSQProducer();

	void initLib();
	void detachLib();
	void close();
	void setMsgMode(int mode) { this->diliveryMode = mode; }
	int msgsend(char *message, int property);

};

using namespace activemq::transport;

class MSQConsumer : public ExceptionListener, public MessageListener, public DefaultTransportListener 
{
private:
	Connection* 		connection;
    Session* 			session;
    Destination* 		destination;
    MessageConsumer* 	consumer;
    bool 				useTopic;
    std::string 		brokerURI;
    std::string 		destURI;
    bool 				clientAck;
	char*				buffer;
	int 				bufsize;
	int					tag;

private:
	void cleanup()
	{
		if(this->destination != NULL)
        	delete this->destination;
    	this->destination = NULL;

    	if(this->producer != NULL)
        	delete this->producer;
    	this->producer = NULL;

    	if(this->session != NULL)
        	delete this->session;
    	this->session = NULL;

    	if(this->connection != NULL)
        	delete this->connection;
    	this->connection = NULL;
	}

public:
	MSQConsumer(const std::string& brokerURI, const std::string& destURI, bool useTopic, bool clientAck)
	{
		this->connection = NULL;
		this->session = NULL;
		this->destination = NULL;
		this->consumer = NULL;
		this->useTopic = useTopic;
		this->brokerURI = brokerURI;
		this->destURI = destURI;
		this->clientAck = clientAck;
		this->buffer = NULL;
		this->bufsize = 0;
		this->tag = 0;
	}

	~MSQConsumer() 
	{ 
		this->cleanup(); 
	}

	void initLib()
	{
		activemq::library::ActiveMQCPP::initializeLibrary();	
	}

	void detachLib()
	{
		activemq::library::ActiveMQCPP::shutdownLibrary();
	}

	void close()
	{
		this->cleanup();
	}

	int msgrecv(char *recvmsg);

	int getRecvMsg(char **recvmsg);

	void bufferClear();

	virtual void onMessage(const Message* message);

	virtual void onException(const CMSException& ex AMQCPP_UNUSED) {
        printf("CMS Exception occurred.  Shutting down client.\n");
        exit(1);
    }

	virtual void onException(const decaf::lang::Exception& ex) {
        printf("Transport Exception occurred: %s.\n", ex.getMessage().c_str());
    }

	virtual void transportInterrupted() {
        std::cout << "The Connection's Transport has been Interrupted." << std::endl;
    }

	virtual void transportResumed() {
        //std::cout << "The Connection's Transport has been Restored." << std::endl;
    }

};

#endif
