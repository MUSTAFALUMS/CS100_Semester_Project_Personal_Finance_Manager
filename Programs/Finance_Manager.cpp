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



// Function and Class definitions would go here

struct User{
    string username;
    string password;
    string Email;
    string SignUptime,LoginLogoutTime;
    bool LoggedIn;
    double balance;
    double MonthlyBudget;
    double SavingsGoal;
};
struct TransactionRecord{
    string type; // income or expense
    double amount;
    string date;
    string description;
};

const string UserDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\UserData.csv";
const string TransactionsDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\TransactionsData.csv";

map<string, // userID
    map<string, bool> // Key: Login Status, Value: true for logged in, false for logged out
    > Logins; // Track login and Logout status
map<string, // username
    map<string, string>, // Key: Email, Value: email address
    map<string, string>, // Key: Password, Value: encrypted password
    map<string, string> // Key: ID, Value: user ID
    > UserCredentials; // Store user credentials data
map<string, //Username
    map<string, double>, // Key: Balance, Value: balance amount
    map<string, double>,  // Key: Savings Goal, Value: savings goal amount
    map<string, double>   // Key: Monthly Budget, Value: monthly budget amount
    > UserFinances; // Store user financial data
map<string, //User ID
    map<string,TransactionRecord> // Key: Transaction ID, Value: TransactionRecord
    > Transactions; // Store transaction records with unique user IDs
    
