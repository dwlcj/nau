#include "dlgDbgGLILogRead.h"
#include <nau.h>
#include <nau/debug/profile.h>
#include "..\..\GLIntercept\Src\MainLib\ConfigDataExport.h"
#include <dirent.h>

BEGIN_EVENT_TABLE(DlgDbgGLILogRead, wxDialog)

END_EVENT_TABLE()


wxWindow *DlgDbgGLILogRead::m_Parent = NULL;
DlgDbgGLILogRead *DlgDbgGLILogRead::m_Inst = NULL;


void 
DlgDbgGLILogRead::SetParent(wxWindow *p) {

	m_Parent = p;
}


DlgDbgGLILogRead* 
DlgDbgGLILogRead::Instance () {

	if (m_Inst == NULL)
		m_Inst = new DlgDbgGLILogRead();

	return m_Inst;
}
 

DlgDbgGLILogRead::DlgDbgGLILogRead(): wxDialog(DlgDbgGLILogRead::m_Parent, -1, wxT("Nau - GLINTERCEPT LOG"),wxDefaultPosition,
						   wxDefaultSize,wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE)
{

	this->SetSizeHints( wxDefaultSize, wxDefaultSize);

	wxBoxSizer *bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL);

	wxStaticBoxSizer * sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );

	m_log = new wxTreeCtrl(this,NULL,wxDefaultPosition,wxDefaultSize,wxLB_SINGLE | wxLB_HSCROLL | wxEXPAND);

	sbSizer1->Add(m_log, 1, wxALL|wxEXPAND, 5);

	bSizer1->Add(sbSizer1, 1, wxEXPAND, 5);

	this ->SetSizer(bSizer1);
	this->Layout();
	this->Centre(wxBOTH);

	isLogClear=true;
}


void
DlgDbgGLILogRead::updateDlg() 
{

}


std::string &
DlgDbgGLILogRead::getName ()
{
	name = "DlgDbgGLILogRead";
	return(name);
}


void
DlgDbgGLILogRead::eventReceived(const std::string &sender, const std::string &eventType, nau::event_::IEventData *evt)
{
}


void DlgDbgGLILogRead::append(std::string s) {

}


void DlgDbgGLILogRead::clear() {

	m_log->DeleteAllItems();
	isLogClear=true;
}

void DlgDbgGLILogRead::loadNewLogFile(string logfile, int fNumber){
	wxTreeItemId frame;
	string line;
	isNewFrame = true;
	filestream.clear();
	filestream.open(logfile);
	
	if (fNumber >= 0){
		frameNumber=fNumber;
	}

	if (filestream){
	
		getline(filestream,line);
		getline(filestream,line);
		getline(filestream,line);
		getline(filestream,line);
		getline(filestream,line);
		
		while (getline(filestream, line))
		{
			if (isNewFrame){
				frame = m_log->AppendItem(rootnode, "frame "+to_string(frameNumber));
				frameNumber++;
				isNewFrame=false;
			}
			m_log->AppendItem(frame,line);
			if(strcmp(line.substr(0, strlen("wglSwapBuffers")).c_str(), "wglSwapBuffers") == 0){
				isNewFrame=true;
			}
		}

	}
	//if (gliIsLogPerFrame()){
		filestream.close();
	//}
}

void DlgDbgGLILogRead::continueReadLogFile(){
	wxTreeItemId frame;
	string line;

	if (filestream){
		
		while (getline(filestream, line))
		{
			if (isNewFrame){
				frame = m_log->AppendItem(rootnode, "frame "+to_string(frameNumber));
				frameNumber++;
				isNewFrame=false;
			}
			m_log->AppendItem(frame,line);
			if(strcmp(line.substr(0, strlen("wglSwapBuffers")).c_str(), "wglSwapBuffers") == 0){
				isNewFrame=true;
			}
		}
	}

}

void DlgDbgGLILogRead::finishReadLogFile(){
	filestream.close();
}

void DlgDbgGLILogRead::loadLog() {
#ifdef GLINTERCEPTDEBUG
	string logname = gliGetLogName();
	string logfile;
	

	if (isLogClear){
		if (!gliIsLogPerFrame()){
			// Corresponding logfile
			logfile = gliGetLogPath()+logname+".txt";
			rootnode = m_log->AddRoot(logfile);

			//Reads logfile
			loadNewLogFile(logfile, 0);

			//If no logfile was found then leave a message
			if (m_log->GetChildrenCount(rootnode, false) == 0){
				m_log->AppendItem(rootnode, "logfile not found");
			}
		}
		else{
			rootnode = m_log->AddRoot(gliGetLogPath()+string("Frame_*\\")+logname+".txt");
			//Directory searching algorithm source:
			//dirent.h
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir (gliGetLogPath())) != NULL) {
				// Read all files and directories in the directory
				while ((ent = readdir (dir)) != NULL) {
					// Filters directories starting with Frame_* only
					if (ent->d_type == S_IFDIR && strstr(ent->d_name, "Frame_")){
						// Corresponding logfile
						logfile = gliGetLogPath()+string(ent->d_name)+"/"+logname+".txt";

						//Reads logfile
						loadNewLogFile(logfile, atoi(ent->d_name+6));
					}
				}
				closedir (dir);
			}

			//If no logfile was found then leave a message
			if (m_log->GetChildrenCount(rootnode, false) == 0){
				m_log->AppendItem(rootnode, "no related logfiles were found");
			}
		}
		m_log->Expand(rootnode);
		isLogClear=false;
	}
	//else{
	//	if (!gliIsLogPerFrame()){
	//		continueReadLogFile();
	//	}
	//}
#endif
}