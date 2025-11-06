#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
using namespace std;

enum Gender
{
	Male,
	Female,
};

class User
{
	string id;
	string username;
	string password;
	string email;
	string name;
	string surname;
	string number;
	int age;
	Gender gender;
	string card;
	tm dataofbirth{};
	void setId(string id) {
		if (isValidId(id))
			this->id = id;
		else
			throw string("Invalid Id...");
	}
public:
	User() {};

	User(string id, string username, string password, string email, string name, string surname, int age, string number, Gender gender, int day, int month, int year, string card = "") {
		setId(id);
		setUsername(username);
		setPassword(password);
		setEmail(email);
		setName(name);
		setSurname(surname);
		setAge(age);
		setDataOfBirth(day, month, year);
		setGender(gender);
		setNumber(number);
		setCard(card);
	}

	bool isValidPhoneNumber(const string& number) {
		const regex pattern("^\\+994(50|51|55|70|77|99|10)[0-9]{7}$");
		return regex_match(number, pattern);
	}

	bool isValidEmail(const string& email) {
		const regex pattern("^[A-Za-z0-9._%+-]+@(gmail|mail|outlook|yahoo)\\.(com|ru|az)$");
		return regex_match(email, pattern);
	}

	bool isValidId(const string& id) {
		const regex pattern("^[A-Z0-9]{7}$");
		return regex_match(id, pattern);
	}



	string getId() const noexcept { return id; }
	string getUserName() const noexcept { return username; }
	string getPassword() const noexcept { return password; }
	string getEmail() const noexcept { return email; }
	string getName() const noexcept { return name; }
	string getSurname() const noexcept { return surname; }
	string getNumber() const noexcept { return number; }
	string getCard() const noexcept { return card; }
	int getAge() const noexcept { return age; }
	tm getDataOfBirth() const noexcept { return dataofbirth; }
	string getGender() const noexcept {
		if (gender == Male) { return "Male"; }
		else if (gender == Female) { return "Female"; }
		return "Unknown";
	}

	void setUsername(string username) {
		if (username.length() > 7)
			this->username = username;
		else
			throw string("The Username must be at least 8 characters long.");
	}
	void setPassword(string password) {
		if (password.length() > 7)
			this->password = password;
		else
			throw string("The Password must be at least 8 characters long.");
	}
	void setEmail(string email) {
		if (isValidEmail(email))
			this->email = email;
		else
			throw string("Invalid email format!");
	}
	void setName(string name) {
		if (name.length() > 2)
			this->name = name;
		else
			throw string("The Name must be at least 2 characters long.");
	}
	void setSurname(string surname) {
		if (surname.length() > 4)
			this->surname = surname;
		else
			throw string("Surname must be at least 4 characters long.");
	}
	void setAge(int age) {
		try
		{
			if (age > 18)
				this->age = age;
			else
				throw string("User must be at least 18 years old.");
		}
		catch (const string& ex)
		{
			cout << ex << endl;
		}
	}
	void setCard(string card) {
		this->card = card;
	}
	void setDataOfBirth(int day, int month, int year) {

		dataofbirth.tm_mday = day;
		dataofbirth.tm_mon = month - 1;
		dataofbirth.tm_year = year - 1900;

		time_t now = time(0);
		tm currenct{};
		localtime_s(&currenct, &now);

		int calculatorAge = (currenct.tm_year - dataofbirth.tm_year);
		if ((currenct.tm_mon < dataofbirth.tm_mon) ||
			(currenct.tm_mon == dataofbirth.tm_mon &&
				currenct.tm_mday < dataofbirth.tm_mday)) {
			calculatorAge--;
		}

		if (calculatorAge >= 18)
			this->age = calculatorAge;
		else
			throw string("User must be at least 18 years old.");
	}
	void setGender(Gender gender) {
		if (gender == Male || gender == Female)
			this->gender = gender;
		else
		{
			throw string("Invalid gender value!");
		}
	}
	void setNumber(string number) {
		if (isValidPhoneNumber(number)) {
			this->number = number;
		}
		else
			throw string("Invalid phone number format!");
	}
	void ShowUser() {

		cout << "--------------------------------------------------------------------------------------------------\n" << endl;

		cout << "----------------User-Login-Information----------------" << endl;
		cout << "Username: " << getUserName() << endl;
		cout << "Password: " << getPassword() << endl;
		cout << "Email: " << getEmail() << endl;
		cout << "----------------User-Information----------------" << endl;
		cout << "Id: " << getId() << endl;
		cout << "Name: " << getName() << endl;
		cout << "Surname: " << getSurname() << endl;
		cout << "Age: " << getAge() << endl;
		cout << "Birthday( Day / Month / Year ): " << dataofbirth.tm_mday << " / " << dataofbirth.tm_mon + 1 << " / " << dataofbirth.tm_year + 1900 << endl;
		cout << "Gender: " << getGender() << endl;
	}

