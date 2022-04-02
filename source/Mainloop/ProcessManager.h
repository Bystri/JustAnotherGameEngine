#pragma once

#include "Process.h"

#include <list>

class ProcessManager
{
	typedef std::list<StrongProcessPtr> ProcessList;
	ProcessList m_processList;

public:
	~ProcessManager();

	unsigned int UpdateProcesses(unsigned long deltaMs);
	WeakProcessPtr AttachProcess(StrongProcessPtr pProcess);
	void AbortAllProcesses(bool immediate);

	unsigned int GetProcessCount() const { return m_processList.size();  }

private:
	void ClearAllProcesses();
};