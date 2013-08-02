#include <iostream>
#include <fstream>
#include <string>
#include "addressbook.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

using namespace std;
//using namespace google::protobuf;

google::protobuf::Message* createMessageByName(const std::string& name)
{
	const google::protobuf::Descriptor* descriptor 
		= google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(name);
	if(!descriptor)
	{
		cout << "Descriptor not found." << endl;
		return NULL;
	}

	const google::protobuf::Message* prototype 
		= google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
	if(!prototype)
	{
		cout << "Prototype not found." << endl;
		return NULL;
	}
	return prototype->New();
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cout << "Usage: " << argv[0] << " MessageName " << endl;
		return -1;
	}

	cout << "Person's full name: " << tutorial::Person::descriptor()->full_name() << endl;


	google::protobuf::Message* message = createMessageByName(argv[1]);

	if(message == NULL)
	{
		cout << "No such message!" << endl;
		return 0;
	}
	else
	{
		cout << "The message found" << endl;
		cout << "The message is: " << message << endl;
		return 0;
	}
}
