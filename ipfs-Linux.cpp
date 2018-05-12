#include<iostream>
#include<string>
#include<unistd.h>
#include<algorithm>
#include<fstream>
#include<vector>
#include<sstream>
using namespace std;

class Cfile
{
private:
	int number;
	string hash;
	string filename;
public:
	Cfile(int num_, const string& hash_, const string& filename_);
	int getNumber() const;
	const string& getHash() const;
	const string& getFilename() const;
};
class constants
{
public:
	static string path;
	static vector<Cfile> files;
};
class operations
{
public:
	static void welcome();
	static void init();

	static void read_files();
	static void parse(const string& cmd_);

	static void change_directory(const string& path_);
	static void upload(const string& flname_);
	static void download(const string& flnum_);
	static void list();
};

string constants::path = "";
vector<Cfile> constants::files;

int main()
{
	operations::welcome();
	operations::init();
	while (true)
	{
		string cmd;
		getline(cin, cmd);
		//system(cmd.c_str());
		operations::parse(cmd);
	}
}

Cfile::Cfile(int num_, const string& hash_, const string& filename_)
{
	number = num_;
	hash = hash_;
	filename = filename_;
}
int Cfile::getNumber() const
{
	return number;
}
const string& Cfile::getHash() const
{
	return hash;
}
const string& Cfile::getFilename() const
{
	return filename;
}


void operations::welcome()
{
	cout << "Welcome to the IPFS test system!" << endl;
	cout << "Commands:" << endl;
	cout << "cd <path>\t\tChange working directory" << endl;
	cout << "upload <filename>\t\tUpload a file to IPFS" << endl;
	cout << "download <filenumber>\t\tDownload a file" << endl;
	cout << "ls\t\tShow available files and their numbers" << endl;
	cout << "exit\t\tLeave the program" << endl;
}
void operations::init()
{
	char tmp[1024];
	getcwd(tmp, 1024);
	change_directory(tmp);
	//read_files();
}
void operations::read_files()
{
	constants::files.clear();
	ifstream logIn;
	logIn.open(constants::path + "/log.txt", ios::in);
	if (!logIn.is_open())
	{
		cout << "Log file not found. Please change to another directory or upload a file to create a new log file." << endl;
		return;
	}
	while (!logIn.eof())
	{
		int num;
		string hash, filename;
		logIn >> num >> hash >> filename;
		constants::files.push_back(Cfile(num, hash, filename));
	}
	cout << "Found " << constants::files.size() << " files" << endl;
	list();
}
void operations::parse(const string& cmd_)
{
	if (cmd_.compare(0, 2, "cd") == 0)
		change_directory(cmd_.substr(3));
	else if (cmd_.compare(0, 6, "upload") == 0)
		upload(cmd_.substr(7));
	else if (cmd_.compare(0, 8, "download") == 0)
		download(cmd_.substr(9));
	else if (cmd_.compare(0, 2, "ls") == 0)
		list();
	else if (cmd_.compare(0, 4, "exit") == 0)
		exit(0);
	else
		cout << "Invalid command" << endl;

}
void operations::change_directory(const string& path_)
{
	constants::path.assign(path_);
	cout << "Current path: " << path_ << endl;
	read_files();
}
void operations::upload(const string& flname_)
{
	string cmd = "ipfs add ";
	cmd.append(constants::path);
	cmd.append("/");
	cmd.append(flname_);
	cmd.append(" > ");
	cmd.append(constants::path);
	cmd.append("/added.tmp");
	system(cmd.c_str());

	ifstream added;
	added.open(constants::path + "/added.tmp", ios::in);
	/*"added.tmp"'s contents: added <hash> <filename>*/
    if(added.eof())
    {
        cout<<"Upload failed"<<endl;
        return;
    }
	string hash;
	added >> hash;//ignore the word "added"
	added >> hash;
	added.close();

	constants::files.push_back(Cfile(constants::files.size(), hash, flname_));

	ofstream logOut;
	logOut.open(constants::path + "/log.txt", ios::app);
	logOut << constants::files.size() << " " << hash << " " << flname_ << endl;
	logOut.close();

	system(("rm " + constants::path + "/added.tmp").c_str());
	cout << "DEBUG:" << "rm " + constants::path + "/added.tmp" << endl;
}
void operations::download(const string& flnum_)
{
	stringstream buf(flnum_);
	int num;
	buf >> num;

	bool found = false;
	for (const Cfile& f : constants::files)
	{
		if (f.getNumber() == num)
		{
			string cmd = "ipfs cat ";
			cmd.append(f.getHash());
			cmd.append(" > ");
			cmd.append(constants::path);
			cmd.append("/");
			cmd.append(f.getFilename());
			/*cmd: ipfs cat <hash> > <path><filename>*/

			system(cmd.c_str());
			found = true;
		}
	}

	if (!found)
		cout << "Number not found. Please try another number." << endl;
	else
		cout << "Successfully download files." << endl;
}
void operations::list()//the name "list" may lead to some errors
{
	cout << "Number\tFilename" << endl;
	for (const Cfile& f : constants::files)
	{
		cout << f.getNumber() << '\t' << f.getFilename() << endl;
	}

}
