#include <iostream>
#include <direct.h>
#include <time.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>
#include <random>
#include <windows.h>

using namespace std;

ofstream glb__writer;
ifstream glb__reader;

string working_path()
{
	char temp[261];
	return (_getcwd(temp, sizeof(temp)) ? string(temp) : string(""));
}

struct BANK
{
	int size = 0;
	string keys[10];
	string hints[10];
};

struct PLAYER
{
	string name = "";
	int score = 50;

	BANK played_e, played_m, played_h;                      // Chứa các từ đã chơi cho mỗi mức độ
};

struct PLAYER_LIST
{
	int size = 0;
	PLAYER players[50];
};

string  glb__dir_bank_e = working_path() + "\\saves\\banks\\de",               // File câu hỏi mức độ dễ
		glb__dir_bank_m = working_path() + "\\saves\\banks\\tb",               //                     trung bình
		glb__dir_bank_h = working_path() + "\\saves\\banks\\kho",              //                     khó
		glb__dir_scrbrd = working_path() + "\\saves\\scoreboard",              // File scoreboard
		glb__dir_players_folder = working_path() + "\\saves\\players\\",           // Folder dữ liệu người chơi

		glb__psswd,                                              // Mật khẩu người chơi
		glb__key, glb__hint,                                     // Từ khóa được chọn
		glb__input;                                              // Kết quả người chơi nhập vào

int glb__round_pnt,                                              // Số điểm có được sau vòng chơi
	glb__dif,                                                    // Độ khó (1: Easy    2: Medium    3: Hard)

	glb__pnt,                                                    // Điểm nếu đoán đúng và không lật
	glb__pnt_a,                                                  // Điểm bị trừ khi lật các kí tự cho phép
	glb__pnt_b,                                                  // Điểm bị trừ khi lật 1 kí tự cấm
	glb__pnt_w,                                                  // Điểm bị trừ khi đoán sai
	glb__allowed,                                                // Số kí tự cho phép được mở
	glb__banned,                                                 // Số kí tự cấm được phép mở
	glb__used_banned = 0,                                        // Số lần mở kí tự cấm
	glb__duration;                                               // Dùng cho Timer()

BANK glb__bank_e, glb__bank_m, glb__bank_h,                      // Ngân hàng từ khóa
	 glb__bank_playing,                                          // Ngân hàng được chọn dựa trên độ khó
	 glb__played;                                                // Các từ khóa đã chơi của độ khó đã chọn

PLAYER_LIST glb__scrbrd;                                         // Scoreboard

PLAYER glb__player;                                              // Người chơi

string glb__revealed[30];                                        // Các kí tự đã được mở


string Encryptor(string);

bool ReadPlayerData(string);

void PrintLogo(), Menu(), Rule(), Initializer(bool),
	 ReadKeys(), ReadScrbrdData(),
	 UpdateScrbrdData(), UpdatePlayerData(string), UpdateSessionVars(),
	 CreateNewPlayerProfile(string), AddNewPlayer(PLAYER&, bool),
	 AddPlayerToScrbrd(PLAYER), PrintScrbrd(bool), SortScrbrd(char),
	 PlayerSelector(bool), Printer(), Timer(int, char),

	 AskToReveal(), Revealer(), KeyPicker(),

	 AnswerResult(), SessionResult();




int main()
{
	ReadKeys();



	PrintLogo();
	cout << "\n"
		<< "\n" << "=================================================================="
		<< "\n" << "||   CHAO MUNG BAN DEN VOI GAME MYWORDS  (BY ITEC DEVELOPERS)   ||"
		<< "\n" << "==================================================================";

	cout << "\n" << "Co " << glb__bank_e.size << " o chu De, " << glb__bank_m.size << " o chu Trung binh, " << glb__bank_h.size << " o chu Kho da duoc load";


	Initializer(false);


	return 0;
}



string Encryptor(string inp)
{
	char encryption_key[3] = { '\3', '\1', '\5' };
	int j = 0;

	string output = inp;

	for (int i = 0; i < inp.size(); i++)
	{
		output[i] = inp[i] ^ encryption_key[j];
		j++;

		if (j == 3)
			j = 0;
	}

	return output;
}


