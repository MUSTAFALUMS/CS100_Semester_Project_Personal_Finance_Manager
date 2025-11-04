#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;
using namespace sf;


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
    string userID;
    string transactionNo;
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
bool addUserToFile(const User &newUser, const string &filePath);
bool removeUserFromFile(const string &userID, const string &filePath);
void addUser(const User &newUser);
void deleteUser(const string &userID);
bool updateUser(const User &userToUpdate);
User* getUser(const string &userID);
void getAllUsers(const string &filepath, vector<User> &LoadedUsers);

// User finance management functions
bool UpdateBalance(const string &userID, double amount);
bool SetMonthlyBudget(const string &userID, double budget);
bool SetSavingsGoal(const string &userID, double goal);
double GetBalance(const string &userID);
double GetMonthlyBudget(const string &userID);
double GetSavingsGoal(const string &userID);

// Transaction management functions
void generateTransactionNumber(Transaction &newTransaction);
bool CreateNewTransaction(const string &userID, const string &type, double amount, const string &description);
bool RevokeTransaction(const string &userID, const string &transactionNumber, const string &filePath);
bool SaveAllTransactions(const vector<Transaction> &userTransactions, const string &filePath);
void LoadAllTransactions(const string &filePath, vector<Transaction> &loadedTransactions);

// function prototype for loading data from bank statement
bool LoadDataFromBankStatement(const string userID, const string filePath);


