#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <string>
#include <map>
using namespace sf;
using namespace std;

// Class and Function definitions would go here
class User{
    public:
        string username;
        string password;
        double balance;
        map<string, string> Logins; // Track login status
        map<string, string> Users;
        
        bool Login(string user, string pass){ // Add Save to, and Load from File Functionality
            // Implementation of login functionality
            if (Authentication(user, pass, false)) {
                cout << "Login successful for user: " << user << endl;
                Logins[user] = "Login"; // Track successful login
                return true;
            } else {
                cout << "Login failed for user: " << user << endl;
                return false;
            }
        }
        bool Signup(string user, string pass){ // Add Save to File Functionality
            // Implementation of signup functionality
            if (IsUsernameAvailable(user) && IsPasswordValid(pass)) {
                Users[user] = pass;
                cout << "Signup successful for user: " << user << endl;
                return true;
            } else {
                cout << "Signup failed for user: " << user << endl;
                return false;
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
        void Logout(string user){
            // Implementation of logout functionality
            Logins[user] = "Logout"; // Track logout
            cout << "User " << user << " logged out successfully." << endl;
        }
        bool Authentication(string user, string pass, bool newAccount){
            // Implementation of authentication functionality
            auto FoundUser = Users.find(user);
            // Check if user exists and password matches
            // FoundUser points to a pair of (username, password) in the Users map
            // If user is found and password matches, FoundUser != Users.end() returns true
            // FoundUser->second points to the second value in the pair, stored in the map, which is the password
            if (FoundUser != Users.end() && FoundUser->second == pass) {
                return true;
            }
            return false;
        }
        void UpdateBalance(double amount);
        void viewBalance(string user);
        void SetBudget(double amount);
        void SetFinancialGoals(string user, string goal);
        void ViewFinancialGoals(string user);
        void LoadUserData(string filename);
        void UpdatePassword(string user, string oldPass, string newPass);   
        void SaveChanges(string filename);

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