void PrintLogo()
{
	cout << "\n" << "||\\    /|| ||    ||  ||        ||  /|||\\  |||||\\  |||||\\   /||||| "
		<< "\n" << "||\\\\  //||  \\\\  //   ||   /\\   || ||   || ||   || ||   || ||      "
		<< "\n" << "|| \\\\// ||   \\\\//    ||  //\\\\  || ||   || |||||/  ||   ||  \\|||\\  "
		<< "\n" << "||  \\/  ||    ||      \\\\//  \\\\//  ||   || || \\\\\\  ||   ||      || "
		<< "\n" << "||      ||    ||       \\/    \\/    \\|||/  ||  \\\\\\ |||||/  |||||/  ";
}

void Menu()
{
	string menu_options[] = { "", "", "", "", ""}, picked_menu_option = "";
	int menu_index = 1, player_option = 0, num_of_options = 0;

	cout << "\n\n" << "------------------------------ Menu ------------------------------";

	if (glb__scrbrd.size == 0)
	{
		cout << "\n" << menu_index << ". Tao ho so moi";
		menu_options[menu_index] = "new";
		menu_index++;
		cout << "\n" << menu_index << ". Luat choi";
		menu_options[menu_index] = "rule";
		menu_index++;
		cout << "\n" << menu_index << ". Thoat";
		menu_options[menu_index] = "exit";

		num_of_options = menu_index;
	}

	else
	{
		cout << "\n" << menu_index << ". Tao ho so moi";
		menu_options[menu_index] = "new";
		menu_index++;
		cout << "\n" << menu_index << ". Tiep tuc choi";
		menu_options[menu_index] = "cont";
		menu_index++;
		cout << "\n" << menu_index << ". Luat choi";
		menu_options[menu_index] = "rule";
		menu_index++;
		cout << "\n" << menu_index << ". Thoat";
		menu_options[menu_index] = "exit";

		num_of_options = menu_index;
	}


	cout << "\n" << "\n" << "Nhap ma so: ";
	cin >> player_option;

	while (player_option < 1 || num_of_options < player_option)
	{
		cout << "Khong hop le. Vui long nhap lai: ";
		cin >> player_option;
	}

	picked_menu_option = menu_options[player_option];


	if (picked_menu_option == "new")
		AddNewPlayer(glb__player, true);

	else if (picked_menu_option == "cont")
	{
		PlayerSelector(true);
		UpdateScrbrdData();
	}

	else if (picked_menu_option == "rule")
		Rule();

	else if (picked_menu_option == "exit")
	{
		cout << "\n" << "Chao ban, hen gap lan sau!" << "\n";
		exit(0);
	}
}

void Rule()
{
	system("cls");
	PrintLogo();

	cout << "\n\nCach tinh diem:";

	cout << "\n\n"	<< "  ________________________________________________________________________________________________________________________ \n"
					<< " | Cap do | Thoi gian | So luong | So ki tu | So ki tu |  Diem bi tru  | Diem kiem duoc neu |  Diem kiem duoc  | Diem tru |\n"
					<< " |        | doan (s)  |  ki tu   | Cho phep |    Cam   | moi ki tu cam | khong lat Cho phep | neu lat Cho phep | doan sai |\n"
					<< "  ------------------------------------------------------------------------------------------------------------------------ \n"
					<< " |   De   |    10     |  4 - 8   |     1    |     1    |       3       |         10         |         5        |     2    |\n"
					<< "  ------------------------------------------------------------------------------------------------------------------------ \n"
					<< " |   TB   |    15     |  9 - 15  |     2    |     1    |       5       |         15         |         8        |     4    |\n"
					<< "  ------------------------------------------------------------------------------------------------------------------------ \n"
					<< " |   Kho  |    20     | 15 - 30  |     3    |     2    |       8       |         20         |         10       |     6    |\n"
					<< "  ------------------------------------------------------------------------------------------------------------------------ \n";

	cout << "\n\nNhap phim bat ki de quay lai menu: ";
	string temp;
	cin >> temp;
	system("cls");

	PrintLogo();
	Menu();
}


