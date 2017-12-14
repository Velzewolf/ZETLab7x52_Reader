#pragma once
#include "Reader.h"
#include <vector>
#include <msclr\marshal.h>


ZETLab7x52::Reader::Reader(void)
{
	InitializeComponent();
}

ZETLab7x52::Reader::~Reader()
{
	if (components)
	{
		delete components;
	}
}

void ZETLab7x52::Reader::InitializeComponent(void)
{
	System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Reader::typeid));
	this->ComList = (gcnew System::Windows::Forms::ComboBox());
	this->COMscan_button = (gcnew System::Windows::Forms::Button());
	this->ChannelScan_button = (gcnew System::Windows::Forms::Button());
	this->DeviceList = (gcnew System::Windows::Forms::DataGridView());
	this->DeviceConnect_button = (gcnew System::Windows::Forms::Button());
	this->ConnectedDevices = (gcnew System::Windows::Forms::DataGridView());
	this->Start_button = (gcnew System::Windows::Forms::Button());
	this->Measuring = (gcnew System::ComponentModel::BackgroundWorker());
	this->Stop_button = (gcnew System::Windows::Forms::Button());
	this->JournalCheck = (gcnew System::Windows::Forms::CheckBox());
	this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
	this->WorkToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->ConfigSaveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->ExitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->JournalName = (gcnew System::Windows::Forms::TextBox());
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->DeviceList))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ConnectedDevices))->BeginInit();
	this->menuStrip1->SuspendLayout();
	this->SuspendLayout();
	// 
	// ComList
	// 
	this->ComList->AllowDrop = true;
	this->ComList->FormattingEnabled = true;
	this->ComList->Location = System::Drawing::Point(23, 30);
	this->ComList->Name = L"ComList";
	this->ComList->Size = System::Drawing::Size(121, 21);
	this->ComList->TabIndex = 0;
	// 
	// COMscan_button
	// 
	this->COMscan_button->Location = System::Drawing::Point(189, 30);
	this->COMscan_button->Name = L"COMscan_button";
	this->COMscan_button->Size = System::Drawing::Size(75, 21);
	this->COMscan_button->TabIndex = 2;
	this->COMscan_button->Text = L"Поиск COM";
	this->COMscan_button->UseVisualStyleBackColor = true;
	this->COMscan_button->Click += gcnew System::EventHandler(this, &Reader::COMscan_button_Click);
	// 
	// ChannelScan_button
	// 
	this->ChannelScan_button->Location = System::Drawing::Point(322, 30);
	this->ChannelScan_button->Name = L"ChannelScan_button";
	this->ChannelScan_button->Size = System::Drawing::Size(101, 21);
	this->ChannelScan_button->TabIndex = 4;
	this->ChannelScan_button->Text = L"Поиск устройств";
	this->ChannelScan_button->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
	this->ChannelScan_button->UseVisualStyleBackColor = true;
	this->ChannelScan_button->Click += gcnew System::EventHandler(this, &Reader::ChannelScan_button_Click);
	// 
	// DeviceList
	// 
	this->DeviceList->AllowUserToAddRows = false;
	this->DeviceList->AllowUserToDeleteRows = false;
	this->DeviceList->AllowUserToResizeColumns = false;
	this->DeviceList->AllowUserToResizeRows = false;
	this->DeviceList->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
	this->DeviceList->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
	this->DeviceList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
	this->DeviceList->Enabled = false;
	this->DeviceList->Location = System::Drawing::Point(23, 57);
	this->DeviceList->MaximumSize = System::Drawing::Size(400, 150);
	this->DeviceList->Name = L"DeviceList";
	this->DeviceList->RowHeadersVisible = false;
	this->DeviceList->Size = System::Drawing::Size(400, 150);
	this->DeviceList->TabIndex = 6;
	// 
	// DeviceConnect_button
	// 
	this->DeviceConnect_button->Enabled = false;
	this->DeviceConnect_button->Location = System::Drawing::Point(23, 226);
	this->DeviceConnect_button->Name = L"DeviceConnect_button";
	this->DeviceConnect_button->Size = System::Drawing::Size(105, 23);
	this->DeviceConnect_button->TabIndex = 7;
	this->DeviceConnect_button->Text = L"Подключиться";
	this->DeviceConnect_button->UseVisualStyleBackColor = true;
	this->DeviceConnect_button->Click += gcnew System::EventHandler(this, &Reader::DeviceConnect_button_Click);
	// 
	// ConnectedDevices
	// 
	this->ConnectedDevices->AllowUserToAddRows = false;
	this->ConnectedDevices->AllowUserToDeleteRows = false;
	this->ConnectedDevices->AllowUserToResizeColumns = false;
	this->ConnectedDevices->AllowUserToResizeRows = false;
	this->ConnectedDevices->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
	this->ConnectedDevices->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
	this->ConnectedDevices->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
	this->ConnectedDevices->Enabled = false;
	this->ConnectedDevices->Location = System::Drawing::Point(23, 264);
	this->ConnectedDevices->Name = L"ConnectedDevices";
	this->ConnectedDevices->ReadOnly = true;
	this->ConnectedDevices->RowHeadersVisible = false;
	this->ConnectedDevices->Size = System::Drawing::Size(400, 144);
	this->ConnectedDevices->TabIndex = 8;
	this->ConnectedDevices->Visible = false;
	// 
	// Start_button
	// 
	this->Start_button->Cursor = System::Windows::Forms::Cursors::Default;
	this->Start_button->Location = System::Drawing::Point(23, 429);
	this->Start_button->Name = L"Start_button";
	this->Start_button->Size = System::Drawing::Size(75, 23);
	this->Start_button->TabIndex = 9;
	this->Start_button->Text = L"Запуск";
	this->Start_button->UseVisualStyleBackColor = true;
	this->Start_button->Visible = false;
	this->Start_button->Click += gcnew System::EventHandler(this, &Reader::Start_button_Click);
	// 
	// Measuring
	// 
	this->Measuring->WorkerReportsProgress = true;
	this->Measuring->WorkerSupportsCancellation = true;
	this->Measuring->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Reader::Measuring_DoWork);
	// 
	// Stop_button
	// 
	this->Stop_button->Enabled = false;
	this->Stop_button->Location = System::Drawing::Point(348, 429);
	this->Stop_button->Name = L"Stop_button";
	this->Stop_button->Size = System::Drawing::Size(75, 23);
	this->Stop_button->TabIndex = 10;
	this->Stop_button->Text = L"Стоп";
	this->Stop_button->UseVisualStyleBackColor = true;
	this->Stop_button->Visible = false;
	this->Stop_button->Click += gcnew System::EventHandler(this, &Reader::Stop_button_Click);
	// 
	// JournalCheck
	// 
	this->JournalCheck->AutoSize = true;
	this->JournalCheck->Checked = true;
	this->JournalCheck->CheckState = System::Windows::Forms::CheckState::Checked;
	this->JournalCheck->Location = System::Drawing::Point(177, 434);
	this->JournalCheck->Name = L"JournalCheck";
	this->JournalCheck->Size = System::Drawing::Size(109, 17);
	this->JournalCheck->TabIndex = 11;
	this->JournalCheck->Text = L"Запись журнала";
	this->JournalCheck->UseVisualStyleBackColor = true;
	this->JournalCheck->Visible = false;
	// 
	// menuStrip1
	// 
	this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->WorkToolStripMenuItem });
	this->menuStrip1->Location = System::Drawing::Point(0, 0);
	this->menuStrip1->Name = L"menuStrip1";
	this->menuStrip1->Size = System::Drawing::Size(449, 24);
	this->menuStrip1->TabIndex = 12;
	this->menuStrip1->Text = L"menuStrip1";
	// 
	// WorkToolStripMenuItem
	// 
	this->WorkToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
		this->ConfigSaveToolStripMenuItem,
			this->ExitToolStripMenuItem
	});
	this->WorkToolStripMenuItem->Name = L"WorkToolStripMenuItem";
	this->WorkToolStripMenuItem->Size = System::Drawing::Size(58, 20);
	this->WorkToolStripMenuItem->Text = L"Сессия";
	this->WorkToolStripMenuItem->TextDirection = System::Windows::Forms::ToolStripTextDirection::Horizontal;
	// 
	// ConfigSaveToolStripMenuItem
	// 
	this->ConfigSaveToolStripMenuItem->Name = L"ConfigSaveToolStripMenuItem";
	this->ConfigSaveToolStripMenuItem->Size = System::Drawing::Size(224, 22);
	this->ConfigSaveToolStripMenuItem->Text = L"Сохранение конфигурации";
	this->ConfigSaveToolStripMenuItem->Click += gcnew System::EventHandler(this, &Reader::ConfigSaveToolStripMenuItem_Click);
	// 
	// ExitToolStripMenuItem
	// 
	this->ExitToolStripMenuItem->Name = L"ExitToolStripMenuItem";
	this->ExitToolStripMenuItem->Size = System::Drawing::Size(224, 22);
	this->ExitToolStripMenuItem->Text = L"Выход";
	this->ExitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Reader::ExitToolStripMenuItem_Click);
	// 
	// JournalName
	// 
	this->JournalName->Location = System::Drawing::Point(177, 482);
	this->JournalName->Name = L"JournalName";
	this->JournalName->Size = System::Drawing::Size(100, 20);
	this->JournalName->TabIndex = 13;
	this->JournalName->Visible = false;
	// 
	// Reader
	// 
	this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	this->ClientSize = System::Drawing::Size(449, 514);
	this->Controls->Add(this->JournalName);
	this->Controls->Add(this->JournalCheck);
	this->Controls->Add(this->Stop_button);
	this->Controls->Add(this->Start_button);
	this->Controls->Add(this->ConnectedDevices);
	this->Controls->Add(this->DeviceConnect_button);
	this->Controls->Add(this->DeviceList);
	this->Controls->Add(this->ChannelScan_button);
	this->Controls->Add(this->COMscan_button);
	this->Controls->Add(this->ComList);
	this->Controls->Add(this->menuStrip1);
	this->ForeColor = System::Drawing::SystemColors::ControlText;
	this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"Logo")));
	this->MainMenuStrip = this->menuStrip1;
	this->Name = L"Reader";
	this->Text = L"ZetLAB 7052 Reader";
	this->Load += gcnew System::EventHandler(this, &Reader::Reader_Load);
	this->Shown += gcnew System::EventHandler(this, &Reader::Reader_Shown);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->DeviceList))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ConnectedDevices))->EndInit();
	this->menuStrip1->ResumeLayout(false);
	this->menuStrip1->PerformLayout();
	this->ResumeLayout(false);
	this->PerformLayout();

}

