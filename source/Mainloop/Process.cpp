#include "Process.h"

#include <stdexcept>

Process::Process()
	: m_state(UNINITIALIZED)
{

}

Process::~Process() {
	if (m_pChild) {
		m_pChild->VOnAbort();
	}
}

inline void Process::Succeed() {
	if (m_state != RUNNING && m_state != PAUSED)
		throw std::runtime_error("Secceed on a wrong process");

	m_state = SUCCEEDED;
}

inline void Process::Fail() {
	if (m_state != RUNNING && m_state != PAUSED)
		throw std::runtime_error("Fail on a wrong process");

	m_state = FAILED;
}

inline void Process::Pause() {
	if (m_state == RUNNING) {
		m_state = PAUSED;
	}
	/*else {
	*	TODO:: THROW WARNING ("Try to pause innactive process")
	* }
	*/
}

inline void Process::UnPause() {
	if (m_state == PAUSED) {
		m_state == RUNNING;
	}
	/*else {
	*	TODO: THROW WARNING ("Try to unpause active process")
	* }
	*/
}

inline void Process::AttachChild(StrongProcessPtr pChild) {
	if (m_pChild) {
		m_pChild->AttachChild(pChild);
	}
	else {
		m_pChild = pChild;
	}
}

StrongProcessPtr Process::RemoveChild() {
	if (m_pChild) {
		StrongProcessPtr pChild = m_pChild;
		m_pChild.reset();

		return pChild;
	}

	return StrongProcessPtr();
}