void Initializer(bool recoursed)
{
	for (int i = 0; i < 30; i++)
		glb__revealed[i] = "\0";

	ReadScrbrdData();
	SortScrbrd('d');

	if (glb__scrbrd.size == -1)
	{
		cout << "\n" << "\n" << "ERROR: Khong tim thay file scoreboard"
			<< "\n"
			<< "\n" << "Vui long khac phuc loi bang cach:"
			<< "\n" << "\t 1. Tao file ten 'scoreboard' trong folder 'saves'"
			<< "\n" << "\t 2. Mo file vua tao bang Notepad va nhap '0' (so khong). Luu file."
			<< "\n" << "\t 3. Xoa tat ca cac file trong folder 'saves\\players'"
			<< "\n"
			<< "\n" << "Sau khi hoan thanh cac buoc tren, ban co the chay lai chuong trinh."
			<< "\n" << "Chao ban, hen gap lan sau!" << "\n";

		exit(0);
	}

	else if (recoursed == false)
		Menu();

	system("cls");

	PrintLogo();
	cout << "\n\nTen nguoi choi: " << glb__player.name
		<< "\n" << "So diem hien tai: " << glb__player.score;

	cout << "\n" << "\n" << "Chon do kho (1: De   2: Trung binh   3: Kho): ";
	cin >> glb__dif;

	while (glb__dif < 1 || 3 < glb__dif)
	{
		cout << "Khong hop le. Vui long nhap lai: ";
		cin >> glb__dif;
	}

	switch (glb__dif)
	{
		case 1:
		{
			glb__bank_playing.size = glb__bank_e.size;
			for (int i = 0; i < glb__bank_playing.size; i++)
			{
				glb__bank_playing.keys[i] = glb__bank_e.keys[i];
				glb__bank_playing.hints[i] = glb__bank_e.hints[i];

				glb__played.keys[i] = glb__player.played_e.keys[i];
				glb__played.hints[i] = glb__player.played_e.hints[i];
			}

			break;
		}

		case 2:
		{
			glb__bank_playing.size = glb__bank_m.size;
			for (int i = 0; i < glb__bank_playing.size; i++)
			{
				glb__bank_playing.keys[i] = glb__bank_m.keys[i];
				glb__bank_playing.hints[i] = glb__bank_m.hints[i];

				glb__played.keys[i] = glb__player.played_m.keys[i];
				glb__played.hints[i] = glb__player.played_m.hints[i];
			}

			break;
		}

		case 3:
		{
			glb__bank_playing.size = glb__bank_h.size;
			for (int i = 0; i < glb__bank_playing.size; i++)
			{
				glb__bank_playing.keys[i] = glb__bank_h.keys[i];
				glb__bank_playing.hints[i] = glb__bank_h.hints[i];

				glb__played.keys[i] = glb__player.played_h.keys[i];
				glb__played.hints[i] = glb__player.played_h.hints[i];
			}

			break;
		}
	}

	UpdateSessionVars();

	KeyPicker();
	Printer();
	AskToReveal();


	string option;
	cout << "\n\nBan co muon choi tiep? (Y/N): ";
	cin >> option;

	while (option != "y" && option != "Y" && option != "n" && option != "N")
	{
		cout << "\nKhong hop le. Vui long nhap lai: ";
		cin >> option;
	}

	if (option == "Y" || option == "y")
	{
		UpdateScrbrdData();
		UpdatePlayerData(glb__player.name);
		Initializer(true);
	}

	else
		SessionResult();
}


void ReadKeys()
{
	string dir;


	dir = glb__dir_bank_e;

	glb__reader.open(dir);

	int counter = 0, elem_counter = 0;
	string content = "", combined = "";

	if (glb__reader.is_open())
	{
		while (getline(glb__reader, content))
		{
			if (counter == 0)
			{
				glb__bank_e.size = stoi(content);
				counter++;
			}

			else if (counter == 1)
			{
				glb__bank_e.keys[elem_counter] = Encryptor(content);
				counter++;
			}

			else if (counter == 2)
			{
				glb__bank_e.hints[elem_counter] = Encryptor(content);
				counter = 1;
				elem_counter++;
			}
		}

		glb__reader.close();
	}

	else
		glb__bank_e.size = -1;


	dir = glb__dir_bank_m;

	glb__reader.open(dir);

	counter = 0, elem_counter = 0;

	if (glb__reader.is_open())
	{
		while (getline(glb__reader, content))
		{
			if (counter == 0)
			{
				glb__bank_m.size = stoi(content);
				counter++;
			}

			else if (counter == 1)
			{
				glb__bank_m.keys[elem_counter] = Encryptor(content);
				counter++;
			}

			else if (counter == 2)
			{
				glb__bank_m.hints[elem_counter] = Encryptor(content);
				counter = 1;
				elem_counter++;
			}
		}

		glb__reader.close();
	}

	else
		glb__bank_m.size = -1;


	dir = glb__dir_bank_h;

	glb__reader.open(dir);

	counter = 0, elem_counter = 0;

	if (glb__reader.is_open())
	{
		while (getline(glb__reader, content))
		{
			if (counter == 0)
			{
				glb__bank_h.size = stoi(content);
				counter++;
			}

			else if (counter == 1)
			{
				glb__bank_h.keys[elem_counter] = Encryptor(content);
				counter++;
			}

			else if (counter == 2)
			{
				glb__bank_h.hints[elem_counter] = Encryptor(content);
				counter = 1;
				elem_counter++;
			}
		}

		glb__reader.close();
	}

	else
		glb__bank_h.size = -1;
}