	friend istream& operator>>(istream& in, User& right) {
		string id, username, password, email, name, surname, number, gender_str;
		int age, day, month, year;
		Gender gender;

		try
		{
			cout << "id: "; cin >> id;
			cout << "Name: "; cin >> name;
			cout << "Surname: "; cin >> surname;
			cout << "Age: "; cin >> age;
			cout << "UserName: "; cin >> username;
			cout << "Password: "; cin >> password;
			cout << "Email: "; cin >> email;
			cout << "Number(+994....): "; cin >> number;
			cout << "Gender(Male/Female): "; cin >> gender_str;
			cout << "Birthday(Day/Month/Year): "; cin >> day >> month >> year;
		}
		catch (const string& ex)
		{
			cout << ex << endl;
		}



		if (gender_str == "Male" || gender_str == "male" || gender_str == "MALE") gender = Male;
		else if (gender_str == "Female" || gender_str == "female" || gender_str == "FEMALE") gender = Female;
		else throw string("Invalid gender input!");

		right = User(id, username, password, email, name, surname, age, number, gender, day, month, year);

		return in;
	}
};

class UserManager {
	vector<User> users;
public:

	UserManager() {
		try
		{
			loadUserData(users);
		}
		catch (string& ex)
		{
			cout << ex << endl;
		}
	}

	void signUp(const User& user) {
		for (size_t i = 0; i < users.size(); i++)
		{
			if (users[i].getId() == user.getId())
				throw string("This id already exists!");
			if (users[i].getEmail() == user.getEmail())
				throw string("This email already exists!");
			if (users[i].getNumber() == user.getNumber())
				throw string("This Number already exists!");
		}
		users.push_back(user);
		saveUserData(users);
		cout << "User successfully registered!\n";
	}

	void signIn(const string& username, const string& password) {
		for (size_t i = 0; i < users.size(); i++)
		{
			if (users[i].getUserName() == username && users[i].getPassword() == password) {
				cout << "Login successful\n";
				return;
			}
		}
		if (username == "Admin" && password == "Admin") {
			cout << "Hi Admin" << endl;
			return;
		}
	}

	void saveUserData(vector<User>& users) {
		ofstream fs("User.txt", ios::out);
		try
		{
			string oldData;
			ifstream read("User.txt");
			while (getline(read, oldData)) {
				string line;
				if (line.empty()) continue;
				oldData += line + "\n";
			}


			if (!fs.is_open()) {
				throw string("File couldn't open...");
			}

			fs << oldData;



			for (auto& user : users)
			{
				if (user.isValidEmail(user.getEmail()) && user.isValidId(user.getId()) && user.isValidPhoneNumber(user.getNumber())) {
					fs << user.getId() << "_"
						<< user.getUserName() << "_"
						<< user.getPassword() << "_"
						<< user.getEmail() << "_"
						<< user.getName() << "_"
						<< user.getSurname() << "_"
						<< user.getAge() << "_"
						<< user.getNumber() << "_"
						<< user.getGender() << "_"
						<< user.getDataOfBirth().tm_mday << "/"
						<< user.getDataOfBirth().tm_mon + 1 << "/"
						<< user.getDataOfBirth().tm_year + 1900 << "\n";
				}
			}
			fs.close();

		}
		catch (const string& ex) {
			cout << ex << endl;
		}
	}


