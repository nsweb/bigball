

#include "../bigball.h"
#include "controller.h"

namespace bigball
{

STATIC_MANAGER_CPP( Controller )

Controller::Controller()
{
	m_pStaticInstance = this;
}

Controller::~Controller()
{
	m_pStaticInstance = nullptr;
}

void Controller::Create()
{

}
void Controller::Destroy()
{

}

void Controller::Tick( float DeltaSeconds )
{

}

}