void ReadScrbrdData()
{
	glb__reader.open(glb__dir_scrbrd);

	int counter = 0, elem_counter = 0;
	string content, combined = "", secret;

	if (glb__reader.is_open())
	{
		while (getline(glb__reader, content))
		{
			if (counter == 0)
			{
				glb__scrbrd.size = stoi(content);

				if (glb__scrbrd.size == 0)
					break;

				else counter++;
			}

			else if (counter == 1)
			{
				secret = content;
				counter++;
			}

			else if (counter == 2)
			{
				glb__scrbrd.players[elem_counter].name = content;
				counter++;
			}

			else if (counter == 3)
			{
				glb__scrbrd.players[elem_counter].score = stoi(content);
				counter = 2;
				elem_counter++;
			}
		}

		glb__reader.close();

		for (int i = 0; i < glb__scrbrd.size; i++)
			combined = combined + glb__scrbrd.players[i].name + to_string(glb__scrbrd.players[i].score);

		if (combined != Encryptor(secret))
		{
			cout << endl << "Du lieu bang xep hang da bi thay doi thu cong. "
				<< "\n" << "Vui long khac phuc loi bang cach:"
				<< "\n" << "\t 1. Tao file ten 'scoreboard' trong folder 'saves'"
				<< "\n" << "\t 2. Mo file vua tao bang Notepad va nhap '0' (so khong). Luu file."
				<< "\n" << "\t 3. Xoa tat ca cac file trong folder 'saves\\players'"
				<< "\n"
				<< "\n" << "Sau khi hoan thanh cac buoc tren, ban co the chay lai chuong trinh."
				<< "\n" << "Chuong trinh se tu dong thoat. Chao ban, hen gap lan sau!";
			exit(0);
		}
	}

	else
		glb__scrbrd.size = -1;
}

bool ReadPlayerData(string inp_name)
{
	glb__player.name = inp_name;

	for (int i = 0; i < glb__scrbrd.size; i++)
	{
		if (glb__scrbrd.players[i].name == inp_name)
			glb__player.score = glb__scrbrd.players[i].score;
	}


	string dir = glb__dir_players_folder + inp_name + "";

	glb__reader.open(dir);

	int counter = 0, elem_counter = 0;
	string content;

	if (glb__reader.is_open())
	{
		while (getline(glb__reader, content))
		{
			if (counter == 0)
			{
				if (Encryptor(content) != glb__psswd)
				{
					glb__reader.close();
					return false;
				}

				else
					counter++;
			}

			else if (counter == 1)
			{
				glb__player.played_e.size = stoi(content);

				if (glb__player.played_e.size > 0)
					counter += 1;
				else
					counter += 2;
			}

			else if (counter == 2)
			{
				glb__player.played_e.keys[elem_counter] = content;
				elem_counter++;

				if (elem_counter == glb__player.played_e.size)
				{
					counter++;
					elem_counter = 0;
				}
			}


			else if (counter == 3)
			{
				glb__player.played_m.size = stoi(content);

				if (glb__player.played_m.size > 0)
					counter += 1;
				else
					counter += 2;
			}

			else if (counter == 4)
			{
				glb__player.played_m.keys[elem_counter] = content;
				elem_counter++;

				if (elem_counter == glb__player.played_m.size)
				{
					counter++;
					elem_counter = 0;
				}
			}


			else if (counter == 5)
			{
				glb__player.played_h.size = stoi(content);

				if (glb__player.played_h.size > 0)
					counter += 1;
				else
					break;
			}

			else if (counter == 6)
			{
				glb__player.played_h.keys[elem_counter] = content;
				elem_counter++;

				if (elem_counter == glb__player.played_h.size)
					break;
			}
		}

		glb__reader.close();
	}

	else
		glb__scrbrd.size = -1;


	return true;
}


