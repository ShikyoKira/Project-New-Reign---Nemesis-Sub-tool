#pragma once

#include <string>
#include <msclr\marshal_cppstd.h>

namespace ProjectNewReignNemesis {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GUIForm
	/// </summary>
	public ref class GUIForm : public System::Windows::Forms::Form
	{
	public:
		GUIForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GUIForm()
		{
			if (components)
			{
				delete components;
			}
		}



	private: System::Windows::Forms::PictureBox^  panel4;
	private: System::Windows::Forms::PictureBox^  panel1;



	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GUIForm::typeid));
			this->panel4 = (gcnew System::Windows::Forms::PictureBox());
			this->panel1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->panel4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->panel1))->BeginInit();
			this->SuspendLayout();
			// 
			// panel4
			// 
			this->panel4->AllowDrop = true;
			this->panel4->BackColor = System::Drawing::SystemColors::Control;
			this->panel4->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->panel4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel4->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"panel4.Image")));
			this->panel4->Location = System::Drawing::Point(276, 12);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(250, 250);
			this->panel4->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->panel4->TabIndex = 2;
			this->panel4->TabStop = false;
			// 
			// panel1
			// 
			this->panel1->AllowDrop = true;
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"panel1.Image")));
			this->panel1->Location = System::Drawing::Point(12, 12);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(250, 250);
			this->panel1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->panel1->TabIndex = 0;
			this->panel1->TabStop = false;
			this->panel1->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &GUIForm::panel1_DragDrop);
			this->panel1->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &GUIForm::panel1_DragEnter);
			// 
			// GUIForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(538, 274);
			this->Controls->Add(this->panel4);
			this->Controls->Add(this->panel1);
			this->Name = L"GUIForm";
			this->Text = L"GUIForm";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->panel4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->panel1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void panel1_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
		if (e->Data->GetDataPresent(DataFormats::FileDrop))
		{
			e->Effect = DragDropEffects::Copy;
		}
		else
		{
			e->Effect = DragDropEffects::None;
		}
	}
	private: System::Void panel1_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
		array<String^>^ files = safe_cast<array<String^>^>(e->Data->GetData(DataFormats::FileDrop));
		System::Windows::Forms::PictureBox^  pictureBox1;
		bool out = false;

		for each (String^ filename in files)
		{
			String^ ext = System::IO::Path::GetExtension(filename)->ToLower();
			std::string file = msclr::interop::marshal_as<std::string>(filename);

			if (ext == ".xml")
			{
				if (!out)
				{
					pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
					(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(pictureBox1))->BeginInit();
					// 
					// pictureBox1
					// 
					pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>((gcnew System::ComponentModel::ComponentResourceManager(GUIForm::typeid))->GetObject(L"pictureBox1.Image")));
					pictureBox1->Location = System::Drawing::Point(287, 136);
					pictureBox1->Name = L"pictureBox1";
					pictureBox1->Size = System::Drawing::Size(239, 210);
					pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
					pictureBox1->TabIndex = 3;
					pictureBox1->TabStop = false;
					Controls->Add(pictureBox1);
					(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(pictureBox1))->EndInit();
					out = true;
				}

				MessageBox::Show(filename);
			}

		}

		if (pictureBox1)
			delete pictureBox1;
		out = false;
	}

};
}
