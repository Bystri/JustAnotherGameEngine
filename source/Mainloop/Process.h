#pragma once

#include <memory>

class Process;
typedef std::shared_ptr<Process> StrongProcessPtr;
typedef std::weak_ptr<Process> WeakProcessPtr; 

class Process {
	friend class ProcessManager;

public:
	enum State
	{
		UNINITIALIZED = 0, //Created but not running
		REMOVED, //removed from the process list but not destroyed; 
		//this can happen when a process that is already running is 
		// parented to another process.

		//Living processes
		RUNNING, //Initialized and running
		PAUSED, //initialized but paused

		//Dead processes
		SUCCEEDED, //completed successfully
		FAILED, //failed to complete
		ABORTED // aborted; may not have started
	};

private:
	State m_state;
	StrongProcessPtr m_pChild;

public:
	Process();
	virtual ~Process();

protected:
	virtual void VOnInit() { m_state = RUNNING; }
	virtual void VOnUpdate(unsigned long deltaMs) = 0;
	virtual void VOnSuccess() {}
	virtual void VOnFail() {}
	virtual void VOnAbort() {}

public:
	inline void Succeed();
	inline void Fail();

	inline void Pause();
	inline void UnPause();

	State GetState() const { return m_state; }
	bool IsAlive() const {
		return (m_state == RUNNING || m_state == PAUSED);
	}
	bool IsDead() const {
		return (m_state == SUCCEEDED || m_state == FAILED || m_state == ABORTED);
	}
	bool IsRemoved() const { return m_state == REMOVED; }
	bool IsPaused() const { return m_state == PAUSED; }

	inline void AttachChild(StrongProcessPtr pChild);
	StrongProcessPtr RemoveChild();
	StrongProcessPtr PeekChild() { return m_pChild; }

private:
	void SetState(State newState) { m_state = newState; }
};