void UpdateScrbrdData()
{
	SortScrbrd('d');
	string combined = "", secret = "";

	for (int i = 0; i < glb__scrbrd.size; i++)
	{
		if (glb__scrbrd.players[i].name == glb__player.name)
			glb__scrbrd.players[i].score = glb__player.score;

		combined = combined + glb__scrbrd.players[i].name + to_string(glb__scrbrd.players[i].score);
	}

	secret = Encryptor(combined);

	glb__writer.open(glb__dir_scrbrd);

	glb__writer << glb__scrbrd.size << "\n" << secret;

	for (int i = 0; i < glb__scrbrd.size; i++)
		glb__writer << "\n" << glb__scrbrd.players[i].name << "\n" << glb__scrbrd.players[i].score;

	glb__writer.close();
}

void UpdatePlayerData(string inp_name)
{
	glb__writer.open(glb__dir_players_folder + inp_name);

	glb__writer << Encryptor(glb__psswd);

	int size;

	(glb__dif == 1) ? size = glb__played.size : size = glb__player.played_e.size;

	glb__writer << "\n" << size;

	for (int i = 0; i < size; i++)
		(glb__dif == 1) ? glb__writer << "\n" << glb__played.keys[i] : glb__writer << "\n" << glb__player.played_e.keys[i];


	(glb__dif == 2) ? size = glb__played.size : size = glb__player.played_m.size;

	glb__writer << "\n" << size;

	for (int i = 0; i < size; i++)
		(glb__dif == 2) ? glb__writer << "\n" << glb__played.keys[i] : glb__writer << "\n" << glb__player.played_m.keys[i];


	(glb__dif == 3) ? size = glb__played.size : size = glb__player.played_h.size;

	glb__writer << "\n" << size;

	for (int i = 0; i < size; i++)
		(glb__dif == 3) ? glb__writer << "\n" << glb__played.keys[i] : glb__writer << "\n" << glb__player.played_h.keys[i];


	glb__writer.close();
}

void UpdateSessionVars()
{
	ReadPlayerData(glb__player.name);
	glb__player.score = glb__player.score;


	switch (glb__dif)
	{
		case 1:
		{
			glb__allowed = 1;
			glb__banned = 1;
			glb__pnt = 10;
			glb__round_pnt = glb__pnt;
			glb__pnt_a = 5;
			glb__pnt_b = 3;
			glb__pnt_w = 2;
			glb__duration = 10;

			break;
		}

		case 2:
		{
			glb__allowed = 2;
			glb__banned = 1;
			glb__pnt = 15;
			glb__round_pnt = glb__pnt;
			glb__pnt_a = 7;
			glb__pnt_b = 5;
			glb__pnt_w = 4;
			glb__duration = 15;

			break;
		}

		case 3:
		{
			glb__allowed = 3;
			glb__banned = 2;
			glb__pnt = 20;
			glb__round_pnt = glb__pnt;
			glb__pnt_a = 10;
			glb__pnt_b = 8;
			glb__pnt_w = 6;
			glb__duration = 20;

			break;
		}
	}
}


void CreateNewPlayerProfile(string name)
{
	string new_dir = glb__dir_players_folder + name + "";

	ofstream new_file(new_dir);

	PLAYER new_player;
	new_player.name = name;

	UpdatePlayerData(new_player.name);
}

