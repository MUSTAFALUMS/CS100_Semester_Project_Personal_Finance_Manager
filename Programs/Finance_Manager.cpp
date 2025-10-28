#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace sf;
using namespace std;


vector <string> UniqueUserIDs;

// User and Transaction structs
struct User{
    string userID;
    string username;
    string password;
    string Email;
    string SignUptime,LoginLogoutTime;
    bool LoggedIn;
    double balance;
    double MonthlyBudget;
    double SavingsGoal;
};
struct Transaction{
    string type; // income or expense
    double amount;
    string date;
    string description;
};

// User and Transaction Data Vectors
vector<User> users;
vector<Transaction> transactions;


// Functions Declarations

// User authentication and login management functions
bool loginUser(string username, string password);
bool logoutUser(string userID);
bool signUpUser(string username, string password, string email);
bool deleteUser(string userID);
bool authenticateUser(string username, string password);
bool isPasswordValid(string password);
bool isEmailValid(string email);
bool isUsernameAvailable(string username);
bool generateUniqueUserID(User &newUser);
bool UpdatePassword(string userID, string newPassword);
bool UpdateEmail(string userID, string newEmail);
bool PasswordEncryption(string &password);
bool PasswordDecryption(string &password);

// User data management functions
bool addUser(const User &newUser);
bool removeUser(const string &userID);
bool updateUser(const User &updatedUser);
User* getUser(const string &userID);
vector<User> getAllUsers();

// User finance management functions
bool UpdateBalance(const string &userID, double amount);
bool SetMonthlyBudget(const string &userID, double budget);
bool SetSavingsGoal(const string &userID, double goal);
double GetBalance(const string &userID);
double GetMonthlyBudget(const string &userID);
double GetSavingsGoal(const string &userID);

// Transaction management functions
bool addTransaction(const string &userID, const Transaction &newTransaction);
bool removeTransaction(const string &userID, const string &transactionID);
bool updateTransaction(const string &userID, const string &transactionID, const Transaction &updatedTransaction);
vector<Transaction> getUserTransactions(const string &userID);

// Data persistence functions
bool loadUserData(string userID, string filePath);
bool saveUserData(string userID, string filePath, vector<char> changes);
bool LoadDataFromBankStatement(const string userID, const string filePath);


int main()
{
    // All relevant file paths
    const string UserDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\UserData.csv";
const string TransactionsDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\TransactionsData.csv";


    return 0;
}


// Functions Definitions