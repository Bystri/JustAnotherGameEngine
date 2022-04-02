#include "ProcessManager.h"

ProcessManager::~ProcessManager() {
	ClearAllProcesses();
}

unsigned int ProcessManager::UpdateProcesses(unsigned long deltaMs) {
	unsigned short int successCount = 0;
	unsigned short int failCount = 0;

	ProcessList::iterator it = m_processList.begin();
	while (it != m_processList.end()) {
		StrongProcessPtr pCurrProcess = *it;

		if (pCurrProcess->GetState() == Process::UNINITIALIZED)
			pCurrProcess->VOnInit();

		if (pCurrProcess->GetState() == Process::RUNNING)
			pCurrProcess->VOnUpdate(deltaMs);

		if (pCurrProcess->IsDead()) {
			switch (pCurrProcess->GetState())
			{
			case Process::SUCCEEDED:
			{
				pCurrProcess->VOnSuccess();
				StrongProcessPtr pChild = pCurrProcess->RemoveChild();
				if (pChild)
					AttachProcess(pChild);
				else
					++successCount;

				break;
			}
			case Process::FAILED:
			{
				pCurrProcess->VOnFail();
				++failCount;
				break;
			}
			case Process::ABORTED:
			{
				pCurrProcess->VOnAbort();
				++failCount;
				break;
			}
			default:
				break;
			}

			it = m_processList.erase(it);
		}
	}

	return ((successCount << 16) | failCount);
}

WeakProcessPtr ProcessManager::AttachProcess(StrongProcessPtr pProcess) {
	m_processList.push_front(pProcess);
	return WeakProcessPtr(pProcess);
}

void ProcessManager::AbortAllProcesses(bool immediate) {
	for (auto it = m_processList.begin(); it != m_processList.end(); ++it) {
		StrongProcessPtr pProcess = *it;
		if (pProcess->IsAlive()) {
			pProcess->SetState(Process::ABORTED);

			if (immediate) {
				pProcess->VOnAbort();
				it = m_processList.erase(it);
			}
		}
	}
}

void ProcessManager::ClearAllProcesses() {
	m_processList.clear();
}