void AddNewPlayer(PLAYER &player, bool ask_name)
{
	bool called_external = false;
	if (ask_name)
	{
		cin.clear(); cin.ignore();
		cout << "\n" << "\n" << "Vui long nhap ten cua ban (co phan biet hoa/thuong). Nhap ^ de quay lai menu: ";
		getline(cin, player.name);

		if (player.name == "^")
		{
			called_external = true;

			system("cls");
			PrintLogo();
			Menu();
		}
	}

	if (called_external == false)
	{
		bool existed = false;
		int cur_scrbrd_size = glb__scrbrd.size;

		for (int i = 0; i < cur_scrbrd_size; i++)
		{
			if (glb__scrbrd.players[i].name == player.name)
			{
				existed = true;
				string option;

				cout << "\n" << "Ten nguoi choi da co trong Scoreboard. Ban co muon tao moi? (Y/N): ";
				cin >> option;

				while (option != "y" && option != "Y" && option != "n" && option != "N")
				{
					cout << "Khong hop le. Vui long nhap lai: ";
					cin >> option;
				}

				if (option == "y" || option == "Y")
				{
					cin.clear(); cin.ignore();
					cout << "Mat khau (nhap 'quit' de thoat): ";
					getline(cin, glb__psswd);

					if (glb__psswd == "quit")
					{
						called_external = true;

						system("cls");
						PrintLogo();
						Menu();
					}

					if (called_external == false)
					{
						while (ReadPlayerData(player.name) == false)
						{
							cout << "\n" << "Mat khau khong chinh xac" << "\n" << "Vui long nhap lai (nhap 'quit' de nhap lai ten): ";
							getline(cin, glb__psswd);

							if (glb__psswd == "quit")
							{
								called_external = true;

								system("cls");
								PrintLogo();
								Menu();
							}
						}

						if (called_external == false)
						{
							cout << "\n" << "Mat khau chinh xac. ";

							PLAYER temp;
							temp.name = player.name;
							player = temp;

							glb__scrbrd.size -= 1;

							AddNewPlayer(player, false);
						}
					}

					break;
				}

				else
				{
					cin.clear(); cin.ignore();
					cout << "Mat khau (nhap 'quit' de nhap lai ten): ";
					getline(cin, glb__psswd);

					if (glb__psswd == "quit")
					{
						called_external = true;

						system("cls");
						PrintLogo();
						Menu();
					}

					if (called_external == false)
					{
						while (ReadPlayerData(player.name) == false)
						{
							cout << "\n" << "Mat khau khong chinh xac" << "\n" << "Vui long nhap lai (nhap 'quit' de nhap lai ten): ";
							getline(cin, glb__psswd);

							if (glb__psswd == "quit")
							{
								called_external = true;

								system("cls");
								PrintLogo();
								Menu();
							}

						}

						if (called_external == false)
						{
							UpdateScrbrdData();

							cout << "\n" << "Mat khau chinh xac. Du lieu cua nguoi choi " << player.name << " duoc doc thanh cong" << "\n";
						}
					}

					break;
				}
			}
		}

		if (existed == false)
		{
			cout << "Vui long tao mat khau (Luu y: Mat khau khi mat se khong the khoi phuc): ";
			cin.clear();
			getline(cin, glb__psswd);

			while (glb__psswd == "quit")
			{
				cout << "Mat khau khong hop le. Vui long chon mat khau khac: ";
				cin.clear();
				getline(cin, glb__psswd);
			}

			CreateNewPlayerProfile(player.name);
			AddPlayerToScrbrd(player);
			UpdateScrbrdData();

			cout << "Da luu du lieu cua nguoi choi " << player.name << "\n";
		}
	}
}

void AddPlayerToScrbrd(PLAYER player)
{
	glb__scrbrd.players[glb__scrbrd.size] = player;
	glb__scrbrd.size += 1;
}


void PrintScrbrd(bool with_scr)
{
	ReadScrbrdData();

	for (int i = 0; i < glb__scrbrd.size; i++)
	{
		cout << "\n" << i + 1 << ". " << glb__scrbrd.players[i].name;

		if (with_scr)
			cout << "\t\t\t" << glb__scrbrd.players[i].score << " diem" << "\n";
	}
}

void SortScrbrd(char mode)
{
	PLAYER temp;

	switch (mode)
	{
	case 'i':                                                       // Increment
	{
		for (int i = 0; i < (glb__scrbrd.size - 1); i++)
		{
			for (int j = i + 1; j < glb__scrbrd.size; j++)
			{
				if (glb__scrbrd.players[j].score > glb__scrbrd.players[j + 1].score)
				{
					temp = glb__scrbrd.players[j];

					glb__scrbrd.players[j] = glb__scrbrd.players[j + 1];
					glb__scrbrd.players[j + 1] = temp;
				}
			}
		}

		break;
	}

	case 'd':                                                       // Decrement
	{
		for (int i = 0; i < (glb__scrbrd.size - 1); i++)
		{
			for (int j = i + 1; j < glb__scrbrd.size; j++)
			{
				if (glb__scrbrd.players[i].score < glb__scrbrd.players[j].score)
				{
					temp = glb__scrbrd.players[i];

					glb__scrbrd.players[i] = glb__scrbrd.players[j];
					glb__scrbrd.players[j] = temp;
				}
			}
		}

		break;
	}
	}
}