System::Void ZETLab7x52::Reader::Reader_Load(System::Object^  sender, System::EventArgs^  e){
	auto ConnectColumn = gcnew DataGridViewCheckBoxColumn;
	ConnectColumn->HeaderText = "Подключено";
	ConnectColumn->Name = "connect";
	ConnectColumn->CellTemplate = gcnew DataGridViewCheckBoxCell();

	auto NodeColumn = gcnew DataGridViewColumn();
	NodeColumn->HeaderText = "Адрес";
	NodeColumn->ReadOnly = true;
	NodeColumn->Name = "name";
	NodeColumn->CellTemplate = gcnew DataGridViewTextBoxCell();

	auto BaudColumn = gcnew DataGridViewColumn();
	BaudColumn->HeaderText = "Бодрейт";
	BaudColumn->ReadOnly = true;
	BaudColumn->Name = "baud";
	BaudColumn->CellTemplate = gcnew DataGridViewTextBoxCell();

	auto ParityColumn = gcnew DataGridViewColumn();
	ParityColumn->HeaderText = "Бит четности";
	ParityColumn->ReadOnly = true;
	ParityColumn->Name = "parity";
	ParityColumn->CellTemplate = gcnew DataGridViewTextBoxCell();

	DeviceList->Columns->Add(ConnectColumn);
	DeviceList->Columns->Add(NodeColumn);
	DeviceList->Columns->Add(BaudColumn);
	DeviceList->Columns->Add(ParityColumn);

	auto ConnectedNodeColumn = gcnew DataGridViewColumn();
	ConnectedNodeColumn->HeaderText = "Адрес";
	ConnectedNodeColumn->ReadOnly = true;
	ConnectedNodeColumn->Name = "node";
	ConnectedNodeColumn->CellTemplate = gcnew DataGridViewTextBoxCell();

	auto XColumn = gcnew DataGridViewColumn();
	XColumn->HeaderText = "Ось X";
	XColumn->ReadOnly = true;
	XColumn->Name = "x";
	XColumn->CellTemplate = gcnew DataGridViewTextBoxCell();

	auto YColumn = gcnew DataGridViewColumn();
	YColumn->HeaderText = "Ось Y";
	YColumn->ReadOnly = true;
	YColumn->Name = "y";
	YColumn->CellTemplate = gcnew DataGridViewTextBoxCell();

	auto ZColumn = gcnew DataGridViewColumn();
	ZColumn->HeaderText = "Ось Z";
	ZColumn->ReadOnly = true;
	ZColumn->Name = "z";
	ZColumn->CellTemplate = gcnew DataGridViewTextBoxCell();

	ConnectedDevices->Columns->Add(ConnectedNodeColumn);
	ConnectedDevices->Columns->Add(XColumn);
	ConnectedDevices->Columns->Add(YColumn);
	ConnectedDevices->Columns->Add(ZColumn);
}

