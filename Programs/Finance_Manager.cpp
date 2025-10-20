#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <string>
using namespace sf;
using namespace std;

// Class and Function definitions would go here
class User{
    public:
        string username;
        string password;
        double balance;
        
        void Login(string user, string pass);
        void Signup(string user, string pass);
        void Logout(string user);
        bool Authentication(string user, string pass, bool newAccount);
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