	void loadUserData(vector<User> users) {
		ifstream fs("User.txt");

		if (!fs.is_open()) {
			ofstream fs("User.txt");
			fs.close();
			return;
		}

		string row;
		while (getline(fs, row)) {
			if (row.empty()) {
				throw string();
			}

			string id, username, password, email, name, surname, age_str, number, gender_str, day_str, month_str, year_str;
			int counter = 0;
			for (auto& character : row) {
				if (character == '_') {
					counter++;
					continue;
				}
				switch (counter)
				{
				case 0: id += character; break;
				case 1: username += character; break;
				case 2: password += character; break;
				case 3: email += character; break;
				case 4: name += character; break;
				case 5: surname += character; break;
				case 6: age_str += character; break;
				case 7: number += character; break;
				case 8: gender_str += character; break;
				case 9: day_str += character; break;
				case 10: month_str += character; break;
				case 11: year_str += character; break;
				default:
					break;
				}
			}
			getline(fs, id, '_');
			getline(fs, username, '_');
			getline(fs, password, '_');
			getline(fs, email, '_');
			getline(fs, name, '_');
			getline(fs, surname, '_');
			getline(fs, age_str, '_');
			getline(fs, number, '_');
			getline(fs, gender_str, '_');
			getline(fs, day_str, '/');
			getline(fs, month_str, '/');
			getline(fs, year_str, '\n');

			if (id.empty() || username.empty() || password.empty() || email.empty() || name.empty() || surname.empty() ||
				age_str.empty() || number.empty() || gender_str.empty() || day_str.empty() || month_str.empty() || year_str.empty()) continue;

			int age = stoi(age_str);
			int day = stoi(day_str);
			int month = stoi(month_str);
			int year = stoi(year_str);

			Gender gender;
			if (gender_str == "Male") gender = Male;
			else if (gender_str == "Female") gender = Female;
			else throw string("Invalid gender format!");
			User user(id, username, password, email, name, surname, age, number, gender, day, month, year);
			users.push_back(user);
		}
	}

};


int main()
{
	User use;
	string inputUsername, inputPassword;
	UserManager userM;

	while (true)
	{
		string choice_str;
		int choice;
		cout << "1.Sign In\n2.Sing Up\nEnter: ";
		cin >> choice_str;
		try
		{
			choice = stoi(choice_str);
		}
		catch (...)
		{
			cout << "SOME ERROR" << endl;
		}

		if (choice == 1) {
			try
			{
				cout << "Username: "; cin >> inputUsername;
				cout << "Password: ";cin >> inputPassword;
				userM.signIn(inputUsername, inputPassword);
			}
			catch (const string& ex)
			{
				cout << ex << endl;
			}

		}

		else if (choice == 2) {
			try
			{
				cin >> use;
				userM.signUp(use);
			}
			catch (const string& ex)
			{
				cout << ex << endl;
			}
		}
		else throw string();
	}
}#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
using namespace std;

enum Gender
{
	Male,
	Female,
};

class User
{
	string id;
	string username;
	string password;
	string email;
	string name;
	string surname;
	string number;
	int age;
	Gender gender;
	string card;
	tm dataofbirth{};
	void setId(string id) {
		if (isValidId(id))
			this->id = id;
		else
			throw string("Invalid Id...");
	}
public:
	User() {};

	User(string id, string username, string password, string email, string name, string surname, int age, string number, Gender gender, int day, int month, int year, string card = "") {
		setId(id);
		setUsername(username);
		setPassword(password);
		setEmail(email);
		setName(name);
		setSurname(surname);
		setAge(age);
		setDataOfBirth(day, month, year);
		setGender(gender);
		setNumber(number);
		setCard(card);
	}

	bool isValidPhoneNumber(const string& number) {
		const regex pattern("^\\+994(50|51|55|70|77|99|10)[0-9]{7}$");
		return regex_match(number, pattern);
	}

	bool isValidEmail(const string& email) {
		const regex pattern("^[A-Za-z0-9._%+-]+@(gmail|mail|outlook|yahoo)\\.(com|ru|az)$");
		return regex_match(email, pattern);
	}

	bool isValidId(const string& id) {
		const regex pattern("^[A-Z0-9]{7}$");
		return regex_match(id, pattern);
	}



	string getId() const noexcept { return id; }
	string getUserName() const noexcept { return username; }
	string getPassword() const noexcept { return password; }
	string getEmail() const noexcept { return email; }
	string getName() const noexcept { return name; }
	string getSurname() const noexcept { return surname; }
	string getNumber() const noexcept { return number; }
	string getCard() const noexcept { return card; }
	int getAge() const noexcept { return age; }
	tm getDataOfBirth() const noexcept { return dataofbirth; }
	string getGender() const noexcept {
		if (gender == Male) { return "Male"; }
		else if (gender == Female) { return "Female"; }
		return "Unknown";
	}

