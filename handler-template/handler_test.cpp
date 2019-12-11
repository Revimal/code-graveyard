#include <iostream>

#include "handler_template.hpp"

class TestClass {
private:
	int data_;

public:
	TestClass() : data_(0) {};

	static int testStaticMethod(const int data) {
		return data+1;
	}

	int testMemberMethod(void) {
		return data_+1;
	}
};

int testGlobalFunction(const int data) {
	return data+1;
}

int main( int argc, char *argv[] ) {
	class TestClass testInst;
	int counter = 0;

	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass(&TestClass::testMemberMethod)(&testInst);
	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass(TestClass::testStaticMethod)(counter);
	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass(std::ref(TestClass::testStaticMethod).get())(counter);
	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass(&TestClass::testStaticMethod)(counter);
	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass(testGlobalFunction)(counter);
	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass(std::ref(testGlobalFunction).get())(counter);
	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass(&testGlobalFunction)(counter);
	std::cout << counter << std::endl;

	counter = HandlerTemplate::newHandlerClass([](const int data) { return data+1; })(counter);
	std::cout << counter << std::endl;

	return 0;
}