System::Void ZETLab7x52::Reader::Reader_Shown(System::Object^  sender, System::EventArgs^  e){
	JournalFile = NULL;
	COMScan();
}

System::Void ZETLab7x52::Reader::COMscan_button_Click(System::Object^  sender, System::EventArgs^  e){
	COMScan();
}

System::Void ZETLab7x52::Reader::ChannelScan_button_Click(System::Object^  sender, System::EventArgs^  e){
	ChannelScan();
}

System::Void ZETLab7x52::Reader::DeviceConnect_button_Click(System::Object^  sender, System::EventArgs^  e){
	DeviceConnect();
}

System::Void ZETLab7x52::Reader::Start_button_Click(System::Object^  sender, System::EventArgs^  e){
	Start_button->Enabled = false;
	JournalCheck->Enabled = false;
	Stop_button->Enabled = true;
	ComList->Enabled = false;
	COMscan_button->Enabled = false;
	ChannelScan_button->Enabled = false;
	DeviceConnect_button->Enabled = false;
	if (JournalCheck->Checked){
		if (!PrepareJournal())
			return;
	}
	Measuring->RunWorkerAsync();
}

System::Void ZETLab7x52::Reader::ConfigSaveToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){

}

System::Void ZETLab7x52::Reader::Measuring_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e){
	Measure buffer;
	SYSTEMTIME start, now;
	Act^ Del = gcnew Act(this, &Form::Refresh);
	Port->COMPortReady();
	GetLocalTime(&start);
	if (JournalFile)
		*JournalFile << start.wHour << ':' << start.wMinute << ':' << start.wSecond << ':' << start.wMilliseconds << endl << endl;
	while (1){
		for (int i(0); i < Port->ConnectedDevice->size(); i++){
			GetLocalTime(&now);
			buffer = Port->GetMeasure(Port->ConnectedDevice->at(i));
			ConnectedDevices->Rows[i]->Cells[1]->Value = buffer.X;
			ConnectedDevices->Rows[i]->Cells[2]->Value = buffer.Y;
			ConnectedDevices->Rows[i]->Cells[3]->Value = buffer.Z;
			BeginInvoke(Del);
			//Refresh();
			if (JournalFile)
				*JournalFile << now.wHour << ':' << now.wMinute << ':' << now.wSecond << ':' << now.wMilliseconds << ';' <<
				buffer.X << ';' << buffer.Y << ';' << buffer.Z << ';';
		}
		if (JournalFile)
			*JournalFile << endl;
		if (Measuring->CancellationPending)
		{
			Port->COMPortClose();
			e->Cancel = true;
			Measuring->ReportProgress(0);
			return;
		}
	}
}