	void setUsername(string username) {
		if (username.length() > 7)
			this->username = username;
		else
			throw string("The Username must be at least 8 characters long.");
	}
	void setPassword(string password) {
		if (password.length() > 7)
			this->password = password;
		else
			throw string("The Password must be at least 8 characters long.");
	}
	void setEmail(string email) {
		if (isValidEmail(email))
			this->email = email;
		else
			throw string("Invalid email format!");
	}
	void setName(string name) {
		if (name.length() > 2)
			this->name = name;
		else
			throw string("The Name must be at least 2 characters long.");
	}
	void setSurname(string surname) {
		if (surname.length() > 4)
			this->surname = surname;
		else
			throw string("Surname must be at least 4 characters long.");
	}
	void setAge(int age) {
		try
		{
			if (age > 18)
				this->age = age;
			else
				throw string("User must be at least 18 years old.");
		}
		catch (const string& ex)
		{
			cout << ex << endl;
		}
	}
	void setCard(string card) {
		this->card = card;
	}
	void setDataOfBirth(int day, int month, int year) {

		dataofbirth.tm_mday = day;
		dataofbirth.tm_mon = month - 1;
		dataofbirth.tm_year = year - 1900;

		time_t now = time(0);
		tm currenct{};
		localtime_s(&currenct, &now);

		int calculatorAge = (currenct.tm_year - dataofbirth.tm_year);
		if ((currenct.tm_mon < dataofbirth.tm_mon) ||
			(currenct.tm_mon == dataofbirth.tm_mon &&
				currenct.tm_mday < dataofbirth.tm_mday)) {
			calculatorAge--;
		}

		if (calculatorAge >= 18)
			this->age = calculatorAge;
		else
			throw string("User must be at least 18 years old.");
	}
	void setGender(Gender gender) {
		if (gender == Male || gender == Female)
			this->gender = gender;
		else
		{
			throw string("Invalid gender value!");
		}
	}
	void setNumber(string number) {
		if (isValidPhoneNumber(number)) {
			this->number = number;
		}
		else
			throw string("Invalid phone number format!");
	}
	void ShowUser() {

		cout << "--------------------------------------------------------------------------------------------------\n" << endl;

		cout << "----------------User-Login-Information----------------" << endl;
		cout << "Username: " << getUserName() << endl;
		cout << "Password: " << getPassword() << endl;
		cout << "Email: " << getEmail() << endl;
		cout << "----------------User-Information----------------" << endl;
		cout << "Id: " << getId() << endl;
		cout << "Name: " << getName() << endl;
		cout << "Surname: " << getSurname() << endl;
		cout << "Age: " << getAge() << endl;
		cout << "Birthday( Day / Month / Year ): " << dataofbirth.tm_mday << " / " << dataofbirth.tm_mon + 1 << " / " << dataofbirth.tm_year + 1900 << endl;
		cout << "Gender: " << getGender() << endl;
	}

	friend istream& operator>>(istream& in, User& right) {
		string id, username, password, email, name, surname, number, gender_str;
		int age, day, month, year;
		Gender gender;

		try
		{
			cout << "id: "; cin >> id;
			cout << "Name: "; cin >> name;
			cout << "Surname: "; cin >> surname;
			cout << "Age: "; cin >> age;
			cout << "UserName: "; cin >> username;
			cout << "Password: "; cin >> password;
			cout << "Email: "; cin >> email;
			cout << "Number(+994....): "; cin >> number;
			cout << "Gender(Male/Female): "; cin >> gender_str;
			cout << "Birthday(Day/Month/Year): "; cin >> day >> month >> year;
		}
		catch (const string& ex)
		{
			cout << ex << endl;
		}



		if (gender_str == "Male" || gender_str == "male" || gender_str == "MALE") gender = Male;
		else if (gender_str == "Female" || gender_str == "female" || gender_str == "FEMALE") gender = Female;
		else throw string("Invalid gender input!");

		right = User(id, username, password, email, name, surname, age, number, gender, day, month, year);

		return in;
	}
};

class UserManager {
	vector<User> users;
public:

	UserManager() {
		try
		{
			loadUserData(users);
		}
		catch (string& ex)
		{
			cout << ex << endl;
		}
	}

	void signUp(const User& user) {
		for (size_t i = 0; i < users.size(); i++)
		{
			if (users[i].getId() == user.getId())
				throw string("This id already exists!");
			if (users[i].getEmail() == user.getEmail())
				throw string("This email already exists!");
			if (users[i].getNumber() == user.getNumber())
				throw string("This Number already exists!");
		}
		users.push_back(user);
		saveUserData(users);
		cout << "User successfully registered!\n";
	}

