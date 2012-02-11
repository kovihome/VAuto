#include "TelescopeNotifier.h"

TelescopeNotifier::TelescopeNotifier(wxWindow* parent, int id, Telescope* telescope)
 : wxThread ()
{
	m_parent = parent;
	m_telescope = telescope;
	m_id = id;
}

void TelescopeNotifier::SendEvent (TelescopeNotifier::TelescopeStopAction stopAction)
{
	wxLogDebug (wxT("TelescopeNotifier::SendEvent post a notification event."));
	wxCommandEvent ev (wxEVT_COMMAND_BUTTON_CLICKED, m_id);
	ev.SetInt (stopAction);
	wxPostEvent (m_parent, ev);
}

void TelescopeNotifier::OnExit ()
{
}

#define TELESCOPE_MOVING_TIMEOUT	20 // sec

#define TN_SLEEP_TIME				1000 // ms	
#define TN_TIMEOUT(s)				(s*1000/TN_SLEEP_TIME)

wxThread::ExitCode TelescopeNotifier::Entry ()
{
	int timeout = TN_TIMEOUT(TELESCOPE_MOVING_TIMEOUT);
	m_abort = FALSE;

	bool isConnected = m_telescope->IsConnected();
	bool isMoving = m_telescope->IsMoving();
	while (!m_abort && timeout-- > 0 && isConnected && isMoving) {
		wxThread::Sleep (TN_SLEEP_TIME);
//		wxCriticalSection cs;
//		cs.Enter ();
		if (!m_abort && m_telescope != NULL) {
			isConnected = m_telescope->IsConnected();
			}
//		cs.Leave ();
//		cs.Enter ();
		if (!m_abort && m_telescope != NULL) {
			isMoving = m_telescope->IsMoving();
			}
//		cs.Leave ();
		}

	TelescopeNotifier::TelescopeStopAction stopAction;
	if (m_abort) {
		m_telescope->StopMoving (); // force the telescope stops moving
		stopAction = TelescopeNotifier::STOP_ABORT;
		}
	else if (timeout <= 0) {
		m_telescope->StopMoving (); // force the telescope stops moving
		stopAction = TelescopeNotifier::STOP_TIMEOUT;
		}
	else if (!isConnected) {
		stopAction = TelescopeNotifier::STOP_DISCONNECT;
		}
	else {
		stopAction = TelescopeNotifier::STOP_NORMALLY;
		}

	// notify parent
	SendEvent (stopAction);

	return NULL;
}

void TelescopeNotifier::Abort ()
{
	m_abort = TRUE;
}

