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
bool isPasswordStrong(string password);
bool isEmailValid(string email);
bool isUsernameAvailable(string username);
void generateUniqueUserID(User &newUser);
bool UpdatePassword(string userID, string newPassword);
bool UpdateEmail(string userID, string newEmail);
void PasswordEncryption(string &password, int shift);
void PasswordDecryption(string &password, int shift);

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

// User authentication and login management functions

// Manages user login
bool loginUser(string username, string password){
    for(User &user : users){
        if(authenticateUser(username, password)){
            user.LoggedIn = true;
            time_t now = time(0);
            user.LoginLogoutTime = ctime(&now);
            cout << "User " << username << " logged in successfully." << endl;
            return true;
        }
    }
    cout << "Invalid username or password." << endl;
    return false;
}
// Manages user logout
bool logoutUser(string userID){
    for(User &user : users){
        if(user.userID == userID && user.LoggedIn){
            user.LoggedIn = false;
            time_t now = time(0);
            user.LoginLogoutTime = ctime(&now);
            cout << "User " << user.username << " logged out successfully." << endl;
            return true;
        }
    }
    cout << "User not found or not logged in." << endl;
    return false;
}
// Manages new user sign up
bool signUpUser(string username, string password, string email){
    if(!isUsernameAvailable(username)){
        cout << "Username already taken." << endl;
        return false;
    }
    if(!isPasswordStrong(password)){
        cout << "Password does not meet criteria." << endl;
        return false;
    }
    if(!isEmailValid(email)){
        cout << "Invalid email address." << endl;
        return false;
    }
    User newUser;
    newUser.username = username;
    newUser.password = password;
    newUser.Email = email;
    newUser.balance = 0.0;
    newUser.MonthlyBudget = 0.0;
    newUser.SavingsGoal = 0.0;
    newUser.LoggedIn = false;
    // Generate unique user ID
    generateUniqueUserID(newUser);
    users.push_back(newUser);
    UniqueUserIDs.push_back(newUser.userID);
    cout << "User signed up successfully. User ID: " << newUser.userID << endl;
    return true;
}
// Manages user deletion
bool deleteUser(string userID){

    auto it = remove_if(users.begin(), users.end(), [&](const User &user){ return user.userID == userID; });
    if(it != users.end()){
        users.erase(it, users.end());
        cout << "User with ID " << userID << " deleted successfully." << endl;
        return true;
    }
    cout << "User with ID " << userID << " not found." << endl;
    return false;
}
// Authenticates user credentials
bool authenticateUser(string username, string password){
    // check if username matches and password after decryption matches
    for(const User &user : users){
        if(user.username == username){
            string decryptedPassword = password;
            PasswordDecryption(decryptedPassword, 7); // Decrypt with the same shift used in encryption
            if(user.password == decryptedPassword){
                return true;
            }
        }
        return false;
    }
    return false;
}
// Check if password meets criteria
bool isPasswordStrong(string password){
    // Example criteria: at least 8 characters, contains uppercase, lowercase, digit, and special character
    if(password.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for(char c : password){
        if(isupper(c)) hasUpper = true;
        else if(islower(c)) hasLower = true;
        else if(isdigit(c)) hasDigit = true;
        else hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}
// Check is email is valid
bool isEmailValid(string email){
    // Simple email validation
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);
    return atPos != string::npos && dotPos != string::npos && dotPos > atPos;
}
// Checks if the username is available
bool isUsernameAvailable(string username){
    for(const User &user : users){
        if(user.username == username){
            return false;
        }
    }
    return true;
}
// Generates a unique user ID
void generateUniqueUserID(User &newUser){
    // User ID generation logic
    // Each user ID follows the format "UXXXX" where XXXX is a unique 4-digit number
    // validate uniqueness against existing user IDs, checking via UniqueUserIDs vector
    string userID;
    do{
        int randomNum = rand() % 10000; // Generate a random number between 0 and 9999
        userID = "U" + to_string(1000 + randomNum).substr(1); // Ensure it's 4 digits
    } while(find(UniqueUserIDs.begin(), UniqueUserIDs.end(), userID) != UniqueUserIDs.end());

    newUser.userID = userID;
}
// Updates user password
bool UpdatePassword(string userID, string newPassword){
    for(User &user : users){
        if(user.userID == userID){
            if(!isPasswordStrong(newPassword)){
                cout << "New password does not meet criteria." << endl;
                return false;
            }
            user.password = newPassword;
            cout << "Password updated successfully for user ID " << userID << endl;
            return true;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return false;
}
// Updates user email
bool UpdateEmail(string userID, string newEmail){
    for(User &user : users){
        if(user.userID == userID){
            if(!isEmailValid(newEmail)){
                cout << "New email address is invalid." << endl;
                return false;
            }
            user.Email = newEmail;
            cout << "Email updated successfully for user ID " << userID << endl;
            return true;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return false;
}
// Manages password encryption
void PasswordEncryption(string &password, int shift){
    for(char &c : password){
        c = c + shift; // Simple Caesar cipher for demonstration
    }
    return;
}
// Manages password decryption
void PasswordDecryption(string &password, int shift){
    for(char &c : password){
        c = c - shift; // Reverse the Caesar cipher
    }
    return;
}

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