int main()
{
    // All relevant file paths
    const string UserDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\UserData.csv";
    const string TransactionsDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\Programs\\TransactionsData.csv";

    // // Load existing users and transactions from files
    getAllUsers(UserDataFilePath, users);
    LoadAllTransactions(TransactionsDataFilePath, transactions);

    // Your main program logic here
    // Part 1: User Authentication and Management


    // SFML window setup and event handling would go here
    /*
        SFML window created which display opens for login/signup on run
        User can enter credentials to login or choose to sign up
        If user login/ signup successful, proceed to main finance management interface
        If user credentials invalid, show error message and allow retry
        If new user signs up, validate inputs, create new user entry, save to UserData.csv
    */
    // sf::RenderWindow window(sf::VideoMode({800, 600}), "Personal Finance Manager");
    
    // if(!window.isOpen()){
    //     cout << "Failed to create window!" << endl;
    //     return -1;
    // }
        
    // while(window.isOpen()){
    //     while(const auto event = window.pollEvent()){
    //         if(event->is<sf::Event::Closed>()){
    //             cout << "Closing window..." << endl;
    //             window.close();
    //         }
    //     }
        
    //     window.clear(sf::Color::Blue);  // Try blue to make it obvious
    //     window.display();
    // }
    
    return 0;
    
    // User login, signup, logout functionalities would be called based on user interactions


    




    
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
    time_t now = time(0);
    User newUser;
    newUser.username = username;
    newUser.password = password;
    newUser.Email = email;
    newUser.balance = 0.0;
    newUser.MonthlyBudget = 0.0;
    newUser.SavingsGoal = 0.0;
    newUser.LoggedIn = false;
    newUser.SignUptime = ctime(&now);
    // Generate unique user ID
    generateUniqueUserID(newUser);
    addUser(newUser);
    cout << "User signed up successfully. User ID: " << newUser.userID << endl;
    return true;
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

// Adds user data to file
bool addUserToFile(const User &newUser, const string &filePath){
    ofstream outFile(filePath, ios::app);
    if(!outFile){
        cerr << "Error opening file for writing." << endl;
        return false;
    }
    outFile << newUser.userID << "," << newUser.username << "," << newUser.password << "," << newUser.Email << ","
    << newUser.balance << "," << newUser.MonthlyBudget << "," << newUser.SavingsGoal << "," << newUser.LoggedIn<< "," << newUser.SignUptime<< "," 
    << newUser.LoginLogoutTime << endl;
    outFile.close();
    return true;
}
// Removes user data from file
bool removeUserFromFile(const string &userID, const string &filePath){
    ifstream inFile(filePath);
    if(!inFile){
        cerr << "Error opening file for reading." << endl;
        return false;
    }
    vector<string> lines;
    string line;
    while(getline(inFile, line)){
        if(line.find(userID) == string::npos){
            lines.push_back(line);
        }
    }
    inFile.close();

    ofstream outFile(filePath);
    if(!outFile){
        cerr << "Error opening file for writing." << endl;
        return false;
    }
    for(const string &l : lines){
        outFile << l;
    }
    outFile.close();
    return true;
}
// Adds new user to appropriate data structures
void addUser(const User &newUser){
    users.push_back(newUser);
    UniqueUserIDs.push_back(newUser.userID);
    return;
}
// Manages user deletion
void deleteUser(const string &userID){
    users.erase(remove_if(users.begin(), users.end(), [&userID](const User &user){ return user.userID == userID; }),users.end());
    UniqueUserIDs.erase(remove(UniqueUserIDs.begin(), UniqueUserIDs.end(), userID), UniqueUserIDs.end());
    return;
}
// Updates user details to user file and vector
bool updateUser(const User &userToUpdate, const string &filePath){
    // updates user details in the UserData.csv file, based on changes made in the User struct instance passed as argument
    // does this by searching for the userID in the users vector and updating the corresponding User instance
    // then it updates the UserData.csv file by rewriting it with the updated user details

    // Update user in users vector
    for(User &user : users){
        if(user.userID == userToUpdate.userID){
            user = userToUpdate;
            break;
        }
    }

    // Rewrite the UserData.csv file with updated user details
    ofstream outFile(filePath);
    if(!outFile){
        cerr << "Error opening file for writing." << endl;
        return false;
    }
    for(const User &user : users){
        outFile << user.userID << "," << user.username << "," << user.password << "," << user.Email << ","
        << user.balance << "," << user.MonthlyBudget << "," << user.SavingsGoal << "," << user.LoggedIn<< "," << user.SignUptime<< "," 
        << user.LoginLogoutTime << endl;
    }
    outFile.close();
    return true;
}
// Retrieves user details based on userID
User* getUser(const string &userID){
    for(User &user : users){
        if(user.userID == userID){
            return &user;
        }
    }
    return nullptr;
}
// Retrieves all users
void getAllUsers(const string &filepath, vector<User> &loadedUsers){
    ifstream inFile(filepath);
    if(!inFile){
        cerr << "Error opening file for reading." << endl;
        return;
    }
    string line;
    while(getline(inFile, line)){
        User user;
        size_t pos = 0;
        vector<string> tokens;
        while((pos = line.find(',')) != string::npos){
            tokens.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }
        tokens.push_back(line); // last token

        if(tokens.size() == 10){
            user.userID = tokens[0];
            user.username = tokens[1];
            user.password = tokens[2];
            user.Email = tokens[3];
            user.balance = stod(tokens[4]);
            user.MonthlyBudget = stod(tokens[5]);
            user.SavingsGoal = stod(tokens[6]);
            user.LoggedIn = (tokens[7] == "1" || tokens[7] == "true");
            user.SignUptime = tokens[8];
            user.LoginLogoutTime = tokens[9];

            loadedUsers.push_back(user);
            UniqueUserIDs.push_back(user.userID);
        }
    }
    inFile.close();
    
}

// User finance management functions

// Updates user balance
bool UpdateBalance(const string &userID, double amount){
    for(User &user : users){
        if(user.userID == userID){
            user.balance += amount;
            cout << "Balance updated successfully to " << user.balance << " for user " << user.username << endl;
            return true;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return false;
}
// Sets monthly budget for user
bool SetMonthlyBudget(const string &userID, double budget){
    for(User &user : users){
        if(user.userID == userID){
            user.MonthlyBudget = budget;
            cout << "Monthly budget updated to " << budget << " successfully for user " << user.username << endl;
            return true;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return false;
}
// Sets savings goal for user
bool SetSavingsGoal(const string &userID, double goal){
    for(User &user : users){
        if(user.userID == userID){
            user.SavingsGoal = goal;
            cout << "Savings goal updated successfully to " << goal << " for user " << user.username << endl;
            return true;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return false;
}
// Retrieves user balance
double GetBalance(const string &userID){
    for(const User &user : users){
        if(user.userID == userID){
            return user.balance;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return -1;  // Meaningful value indicating user not found
}
// Retrieves user monthly budget
double GetMonthlyBudget(const string &userID){
    for(const User &user : users){
        if(user.userID == userID){
            return user.MonthlyBudget;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return -1;  // Meaningful value indicating user not found
}
// Retrieves user savings goal
double GetSavingsGoal(const string &userID){
    for(const User &user : users){
        if(user.userID == userID){
            return user.SavingsGoal;
        }
    }
    cout << "User with ID " << userID << " not found." << endl;
    return -1;  // Meaningful value indicating user not found
}

// Transaction management functions

// Generates a unique transaction number
void generateTransactionNumber(Transaction &newTransaction){
    // Generate a transaction number
    int transactionNum = 0;
    // search existing transactions to ensure uniqueness for a particular user, using transactions vector and userID attribute
    // check for the largest transaction number for that user and increment by 1
    for(const Transaction &trans : transactions){
        if(trans.userID == newTransaction.userID){
            int existingNum = stoi(trans.transactionNo.substr(1)); // Assuming transactionNo starts with a letter followed by number
            if(existingNum >= transactionNum){
                transactionNum = existingNum + 1;
            }
        }
    }
    newTransaction.transactionNo = "T" + to_string(transactionNum);
}
// Creates a new transaction
bool CreateNewTransaction(const string &userID, const string &type, double amount, const string &description){
    // Get current date
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string date = to_string(1900 + ltm->tm_year) + "-" + to_string(1 + ltm->tm_mon) + "-" + to_string(ltm->tm_mday);
    Transaction newTransaction;
    newTransaction.userID = userID;
    newTransaction.type = type;
    newTransaction.amount = amount;
    newTransaction.date = date;
    newTransaction.description = description;
    generateTransactionNumber(newTransaction);
    // Update user balance accordingly
    if(type == "income"){
        UpdateBalance(userID, amount);
    } else if(type == "expense"){
        if (!UpdateBalance(userID, -amount)){
            cout << "Failed to create transaction due to insufficient balance." << endl;
            return false;
        }else{
            UpdateBalance(userID, -amount);
        }
    } else{
        cout << "Invalid transaction type." << endl;
        return false;
    }
    transactions.push_back(newTransaction);
    return true;
}
// Removes a transaction from the user's transaction file and transaction vector and updates the user's balance accordingly
bool RevokeTransaction(const string &userID, const string &transactionNumber, const string &filePath){
    // Find transaction from transactions vector, remove it
    // Re-write the Transactions file with new, updated transactions vector
    auto it = remove_if(transactions.begin(), transactions.end(), [&userID, &transactionNumber](
        const Transaction &trans){
            return trans.userID == userID && trans.transactionNo == transactionNumber;
        });
    if(it != transactions.end()){
        Transaction transToRevoke = *it;
        // Update user balance accordingly
        if(transToRevoke.type == "income"){
            UpdateBalance(userID, -transToRevoke.amount);
        } else if(transToRevoke.type == "expense"){
            UpdateBalance(userID, transToRevoke.amount);
        }
        transactions.erase(it, transactions.end());
        // Rewrite the Transactions file
        SaveAllTransactions(transactions, filePath);
        cout << "Transaction " << transactionNumber << " revoked successfully for user ID " << userID << endl;
        return true;
    } else{
        cout << "Transaction " << transactionNumber << " not found for user ID " << userID << endl;
        return false;
    }
}
// Saves all transactions from the transactions vector to the user's transaction file
bool SaveAllTransactions(const vector<Transaction> &userTransactions, const string &filePath){
    ofstream outFile(filePath);
    if(!outFile){
        cerr << "Error opening file for writing." << endl;
        return false;
    }
    for(const Transaction &trans : userTransactions){
        outFile << trans.userID << "," << trans.transactionNo << "," << trans.type << "," << trans.description << "," << trans.amount << "," << trans.date << endl;
    }
    return true;
}
// Retrieves user transactions
void LoadAllTransactions(const string &filePath, vector<Transaction> &loadedTransactions){
    Transaction trans;
    ifstream inFile(filePath);
    if(!inFile){
        cerr << "Error opening file for reading." << endl;
        return;
    }
    string line;
    vector<string> tokens;
    while(getline(inFile, line)){
        size_t pos = 0;
        if ((pos == line.find(',')) != string::npos) {
            tokens.push_back(line.substr(0,pos));
            line.erase(0, pos + 1);
        }   
        tokens.push_back(line); // last token
    }
    inFile.close();

    // Assuming transaction file contains all necessary functions
    if (tokens.size() == 6){
        trans.userID = tokens[0];
        trans.transactionNo = tokens[1];
        trans.type = tokens[2]; 
        trans.description = tokens[3];
        trans.amount = stod(tokens[4]);
        trans.date = tokens[5];
    }
}
// Loads data from bank statement file
bool LoadDataFromBankStatement(const string userID, const string filePath);