// All functions related to user authentication
bool Login(string user, string pass) {
    
    // Authenticate user credentials
    if (Authentication(user, pass, false)) {
        cout << "Login successful for user: " << user << endl;
        Logins[UserCredentials[user]["ID"]]["Login Status"] = true; // Track successful login 

        // update login status for existing user in UserData File

        UpdateUserData(user, {'L'}); // uppercase 'L' for login

        return true;
    } else {

        return false;
    }
}
bool Signup(string user, string pass, string email){ // Add Save to File Functionality
    // Implementation of signup functionality
    if (Authentication(user, pass, true)) {
        passwordEncryption(pass, 7); // Encrypt password before storing
        UserCredentials[user]["Password"] = pass;
        UserCredentials[user]["Email"] = email;
        UserCredentials[user]["ID"] = to_string((rand() % 10000)+1000); // Randomly generate user ID

        // Update new user date to UserData File

        UpdateUserData(user, {'S'}); // uppercase 'S' for signup
        return true;
    } else {
        cout << "Signup failed for user: " << user << endl;
        return false;
    }
}
void passwordEncryption(string& pass, int shift){
    // Simple encryption by shifting characters (Caesar cipher)
    for (char &c : pass) {
        c += shift; // Shift character by specified amount
    }
}
void passwordDecryption(string& pass, int shift){
    // Simple decryption by shifting characters back
    for (char &c : pass) {
        c -= shift; // Shift character back by specified amount
    }
}
bool IsUsernameAvailable(string user){
    // Check if username is available
    return UserCredentials.find(user) == UserCredentials.end();
}
bool IsPasswordValid(string pass){
    // Check if password is valid
    /* Password must be at least 8 characters long and contain a mix of letters, numbers, and special characters */
    if (pass.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : pass) {
        // Bitwise OR assignments, setting the flag to true if condition is met
        hasUpper |= isupper(c); 
        hasLower |= islower(c);
        hasDigit |= isdigit(c);
        hasSpecial |= ispunct(c);
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}
bool Logout(string user){

    // Implementation of logout functionality
    Logins[UserCredentials[user]["ID"]]["Login Status"] = false; // Track successful login
    
    // update logout status to Data for User, within UserData File
    UpdateUserData(user, {'l'}); // lowercase 'l' for logout

    return true;
}
bool Authentication(string user, string pass, bool newAccount){
    // Implementation of authentication functionality using User data in Users map
    if (newAccount) {
        return IsUsernameAvailable(user) && IsPasswordValid(pass);
    }else {
        auto it = UserCredentials.find(user);
        if (it != UserCredentials.end()) {
            string storedPass = it->second["Password"];
            passwordDecryption(storedPass, 7); // Decrypt stored password for comparison
            return storedPass == pass;
        }
    }
    return false; // Authentication failed
}
void UpdatePassword(string user, string newPass, bool forgotPassword){
    string oldPass = UserCredentials[user]["Password"];
    // Implementation of password update functionality
    if (forgotPassword){
        if (IsPasswordValid(newPass)) {
            if (newPass == oldPass) {
                cout << "New password cannot be the same as the old password for user: " << user << endl;
                return;
            }
            passwordEncryption(newPass, 7); // Encrypt new password before storing
            UserCredentials[user]["Password"] = newPass;
            cout << "Password reset successfully for user: " << user << endl;

        } else {
            cout << "New password is not valid for user: " << user << endl;
        }
    } else if (Authentication(user, oldPass, false)) {
        if (oldPass != newPass && IsPasswordValid(newPass)) {
            passwordEncryption(newPass, 7); // Encrypt new password before storing
            UserCredentials[user]["Password"] = newPass;
            cout << "Password updated successfully for user: " << user << endl;
        } else {
            cout << "New password is not valid for user: " << user << endl;
        }
    } else {
        cout << "Old password is incorrect for user: " << user << endl;
    }
}

// All functions related to financial management
void UpdateBalance(string user, double amount){
    UserFinances[user]["Balance"] += amount;
}
double viewBalance(string user){
    return UserFinances[user]["Balance"];
}
void SetBudget(string user, double amount){
    // Implementation of setting budget
    cout << "Budget set to: $" << fixed << setprecision(2) << amount << endl;
    UserFinances[user]["Monthly Budget"] = amount;
}
void SetSavingsGoal(string user, string savingAmount){
    // setting savings goal
    UserFinances[user]["SavingsGoal"] = stod(savingAmount);
}
double ViewFinancialGoals(string user){
    return UserFinances[user]["SavingsGoal"];
}

// Functions related to loading user data
void LoadAllUsers(){ // Load all users from UserData file into Users  and Logins map
    ifstream userDataFile(UserDataFilePath);
    if (!userDataFile.is_open()) {  
        cerr << "Error: Unable to open UserData.csv for reading" << endl;
        return;
    }
    string line;
    while (getline(userDataFile, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        size_t pos5 = line.find(',', pos4 + 1);
        size_t pos6 = line.find(',', pos5 + 1);
        size_t pos7 = line.find(',', pos6 + 1);
        string fileUser = line.substr(0, pos1);
        string fileID = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string fileEmail = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string filePass = line.substr(pos3 + 1);
        bool fileLoginLogoutStatus = (line.substr(pos4 + 1) == "1");
        double fileUserBalance = stod(line.substr(pos5 + 1));
        double fileMonthlyBudget = stod(line.substr(pos6 + 1));
        double fileSavingsGoal = stod(line.substr(pos7 + 1));

        UserCredentials[fileUser]["Email"] = fileEmail;
        UserCredentials[fileUser]["Password"] = filePass;
        UserCredentials[fileUser]["ID"] = fileID;
        Logins[fileUser]["Login Status"] = fileLoginLogoutStatus;
        UserFinances[fileUser]["Balance"] = fileUserBalance;
        UserFinances[fileUser]["Monthly Budget"] = fileMonthlyBudget;
        UserFinances[fileUser]["Savings Goal"] = fileSavingsGoal;

    }
    userDataFile.close();
}
void UpdateUserData(string user, vector<char> changes){ // changes vector indicates what changes to make
    // Update user data in UserData file based on changes vector
    // 'S' for signup, 'L' for login, 'l' for logout, 'b' for balance update
    fstream userDataFile(UserDataFilePath, ios::in | ios::out | ios::app);
    if (!userDataFile.is_open()) {  
        cerr << "Error: Unable to open UserData.csv for updating" << endl;
        return;
    }
    // Upddating user data in file, by adding new line for new user, and removing old line for existing user
    if (find(changes.begin(), changes.end(), 'S') != changes.end()) {
        // New user signup, append new line
        userDataFile << user << "," << UserCredentials[user]["ID"] << "," << UserCredentials[user]["Email"] << "," << UserCredentials[user]["Password"] << "," << true << "," << "0.0" << ",0.0" << ",0.0" << endl;
    } else {
        // Existing user login/logout, need to update existing line
        // Read all lines and rewrite file excluding old line for the user
        if (find(changes.begin(), changes.end(), 'b') != changes.end()) {
            // User balance update
            UpdateBalance(user, UserFinances[user]["Balance"]); // Just to ensure balance is updated in UserFinances map
        }
        vector<string> lines;
        string line;
        while (getline(userDataFile, line)) {
            if (line.find(user + ",") != 0) { // Exclude line for the user
                lines.push_back(line);
            }
        }
        // remove old data for required user, keeping rest of the data intact
        userDataFile.close();
        userDataFile.open(UserDataFilePath, ios::out | ios::trunc); // Reopen file in truncate mode to overwrite
        for (const string& l : lines) {
            userDataFile << l << endl;
        }
        // Append updated line for the user
        bool loginStatus = Logins[user]["Login Status"];
        userDataFile << user << "," << UserCredentials[user]["ID"] << "," << UserCredentials[user]["Email"] << "," << UserCredentials[user]["Password"] << "," << loginStatus << "," << UserFinances[user]["Balance"] << "," << UserFinances[user]["Monthly Budget"] << "," << UserFinances[user]["Savings Goal"] << endl;
    }
    userDataFile.close();
}

// Function for importing bank statements and updating user balance etc
void UpdateUserDataFromBankStatement(string user, string filename){
    ifstream bankFile(filename);
    if (!bankFile.is_open()) {  
        cerr << "Error: Unable to open bank statement file for reading" << endl;
        return;
    } // Bank statement file format assumed as CSV with columns: Month, Description, Income, Expense
    string line;
    double totalIncome = 0.0;
    double totalExpense = 0.0;

    while (getline(bankFile, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        string month = line.substr(0, pos1);
        string description = line.substr(pos1 + 1, pos2 - pos1 - 1);
        double income = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
        double expense = stod(line.substr(pos3 + 1));

        totalIncome += income;
        totalExpense += expense;

    }

    bankFile.close();

    // Update user data file after importing bank statement
    UpdateBalance(user, totalIncome - totalExpense);
    UpdateUserData(user, {'b'}); // 'b' for balance update
    }

// Functions related to transaction management
bool AddTransaction(string userID, string t_type, double t_amount, string t_date, string t_description){
    // Implementation of adding a transaction
    // Check if trancation is valid
    if (!IsTransactionValid(userID, t_type, t_amount)) {
        return false; // Invalid transaction details
    }
    static int NewTransactionID = 1; // Static variable to generate unique transaction IDs
    string TransactionID = "T0000" + to_string(NewTransactionID); // Transaction ID format T00001, T00002, etc.
    TransactionRecord newTransaction;
    newTransaction.type = t_type;
    newTransaction.amount = t_amount;
    newTransaction.date = t_date;
    newTransaction.description = t_description;

    // Add the new transaction to the user's transaction map
    Transactions[userID][TransactionID] = newTransaction;
    NewTransactionID++;
    return true; // Transaction added successfully
}
vector<TransactionRecord> ViewTransactions(string userID, string filter_type = "", string filter_date = ""){
    // Implementation of viewing transactions with optional filters for specific user
    vector<TransactionRecord> filteredTransactions;
    auto userIt = Transactions.find(userID);
    if (userIt != Transactions.end()) {
        auto& userTransactions = userIt->second; // Points to the map of transactions for the user
        for (const auto& transPair : userTransactions) { // Iterate through user's transactions
            const TransactionRecord& trans = transPair.second; // Get the transaction record
            // Apply filters if provided
            if ((filter_type.empty() || trans.type == filter_type) &&
                (filter_date.empty() || trans.date == filter_date)) {
                filteredTransactions.push_back(trans);
            }
        }
    }
    return filteredTransactions;
}
vector<TransactionRecord> ViewTransactionsFromFile(string userID, string filter_type = "", string filter_date = ""){
    // Implementation of viewing transactions with optional filters from file
    vector<TransactionRecord> filteredTransactions;
    ifstream file(TransactionsDataFilePath);
    if (!file.is_open()) {  
        cerr << "Error: Unable to open TransactionsData.csv for reading" << endl;
        return filteredTransactions;
    }

    // search for transcations for the user with optional filters
    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        string fileUserID = line.substr(0, pos1);
        string t_type = line.substr(pos1 + 1, pos2 - pos1 - 1);
        double t_amount = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
        string t_date = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string t_description = line.substr(pos4 + 1);

        if (fileUserID == userID) {
            // Apply filters if provided
            if ((filter_type.empty() || t_type == filter_type) &&
                (filter_date.empty() || t_date == filter_date)) {
                TransactionRecord record;
                record.type = t_type;
                record.amount = t_amount;
                record.date = t_date;
                record.description = t_description;
                filteredTransactions.push_back(record);
            }
        }
    }
    file.close();
    return filteredTransactions;
}
bool DeleteTransaction(string userID, string transaction_id){
    // Implementation of deleting a transaction
    auto userIt = Transactions.find(userID);
    if (userIt != Transactions.end()) {
        auto& userTransactions = userIt->second; // Points to the map of transactions for the user
        auto transIt = userTransactions.find(transaction_id);// Find the transaction by ID
        if (transIt != userTransactions.end()) {
            userTransactions.erase(transIt); // Delete the transaction
            return true; // Deletion successful
        }
    }
    return false;
}
bool IsTransactionValid(string userID, string t_type, double t_amount){
    // Validate transaction details
    if (t_type != "income" && t_type != "expense") {
        return false; // Invalid transaction type
    }
    if (t_type == "expense" && (t_amount <= 0 && t_amount > viewBalance(userID))) {
        return false; // Expense amount must be positive
    }
    return true; // Transaction is valid
}
bool SaveTransactionsToFile(string filename, string userID, vector<TransactionRecord> userTransactions){
    // userTransactions: vector of transaction records for the user
    // Format: UserID, Type, Amount, Date, Description
    ofstream file(filename, ios::app);
    if (!file.is_open()) {  
        cerr << "Error: Unable to open TransactionsData.csv for writing" << endl;
        return false;
    }
    for (const auto& trans : userTransactions) {
        file << userID << "," << trans.type << "," << trans.amount << "," << trans.date << "," << trans.description << endl;
    }
    file.close();
    return true;
}



int main()
{


    return 0;
}