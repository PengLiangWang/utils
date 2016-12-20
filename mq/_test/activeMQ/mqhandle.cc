#include "mqhandle.h"

MSQProducer::~MSQProducer()
{
	this->cleanup();
}

void MSQProducer::close()
{
	this->cleanup();
}

void MSQProducer::initLib()
{
	activemq::library::ActiveMQCPP::initializeLibrary();
}

void MSQProducer::detachLib()
{
	activemq::library::ActiveMQCPP::shutdownLibrary();
}

int MSQProducer::msgsend(char *message, int property)
{
	/* auto_ptr 单独管理内存, 建议不用 */
	auto_ptr<ActiveMQConnectionFactory> connectionFactory(new ActiveMQConnectionFactory(this->brokerURI));
	//ActiveMQConnectionFactory connectionFactory = new ActiveMQConnectionFactory(this->brokerURI);
	this->connection = connectionFactory->createConnection();
	//this->connection =  connectionFactory->createConnection("admin", "admin");
	if(this->connection == NULL)
	{
		return -1;
	}

	connection->start();

	/* create session*/
	if(this->clientAck)
		this->session = connection->createSession(Session::CLIENT_ACKNOWLEDGE);
	else
		this->session = connection->createSession(Session::AUTO_ACKNOWLEDGE);

	if(this->session == NULL)
	{
		return -1;
	}

	/* create destination (Topic or Queue)*/
	if(this->useTopic)
		this->destination = session->createTopic( this->destURI );
	else
		this->destination = session->createQueue( this->destURI );

	this->producer = session->createProducer( destination );
	
	//producer->setDeliveryMode( DeliveryMode::PERSISTENT );
	producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

	/*for test*/	
	//producer->setDisableMessageTimeStamp(false);
	//producer->setTimeToLive(time(NULL)+60);
	
	TextMessage* textmsg = session->createTextMessage((string)message);
	textmsg->setCMSExpiration(2000);
	std::string msgid = "2016112939743454";
	textmsg->setCMSMessageID(msgid);
	textmsg->setIntProperty("Integer", property);
	producer->send(textmsg);

	delete textmsg;

	return 0;
}


void MSQConsumer::cleanup()
{
	if(this->destination != NULL)
		delete this->destination;
	this->destination = NULL;

	if(this->consumer != NULL)
		delete this->consumer;
	this->consumer = NULL;

	if(this->session != NULL)
		this->session->close();
	this->session = NULL;

	if(this->connection != NULL)
		delete this->connection;
	this->connection = NULL;
}

int MSQConsumer::msgrecv(char *recvmsg)
{
	ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);
	this->connection = connectionFactory->createConnection();
	if(this->connection == NULL)
	{
		delete connectionFactory;
		return -1;
	}
	
	delete connectionFactory;

	ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(this->connection);
	if(amqConnection != NULL)
	{
		amqConnection->addTransportListener(this);
	}

	connection->start();

	connection->setExceptionListener(this);
	 
	if(this->clientAck)
		this->session = connection->createSession(Session::CLIENT_ACKNOWLEDGE);
	else
		this->session = connection->createSession(Session::AUTO_ACKNOWLEDGE);

	if(this->useTopic)
		this->destination = session->createTopic(destURI);
	else
		this->destination = session->createQueue(destURI);

	this->consumer = session->createConsumer(destination);
	consumer->setMessageListener(this);

	return 0;
}

void MSQConsumer::onMessage(const Message* message)
{
	const TextMessage* textMessage = dynamic_cast<const TextMessage*>(message);
	string text = "";
	
	if (textMessage != NULL) {
    	text = textMessage->getText();
    } else {
    	//text = "NOT A TEXTMESSAGE!";
    	text = "";
    }

    if (this->clientAck) {
  		message->acknowledge();
    }

	printf("message Id: %s\n", textMessage->getCMSMessageID().c_str());
	this->bufsize = text.length();
	this->tag = 1;
	this->buffer = new char[this->bufsize + 1];
	memcpy(this->buffer, text.c_str(), this->bufsize);
	//this->setMessage(text.c_str())
}

int MSQConsumer::getRecvMsg(char **recvmsg)
{
	while(1)
	{
		if(this->tag == 1) break;
		usleep(300);
	}

	*recvmsg = this->buffer;

	printf("recv : %s\n", *recvmsg);
	return 0;
}

void MSQConsumer::bufferClear()
{
	delete this->buffer;
}

/* C 语言调用API */
extern "C"
{
	
int amcq_msg_send(char *brokerURI, char *destURI, char *msg)
{
	MSQProducer sender(brokerURI, destURI, false, false);

    sender.initLib();
    sender.msgsend(msg, time(NULL));

    sender.close();

    sender.detachLib();

	return 0;
}

int amcq_msg_recv(char *brokerURI, char *destURI, void **recvmsg)
{
	MSQConsumer recver(brokerURI, destURI, false, false);

	recver.initLib();

	recver.msgrecv(NULL);

	recver.getRecvMsg((char **)recvmsg);
	printf("%s\n", (char *)*recvmsg);
	
	recver.close();

    recver.detachLib();
}

/* extern C*/
}
/* extern C*/

