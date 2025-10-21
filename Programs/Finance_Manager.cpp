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
using namespace sf;
using namespace std;

// Class and Function definitions would go here
class User{
    public:
        string username;
        string password;
        string Email;
        double balance;
        double MonthlyBudget;
        string UserLoginStatusFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\LoginLogout.csv";
        string UserDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\UserData.csv";
        map<string, map<string, bool>, map<string, string>> Logins; // Track login status
        map<string, map<string, string>, map<string, string>, map<string, string>> Users;
        
        // All functions related to user authentication
        bool Login(string user, string pass) {

            username = user;
            password = pass;

            // Get formatted timestamp
            time_t now = time(0);
            char* dt = ctime(&now);
            
            // Authenticate user credentials
            if (Authentication(user, pass, false)) {
                cout << "Login successful for user: " << user << endl;
                Logins[user]["Login Status"] = true; // Track successful login
                Logins[user]["Login Time"] = dt; // Track login time

                // Save login event to file with timestamp
                ofstream LoginLogoutFile(UserLoginStatusFilePath, ios::app);
                
                if (!LoginLogoutFile.is_open()) {
                    cerr << "Error: Unable to open LoginLogout.csv for writing" << endl;
                    return true; // Login succeeded but logging failed
                }
                
                // Write login record (remove newline from ctime output)
                LoginLogoutFile << user << "," << true << "," << dt;
                LoginLogoutFile.close();
                
                return true;
            } else {
                cout << "Login failed for user: " << user << endl;
                return false;
            }
        }
        bool Signup(string user, string pass, string email){ // Add Save to File Functionality
            // Get formatted timestamp
            time_t now = time(0);
            char* dt = ctime(&now);
            // Implementation of signup functionality
            if (IsUsernameAvailable(user) && IsPasswordValid(pass)) {
                passwordEncryption(pass, 7); // Encrypt password before storing
                Users[user]["Password"] = pass;
                Users[user]["Signup Time"] = dt;
                Users[user]["Email"] = email;
                cout << "Signup successful for user: " << user << endl;
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
            return Users.find(user) == Users.end();
            return true; 
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
            // Get formatted timestamp
            time_t now = time(0);
            char* dt = ctime(&now);
            // Implementation of logout functionality
            Logins[user]["Login Status"] = false; // Track successful login
            Logins[user]["Login Time"] = dt; // Track login time
            cout << "User " << user << " logged out successfully." << endl;
            // Save login event to file with timestamp
                ofstream LoginLogoutFile(UserLoginStatusFilePath, ios::app);
                
            if (!LoginLogoutFile.is_open()) {
                cerr << "Error: Unable to open LoginLogout.csv for writing" << endl;
                return true; // Login succeeded but logging failed
            }
            
            // Write login record (remove newline from ctime output)
            LoginLogoutFile << user << "," << false << "," << dt;
            LoginLogoutFile.close();
            return true;
        }
        bool Authentication(string user, string pass, bool newAccount){
            // Implementation of authentication functionality using data stored in UserData file
            ifstream userDataFile(UserDataFilePath);
            if (!userDataFile.is_open()) {  
                cerr << "Error: Unable to open UserData.csv for reading" << endl;
                return false;
            }
            string line;
            while (getline(userDataFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                string fileUser = line.substr(0, pos1);
                string fileEmail = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string filePass = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string fileSignupTime = line.substr(pos3 + 1);
                
                // Decrypt stored password for comparison
                passwordDecryption(filePass, 7);
                
                if (!newAccount) {
                    // For existing account login, check username and password
                    if (fileUser == user && filePass == pass) {
                        userDataFile.close();
                        return true; // Authentication successful
                    }
                }
            }
            return false; // Authentication failed
            
        }
        void UpdatePassword(string user, string oldPass, string newPass); 

        // All functions related to financial management
        void SetInitialBalance(double amount){
            balance = amount;
        }
        void UpdateBalance(double amount){
            balance += amount;
        }
        void viewBalance(string user){
            cout << "Current balance for user " << user << " is: $" << fixed << setprecision(2) << balance << endl;
        }
        void SetBudget(double amount){
            // Implementation of setting budget
            cout << "Budget set to: $" << fixed << setprecision(2) << amount << endl;
            MonthlyBudget = amount;
        }
        void SetFinancialGoals(string user, string goal);
        void ViewFinancialGoals(string user);

        // Functions related to data persistence
        void LoadUserData(string filename){
            // Implementation of loading user data from file
            ifstream userDataFile(filename);
            if (!userDataFile.is_open()) {  
                cerr << "Error: Unable to open " << filename << " for reading" << endl;
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
                string fileUser = line.substr(0, pos1);
                string fileEmail = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string filePass = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string fileLoginStatus = line.substr(pos4 + 1, pos5 - pos4 - 1);
                string fileBalanceStr = line.substr(pos6 + 1);
                double fileBalance = stod(fileBalanceStr);

                Users[fileUser]["Email"] = fileEmail;
                Users[fileUser]["Password"] = filePass;
                Logins[fileUser]["Login Status"] = (fileLoginStatus == "1" || fileLoginStatus == "true");
                if (fileUser == username) {
                    balance = fileBalance; // Load balance for current user
                }
            }
        }
        void SaveUserData(string filename){
            // Implementation of saving user data to file
            ofstream userDataFile(filename);
            if (!userDataFile.is_open()) {
                cerr << "Error: Unable to open " << filename << " for writing" << endl;
                return;
            }
            // Write user data to file
            userDataFile << username << "," 
                        <<  Users[username]["Email"] << "," 
                        <<  Users[username]["Password"] << "," 
                        <<  Users[username]["Signup Time"] << "," 
                        << Logins[username]["Login Status"] << "," 
                        << Logins[username]["Login Time"] << ","
                        << balance << ","
                        << endl;
            userDataFile.close();
        }

};

class Transaction{
    public:
        string type; // income or expense
        double amount;
        string date;
        string description;

        void AddTransaction(string t_type, double t_amount, string t_date, string t_description);
        void ViewTransactions(string filter_type = "", string filter_date = "");
        void DeleteTransaction(int transaction_id);
        bool IsTransactionValid(double t_amount);
        void SaveTransactionsToFile(string filename);
        void LoadTransactionsFromFile(string filename);
};

class Exports{
    public:
        void ExportToCSV(string filename, const vector<Transaction>& transactions);
        void ExportToPDF(string filename, const vector<Transaction>& transactions);
};

class Reports{
    public:
        void GenerateSpendingReport(const vector<Transaction>& transactions, string timeframe);
        void GenerateIncomeReport(const vector<Transaction>& transactions, string timeframe);
        void GenerateBudgetReport(const User& user);
        void GenerateHealthReport(const User& user, const vector<Transaction>& transactions);
        void VisualizeData(const vector<Transaction>& transactions);
};

int main()
{
    // sf::RenderWindow window(sf::VideoMode({200, 200}), "SFML works!");
    // sf::CircleShape shape(100.f);
    // shape.setFillColor(sf::Color::Green);

    // while (window.isOpen())
    // {
    //     while (const std::optional event = window.pollEvent())
    //     {
    //         if (event->is<sf::Event::Closed>())
    //             window.close();
    //     }

    //     window.clear();
    //     window.draw(shape);
    //     window.display();
    // }

    return 0;
}