System::Void ZETLab7x52::Reader::Stop_button_Click(System::Object^  sender, System::EventArgs^  e){
	if (Measuring->IsBusy)
	{
		Measuring->CancelAsync();
	}
	if (JournalFile)
		JournalFile->close();
	Start_button->Enabled = true;
	JournalCheck->Enabled = true;
	Stop_button->Enabled = false;
	ComList->Enabled = true;
	COMscan_button->Enabled = true;
	ChannelScan_button->Enabled = true;
	DeviceConnect_button->Enabled = true;
}

System::Void ZETLab7x52::Reader::ExitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){
	Application::Exit();
}

void ZETLab7x52::Reader::COMScan(){
	ComList->Items->Clear();
	Port = new COM;
	int ListSize = Port->ComList->size();
	if (!ListSize){
		MessageBox::Show("В системе нет активных COM-портов!", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
	}
	else{
		vector<string> List;
		vector<string>::iterator ListIt = Port->ComList->begin();
		for (int i(0); Port->ComList->end() - ListIt > 0; i++, ListIt++){
			if ((*ListIt)[0] != '\\')
				List.push_back(*ListIt);
			else
				List.push_back(ListIt->substr(4, ListIt->size() - 4));
			ComList->Items->Add(gcnew String(List[i].c_str()));
		}
		ComList->Text = gcnew String(List[0].c_str());
	}
};

void ZETLab7x52::Reader::ChannelScan(){
	DeviceList->Rows->Clear();
	Port->COMName = Port->ComList->at(ComList->SelectedIndex);
	Port->DeviceList = Port->DeviceScan();
	BYTE DeviceNum = Port->DeviceList->size();
	if (!DeviceNum){
		MessageBox::Show("Устройства не обнаружены", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
	}
	else{
		char buf[256];
		vector<Device>::iterator It = Port->DeviceList->begin();
		for (int i(0); i < DeviceNum; It++, i++){
			DeviceList->Rows->Add(false, It->Channel, It->BaudRate, It->Parity);
		}
		DeviceList->Enabled = true;
		DeviceConnect_button->Enabled = true;
		if (DeviceNum == 1)	sprintf(buf, "Обнаружено одно подключенное устройство!");
		else if (DeviceNum < 5) sprintf(buf, "Обнаружено %d подключенных устройства!", DeviceNum);
		else sprintf(buf, "Обнаружено %d подключенных устройств!", DeviceNum);
		MessageBox::Show(gcnew String(buf), "Найдены новые устройства", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
	}
};

void ZETLab7x52::Reader::DeviceConnect(){
	Port->ConnectedDevice->clear();
	ConnectedDevices->Rows->Clear();
	vector<Device>::iterator It = Port->DeviceList->begin();
	for (int i(0); i < DeviceList->RowCount; i++, It++){
		if ((bool)DeviceList->Rows[i]->Cells[0]->Value == true){
			Port->ConnectedDevice->push_back(Device(It->Channel, It->BaudRate, It->Parity));
			ConnectedDevices->Rows->Add(It->Channel, 0, 0, 0);
		}
	}
	if (Port->ConnectedDevice->size()){
		ConnectedDevices->Visible = true;
		ConnectedDevices->Enabled = true;
		Start_button->Visible = true;
		Stop_button->Visible = true;
		JournalCheck->Visible = true;
		JournalName->Visible = true;
		//JournalNameLabel->Visible = true;

	}
};

bool ZETLab7x52::Reader::PrepareJournal(){
	SYSTEMTIME Time;
	struct tm * LocalTime;
	GetLocalTime(&Time);
	char Data[11];
	sprintf(Data, "%04d-%02d-%02d", Time.wYear, Time.wMonth, Time.wDay);
	String^ dir = Application::StartupPath + "\\Журнал" + "\\" + gcnew String(Data);
	if (!IO::Directory::Exists(dir))
		IO::Directory::CreateDirectory(dir);
	sprintf(Data, "%02d-%02d-%02d ", Time.wHour, Time.wMinute, Time.wSecond);
	dir += "\\" + gcnew String(Data) + JournalName->Text + ".txt";
	pin_ptr<const wchar_t> filedir = PtrToStringChars(dir);
	JournalFile = new ofstream;
	JournalFile->open(filedir);
	if (!JournalFile->is_open())
		return false;
	for (int i(0); i < Port->ConnectedDevice->size(); i++)
		*JournalFile << (int)Port->ConnectedDevice->at(i).Channel << ';';
	*JournalFile << endl;
	return true;
};