void PlayerSelector(bool prompt)
{
	int id = 0, called_external = false;

	if (prompt)
	{
		cout << "\n" << "===== Danh sach nguoi choi da luu =====" << "\n";
		PrintScrbrd(true);
	}

	cout << "\n" << "\n" << "Nhap so thu tu cua ban (0 de quay lai menu): ";
	cin >> id;

	while (id < 0 || glb__scrbrd.size < id)
	{
		cout << "Khong hop le. Vui long nhap lai: ";
		cin >> id;
	}

	if (id == 0)
	{
		called_external = true;

		system("cls");
		PrintLogo();
		Menu();
	}

	if (called_external == false)
	{
		cin.clear(); cin.ignore();
		cout << "\n" << "Mat khau: ";
		getline(cin, glb__psswd);

		while (ReadPlayerData(glb__scrbrd.players[id - 1].name) == false)
		{
			cout << "\n" << "Mat khau khong chinh xac" << "\n" << "Vui long nhap lai hoac nhan Enter de chon nguoi choi khac: ";
			getline(cin, glb__psswd);

			if (glb__psswd == "")
			{
				cout << "\n" << "\n" << "Nhap so thu tu cua ban (Nhap 0 de quay lai menu): ";
				cin >> id;

				while (id < 0 || glb__scrbrd.size < id)
				{
					cout << "Khong hop le. Vui long nhap lai: ";
					cin >> id;
				}

				if (id == 0)
				{
					called_external = true;

					system("cls");
					PrintLogo();
					Menu();
				}

				if (called_external == false)
				{
					cin.clear(); cin.ignore();
					cout << "\n" << "Mat khau: ";
					getline(cin, glb__psswd);
					continue;
				}
			}
		}

		if (called_external == false)
		{
			ReadPlayerData(glb__player.name);
			cout << "\n" << "Mat khau chinh xac. Du lieu cua nguoi choi " << glb__player.name << " duoc doc thanh cong" << "\n";
		}
	}
}




void KeyPicker()
{
	bool found = false;

	for (int i = 0; i < glb__bank_playing.size; i++)
	{
		srand(time(NULL));
		int n = rand() % (glb__bank_playing.size - 1);

		string temp = glb__bank_playing.keys[n];

		for (int j = 0; j < glb__played.size; j++)
		{
			if (temp == glb__played.keys[j])
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			Sleep(100);
			continue;
		}

		else
		{
			glb__key = glb__bank_playing.keys[n];
			glb__hint = glb__bank_playing.hints[n];
			break;
		}
	}

}


void Printer()
{
	system("cls");
	PrintLogo();
	cout << "\n\nTen nguoi choi: " << glb__player.name
		<< "\n" << "So diem hien tai: " << glb__player.score;

	cout << "\n\n";
	cout << glb__hint << ": ";
	for (int j = 0; j < glb__key.length(); j++)
	{
		if ((glb__revealed[j]) == "\0")
			cout << "_";
		else
			cout << glb__revealed[j];
	}
	cout << "\n";
}



void Timer(int dur, char mode)
{
	switch (mode)
	{
		case 'g':                                           // Guess
		{
			Sleep(1000);
			while (dur >= 0)
			{
				cout << "\r";
				if (dur < 10)
					cout << "0";
				cout << dur << flush;
				Sleep(1000);
				dur--;
			}

			break;
		}

		case 'a':                                           // Answer
		{
			string inp;
			time_t start = time(0);

			cout << "\nBan co " << dur << " giay de nhap dap an. Dap an se duoc viet lien, khong dau, khong phan biet hoa/thuong\n";
			cout << "Vui long nhap dap an: ";
			cin >> glb__input;
		

			double time_taken = difftime(time(0), start);

			if (time_taken > dur)
				glb__input = "TIMEOUT";

			else
				cout << "Ban da nhap: " << glb__input << "\n";
		

			break;
		}
	}
}