	void signIn(const string& username, const string& password) {
		for (size_t i = 0; i < users.size(); i++)
		{
			if (users[i].getUserName() == username && users[i].getPassword() == password) {
				cout << "Login successful\n";
				return;
			}
		}
		if (username == "Admin" && password == "Admin") {
			cout << "Hi Admin" << endl;
			return;
		}
	}

	void saveUserData(vector<User>& users) {
		ofstream fs("User.txt", ios::out);
		try
		{
			string oldData;
			ifstream read("User.txt");
			while (getline(read, oldData)) {
				string line;
				if (line.empty()) continue;
				oldData += line + "\n";
			}


			if (!fs.is_open()) {
				throw string("File couldn't open...");
			}

			fs << oldData;



			for (auto& user : users)
			{
				if (user.isValidEmail(user.getEmail()) && user.isValidId(user.getId()) && user.isValidPhoneNumber(user.getNumber())) {
					fs << user.getId() << "_"
						<< user.getUserName() << "_"
						<< user.getPassword() << "_"
						<< user.getEmail() << "_"
						<< user.getName() << "_"
						<< user.getSurname() << "_"
						<< user.getAge() << "_"
						<< user.getNumber() << "_"
						<< user.getGender() << "_"
						<< user.getDataOfBirth().tm_mday << "/"
						<< user.getDataOfBirth().tm_mon + 1 << "/"
						<< user.getDataOfBirth().tm_year + 1900 << "\n";
				}
			}
			fs.close();

		}
		catch (const string& ex) {
			cout << ex << endl;
		}
	}


	void loadUserData(vector<User> users) {
		ifstream fs("User.txt");

		if (!fs.is_open()) {
			ofstream fs("User.txt");
			fs.close();
			return;
		}

		string row;
		while (getline(fs, row)) {
			if (row.empty()) {
				throw string();
			}

			string id, username, password, email, name, surname, age_str, number, gender_str, day_str, month_str, year_str;
			int counter = 0;
			for (auto& character : row) {
				if (character == '_') {
					counter++;
					continue;
				}
				switch (counter)
				{
				case 0: id += character; break;
				case 1: username += character; break;
				case 2: password += character; break;
				case 3: email += character; break;
				case 4: name += character; break;
				case 5: surname += character; break;
				case 6: age_str += character; break;
				case 7: number += character; break;
				case 8: gender_str += character; break;
				case 9: day_str += character; break;
				case 10: month_str += character; break;
				case 11: year_str += character; break;
				default:
					break;
				}
			}
			getline(fs, id, '_');
			getline(fs, username, '_');
			getline(fs, password, '_');
			getline(fs, email, '_');
			getline(fs, name, '_');
			getline(fs, surname, '_');
			getline(fs, age_str, '_');
			getline(fs, number, '_');
			getline(fs, gender_str, '_');
			getline(fs, day_str, '/');
			getline(fs, month_str, '/');
			getline(fs, year_str, '\n');

			if (id.empty() || username.empty() || password.empty() || email.empty() || name.empty() || surname.empty() ||
				age_str.empty() || number.empty() || gender_str.empty() || day_str.empty() || month_str.empty() || year_str.empty()) continue;

			int age = stoi(age_str);
			int day = stoi(day_str);
			int month = stoi(month_str);
			int year = stoi(year_str);

			Gender gender;
			if (gender_str == "Male") gender = Male;
			else if (gender_str == "Female") gender = Female;
			else throw string("Invalid gender format!");
			User user(id, username, password, email, name, surname, age, number, gender, day, month, year);
			users.push_back(user);
		}
	}

};


int main()
{
	User use;
	string inputUsername, inputPassword;
	UserManager userM;

	while (true)
	{
		string choice_str;
		int choice;
		cout << "1.Sign In\n2.Sing Up\nEnter: ";
		cin >> choice_str;
		try
		{
			choice = stoi(choice_str);
		}
		catch (...)
		{
			cout << "SOME ERROR" << endl;
		}

		if (choice == 1) {
			try
			{
				cout << "Username: "; cin >> inputUsername;
				cout << "Password: ";cin >> inputPassword;
				userM.signIn(inputUsername, inputPassword);
			}
			catch (const string& ex)
			{
				cout << ex << endl;
			}

		}

		else if (choice == 2) {
			try
			{
				cin >> use;
				userM.signUp(use);
			}
			catch (const string& ex)
			{
				cout << ex << endl;
			}
		}
		else throw string();
	}
}