#ifndef READER_H
#define READER_H

#include "COM.h"
#include <fstream>

namespace ZETLab7052 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	public ref class Reader : public System::Windows::Forms::Form
	{
	public:
		COM* Port;
		ofstream* JournalFile;

		Reader(void);
		void COMScan();
		void ChannelScan();
		void DeviceConnect();
		bool PrepareJournal();
		delegate void Act();

	protected:
		~Reader();
	private: System::Windows::Forms::ComboBox^  ComList;
	private: System::Windows::Forms::Button^  COMscan_button;
	private: System::Windows::Forms::Button^  ChannelScan_button;
	private: System::Windows::Forms::DataGridView^  DeviceList;

	private: System::Windows::Forms::Button^  DeviceConnect_button;
	private: System::Windows::Forms::DataGridView^  ConnectedDevices;
	private: System::Windows::Forms::Button^  Start_button;
	private: System::ComponentModel::BackgroundWorker^  Measuring;
	private: System::Windows::Forms::Button^  Stop_button;
	private: System::Windows::Forms::CheckBox^  JournalCheck;

	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  WorkToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ConfigSaveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ExitToolStripMenuItem;
	private: System::Windows::Forms::TextBox^  JournalName;





	private: System::ComponentModel::Container ^components;
#pragma region Windows Form Designer generated code
		void InitializeComponent(void);
#pragma endregion
	private: System::Void Reader_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Reader_Shown(System::Object^  sender, System::EventArgs^  e);
	private: System::Void COMscan_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ChannelScan_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void DeviceConnect_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Start_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Measuring_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e);
	private: System::Void Stop_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ExitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ConfigSaveToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	//private: System::Void Measuring_ProgressChanged(System::Object^  sender, System::ComponentModel::ProgressChangedEventArgs^  e);
	};
}

#endif