void AskToReveal()
{
	string option;

	if (glb__allowed > 0)
	{
		cout << "\nBan co muon mo cac ki tu cho phep? Se co " << glb__allowed << " ki tu duoc mo nhung ban se chi nhan duoc " << glb__pnt - glb__pnt_a << "/" << glb__pnt << " diem (Y/N): ";
		cin >> option;

		while (option != "y" && option != "Y" && option != "n" && option != "N")
		{
			cout << "\nKhong hop le. Vui long nhap lai: ";
			cin >> option;
		}

		if (option == "Y" || option == "y")
		{
			glb__round_pnt -= glb__pnt_a;
			for (int i = 0; i < glb__allowed; i++)
			{
				Revealer();
				Sleep(100);
			}
			glb__allowed = 0;

			system("cls");
			Printer();
			AskToReveal();
		}

		else
		{
			system("cls");
			Printer();
			cout << "\n\nBan vui long doan trong " << glb__duration << " giay\n";
			Timer(glb__duration, 'g');
			Timer(glb__duration - 5, 'a');
			AnswerResult();
		}
	}

	else if (glb__banned > 0)
	{
		cout << "\nBan co muon mo ki tu cam? Ban con " << glb__banned << " luot, moi luot ban se bi tru " << glb__pnt_b << " diem (Y/N): ";
		cin >> option;

		while (option != "y" && option != "Y" && option != "n" && option != "N")
		{
			cout << "\nKhong hop le. Vui long nhap lai: ";
			cin >> option;
		}

		while (glb__banned > 0 && (option == "Y" || option == "y"))
		{
			glb__round_pnt -= glb__pnt_b;
			Revealer();
			glb__banned--;

			system("cls");
			Printer();

			if (glb__banned > 0)
			{
				cout << "\nBan co muon mo them ki tu cam? Ban con " << glb__banned << " luot, moi luot ban se bi tru " << glb__pnt_b << " diem (Y/N): ";
				cin >> option;
			}

			else
				break;
		}

		if (option == "N" || option == "n")
		{
			system("cls");
			Printer();
			cout << "\n\nBan vui long doan trong " << glb__duration << " giay\n";
			Timer(glb__duration, 'g');
			Timer(glb__duration - 5, 'a');
			AnswerResult();
		}

		else if (glb__banned == 0)
		{
			system("cls");
			Printer();
			cout << "\nBan da het luot mo ki tu cam. Vui long doan trong " << glb__duration << " giay\n";
			Timer(glb__duration, 'g');
			Timer(glb__duration, 'a');
			AnswerResult();
		}
	}
}


void Revealer()
{
	bool replaced = false;
	int n;
	string temp;

	for (int j = 0; j < glb__key.length(); j++)
	{
		srand(time(NULL));
		n = rand() % (glb__key.length() - 1) + 1;

		if (glb__revealed[n] == "\0")
		{
			replaced = true;
			temp = glb__key[n];
			break;
		}

		else
		{
			Sleep(100);
			continue;
		}
	}

	if (replaced)
	{
		glb__revealed[n] = temp;
		Printer();
	}
}





void AnswerResult()
{
	for_each(glb__input.begin(), glb__input.end(), [](char& c) { c = toupper(c); });

	if (glb__input == "TIMEOUT")
	{
		glb__player.score -= glb__pnt_w;
		cout << "\n\nThoi gian nhap dap an cua ban qua gioi han \nBan vua bi tru di " << glb__pnt_w << " diem \nDiem hien tai cua ban la " << glb__player.score;
	}

	else if (glb__input == glb__key)
	{
		glb__player.score += glb__round_pnt;
		cout << "\n\nDap an cua ban chinh xac. Xin chuc mung ban" << "\n";
		cout << "\n\nBan vua duoc cong them " << glb__round_pnt << " diem \nDiem hien tai cua ban la " << glb__player.score;

		glb__played.keys[glb__played.size] = glb__key;
		glb__played.size++;
	}

	else
	{
		glb__player.score -= glb__pnt_w;
		cout << "\n\nDap an cua ban khong chinh xac \nBan vua bi tru di " << glb__pnt_w << " diem \nDiem hien tai cua ban la " << glb__player.score;
	}

	UpdateScrbrdData();
	UpdatePlayerData(glb__player.name);
}


void SessionResult()
{
	UpdateScrbrdData();
	UpdatePlayerData(glb__player.name);

	int pos;

	for (int i = 0; i < glb__scrbrd.size; i++)
	{
		if (glb__scrbrd.players[i].name == glb__player.name)
		{
			pos = i;
			break;
		}
	}
	system("cls");
	PrintLogo();
	cout << "\n\nTong diem cua ban la " << glb__player.score << ", vi tri cua ban trong bang xep hang: " << pos + 1 << "/" << glb__scrbrd.size << "\n";
	cout << "\n" << "Chao ban, hen gap lan sau!";

	exit(0);
}