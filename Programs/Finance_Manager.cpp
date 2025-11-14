#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <sstream>
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
    string userID;
    string transactionNo;
    string type; // income or expense
    double amount;
    string date;
    string description;
    time_t timeCreated; // to manage revocation time limit
};

// User and Transaction Data Vectors
vector<User> users;
vector<Transaction> transactions;


// Functions Declarations

// User authentication and login management functions
bool loginUser(string username, string password);
bool logoutUser(string userID);
char signUpUser(string username, string password, string email);
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
bool updateUser(const User &userToUpdate, const string &filePath);
User getUser(const string &username);
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
void ExportMonthlyReport(const string &userID, int month, string year);




int main()
{
    // All relevant file paths
    const string UserDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\UserData.csv";
    const string TransactionsDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\TransactionsData.csv";

    // // Load existing users and transactions from files
    getAllUsers(UserDataFilePath, users);
    LoadAllTransactions(TransactionsDataFilePath, transactions);

    // Your main program logic here : Logic built around Command Line Interface. GUI wrapper will be applied later using SFML.
    // User login, signup, logout functionalities would be called based on user interactions

    for (User &user : users){
        user.LoggedIn = false; // Set all users to logged out on program start
    }


    string username, password,email;
    cout << "\tWelcome to the Personal Finance Manager!" << endl;
    cout << "\tPlease login or sign up to continue. Enter L for login or S for sign up: ";
    char choice;
    cin >> choice;
    choice = toupper(choice);

    switch (choice) {
        case 'L': {
            cout << "\tEnter username: ";
            cin >> username;
            cout << "\tEnter password: ";
            cin >> password;
            do {
                if (loginUser(username, password) == false) {
                    cout << "\tINCORRECT USERNAME OR PASSWORD! Please try again." << endl;
                    cout << "\tEnter username: ";
                    cin >> username;
                    cout << "\tEnter password: ";
                    cin >> password;
                }
            }while (!loginUser(username, password));
            cout << "\tUser " << username << " logged in successfully." << endl;
            updateUser(getUser(username), UserDataFilePath);
            break;
        }
        case 'S': {
            cout << "\tEnter desired username: ";
            cin >> username;
            cout << "\tEnter desired password: ";
            cin >> password;
            cout << "\tEnter your email: ";
            cin >> email;
            char signUpStatus = signUpUser(username, password, email);
            while (signUpStatus != 's'){
                if (signUpStatus == 'u') {
                        cout << "\tUsername already taken! Please try a different username." << endl;
                        cout << "\tEnter desired username: ";
                        cin >> username;
                } else if (signUpStatus == 'p') {
                    cout << "\tPassword does not meet criteria! Password should contain at least 8 characters, contains uppercase, lowercase, digit, and special character. " << endl;
                    cout << "\tPlease try again!" << endl;
                    cout << "\tEnter desired password: ";
                    cin >> password;
                } else if (signUpStatus == 'e') {
                    cout << "\tInvalid email address! Please enter a valid email." << endl;
                    cout << "\tEnter your email: ";
                    cin >> email;
                }
            }
            addUserToFile(users.back(), UserDataFilePath);
            break;
        }
        default:
            cout << "\tInvalid choice. Exiting program." << endl;
            return 0;
    }

    
    while (true) {
        bool exitProgram = false;
        User currentUser = getUser(username);
        string switchChoice;
        if (currentUser.LoggedIn != 0){
            cout << "\tWelcome " << currentUser.username << " to the Personal Finance Manager!" << endl;
            cout << "\tPlease select an option:" << endl;
            cout << "\t1. View Account Details" << endl;
            cout << "\t2. Update Account Details" << endl;
            cout << "\t3. Manage Transactions" << endl;
            cout << "\t4. Logout" << endl;
            cout << "\t5. Exit Program" << endl;
            cout << "\tEnter your choice (1-5): ";
            int option;
            cin >> option;

            string t_type, t_description, t_transactionNo;
            double t_amount;

            switch (option) {
                case 1:
                    // View account details
                    while (true){
                        bool backToMenu = false;
                        // Choice for viewing balance, monthly budget, savings goal
                        cout << "\tPlease select an option to view:" << endl;
                        cout << "\t1. View Balance" << endl;
                        cout << "\t2. View Monthly Budget" << endl;
                        cout << "\t3. View Savings Goal" << endl;
                        cout << "\t4. View Prediction to Reach Savings Goal" << endl;
                        cout << "\t5. Export Monthly Report" << endl;
                        cout << "\t6. Back to Main Menu" << endl;
                        cout << "\tEnter your choice (1-6): ";
                        int viewOption;
                        cin >> viewOption;

                        switch (viewOption) {
                            case 1:
                                cout << endl;
                                cout << "\tCurrent Balance: $" << GetBalance(currentUser.userID) << endl;
                                break;
                            case 2:
                                cout << endl;
                                cout << "\tMonthly Budget: $" << GetMonthlyBudget(currentUser.userID) << endl;
                                break;
                            case 3:
                                cout << endl;
                                cout << "\tSavings Goal: $" << GetSavingsGoal(currentUser.userID) << endl;
                                break;
                            case 4:
                                {
                                    double balance = GetBalance(currentUser.userID);
                                    double savingsGoal = GetSavingsGoal(currentUser.userID);
                                    double monthlyBudget = GetMonthlyBudget(currentUser.userID);
                                    double monthlySavings = monthlyBudget * 0.2; // Assuming 20% of ballance is saved monthly
                                    cout << endl;
                                    if (monthlySavings <= 0) {
                                        cout << "\tYou need to have a positive monthly savings to reach your goal." << endl;
                                    } else {
                                        double monthsNeeded = (savingsGoal - balance) / monthlySavings;
                                        if (monthsNeeded < 0) {
                                            cout << "\tCongratulations! You have already reached your savings goal." << endl;
                                        } else {
                                            cout << "\tAt your current savings rate, you will reach your savings goal in approximately " 
                                                << ceil(monthsNeeded) << " months." << endl;
                                        }
                                    }
                                }
                                break;
                            case 5:
                                {
                                    // provide option to user to export monthly reports of month past, based on current month and year
                                    // It should only provide option for report since the month of user signup
                                    string currentUserSignUpTime = currentUser.SignUptime;
                                    string monthSignup = currentUserSignUpTime.substr(4,3);
                                    string yearSignup = currentUserSignUpTime.substr(currentUserSignUpTime.length() - 5,4);
                                    time_t now = time(0);
                                    tm *ltm = localtime(&now);
                                    int month = 1 + ltm->tm_mon; // tm_mon is 0-11

                                    map<string, int> monthMap = {{"Jan",1}, {"Feb",2}, {"Mar",3}, {"Apr",4}, {"May",5}, {"Jun",6},
                                                                {"Jul",7}, {"Aug",8}, {"Sep",9}, {"Oct",10}, {"Nov",11}, {"Dec",12}};
                                    
                                    cout << "\tSelect month to export report (since your signup in " << monthSignup << " " << yearSignup << "):" << endl;
                                    for (const auto &month : monthMap) {
                                        if (month.second >= monthMap[monthSignup] && yearSignup == to_string(2025 + ltm->tm_year)) {
                                            cout << "\t" << month.first << " " << yearSignup << endl;
                                        }
                                    }
                                    string selectedYear;
                                    string selectedMonth;
                                    cout << "\tEnter year from available year reports: ";
                                    cin >> selectedYear;
                                    cout << "\tEnter month from available month reports: ";
                                    cin >> selectedMonth;

                                    if (monthMap.find(selectedMonth) != monthMap.end()) {
                                        int monthNum = monthMap[selectedMonth];
                                        ExportMonthlyReport(currentUser.userID, monthNum, selectedYear);
                                        cout << "\tMonthly report for " << selectedMonth << " " << selectedYear << " exported successfully." << endl;
                                    } else {
                                        cout << "\tInvalid month selected." << endl;
                                    }
                                }
                                break;
                            case 6:
                                backToMenu = true;
                                break;
                            default:
                                cout << "\tInvalid option. Please try again." << endl;
                        }
                    if (backToMenu) break;
                    }
                    break;
                case 2:
                    // Update account details
                    // Choice for updating password, email, balance, monthly budget, savings goal
                    while (true){
                        bool backToMenu = false;
                        cout << "\tPlease select an option to update:" << endl;
                        cout << "\t1. Update Password" << endl;
                        cout << "\t2. Update Email" << endl;
                        cout << "\t3. Add to Balance" << endl;
                        cout << "\t4. Update Monthly Budget" << endl;
                        cout << "\t5. Update Savings Goal" << endl;
                        cout << "\t6. Back to Main Menu" << endl;
                        cout << "\tEnter your choice (1-6): ";
                        int updateOption;
                        cin >> updateOption;
                        switch(updateOption){
                            case 1: 
                                do {
                                    cout << "\tEnter new password: ";
                                    cin >> password;
                                }while(!UpdatePassword(currentUser.userID, password));
                                break;  
                            case 2:
                                do {
                                    cout << "\tEnter new email: ";
                                    cin >> email;
                                }while(!UpdateEmail(currentUser.userID, email));
                                break;
                            case 3:
                                double amountToAdd;
                                cout << "\tEnter amount to add: ";
                                cin >> amountToAdd;
                                UpdateBalance(currentUser.userID, amountToAdd);
                                break;
                            case 4:
                                double newBudget;
                                cout << "\tEnter new monthly budget: ";
                                cin >> newBudget;
                                SetMonthlyBudget(currentUser.userID, newBudget);
                                break;
                            case 5:
                                double newGoal;
                                cout << "\tEnter new savings goal: ";
                                cin >> newGoal;
                                SetSavingsGoal(currentUser.userID, newGoal);
                                break;
                            case 6:
                                backToMenu = true;
                                break;  
                        }
                        updateUser(currentUser, UserDataFilePath);
                        if (backToMenu) break;
                    }
                    break;
                case 3:
                    // Manage transactions
                    // Add transaction, revoke transaction (with time limit), view transaction history
                    // Provide option to revoke transsaction after Transaction is added, with time limit of 5 minutes
                    while (true){
                        bool backToMenu = false;
                        cout << "\tPlease select an option:" << endl;
                        cout << "\t1. Add Transaction" << endl;
                        cout << "\t2. Revoke Transaction(s)" << endl;
                        cout << "\t3. View Transaction History" << endl;
                        cout << "\t4. Back to main menu" << endl;
                        cout << "\tEnter your choice (1-4): ";
                        int transactionOption;
                        cin >> transactionOption;
                        time_t timeNow;
                        
                        switch (transactionOption){
                            case 1:
                                // Add Transaction
                                cout << "\tEnter transaction type (income/expense): ";
                                cin >> t_type;
                                cout << "\tEnter amount: ";
                                cin >> t_amount;
                                cout << "\tEnter description: ";
                                cin.ignore(); // to ignore the newline character left in the buffer
                                getline(cin, t_description);
                                if (CreateNewTransaction(currentUser.userID, t_type, t_amount, t_description)){
                                    cout << "\tTransaction added successfully." << endl;
                                }else{
                                    cout << "\tFailed to add transaction." << endl;
                                }
                                break;
                            case 2:
                                // Show all upto last 5 revokable transactions
                                cout << endl;
                                cout << "\tRevokable Transactions (within 5 minutes of creation):" << endl;
                                cout << "\t----------------------------------------" << endl;
                                cout << "\tUser ID   | Transaction No | Type | Amount | Date       | Description" << endl;

                                for (const Transaction &trans : transactions) {
                                    if (trans.timeCreated + 300 >= time(0) && trans.userID == currentUser.userID) {
                                        cout << "\t" << currentUser.userID << "          | "<< trans.transactionNo << "          | " << trans.type << " | $" << trans.amount << " | " << trans.date << " | " << trans.description << endl;
                                    }
                                }
                                cout << "\t-----------------------------------------------------------------------" << endl;
                                // Revoke Transaction
                                cout << "\tEnter transaction number to revoke: ";
                                getline(cin, t_transactionNo);
                                if (RevokeTransaction(currentUser.userID, t_transactionNo, TransactionsDataFilePath)){
                                    cout << "\tTransaction revoked successfully." << endl;
                                }else{
                                    cout << "\tFailed to revoke transaction. It may be past the revocation time limit or invalid transaction number." << endl;
                                }

                            case 3:
                                // View Transaction History
                                cout << endl;
                                cout << "\tTransaction History:" << endl;
                                cout << "\t----------------------------------------" << endl;
                                cout << "\tUser ID   | Transaction No | Type | Amount | Date       | Description" << endl;
                                cout << "\t-----------------------------------------------------------------------" << endl;
                                for (const Transaction &trans : transactions) {
                                    if (trans.userID == currentUser.userID) {
                                        cout << "\t" << currentUser.userID << "          | "<< trans.transactionNo << "          | " << trans.type << " | $" << trans.amount << " | " << trans.date << " | " << trans.description << endl;
                                    }
                                }
                                cout << "\t-----------------------------------------------------------------------" << endl;
                                break;
                            case 4:
                                backToMenu = true;
                                break;
                            }
                            if (backToMenu) break;
                        }
                    break;
                case 4:
                    // Logout
                    logoutUser(currentUser.userID);
                    updateUser(getUser(currentUser.username), UserDataFilePath);
                    break;
                case 5:
                    // Exit program
                    exitProgram = true;
                    cout << "\tExiting program." << endl;
                    break;
                default:
                    cout << "\tInvalid option. Please try again." << endl;
                    break;
            }
        }else{
            cout << "\tWelcome to the Personal Finance Manager!" << endl;
            cout << "\tPlease login or sign up to continue. Enter L for login, S for sign up, or E for exit: ";
            char choice;
            cin >> choice;
            choice = toupper(choice);

            switch (choice) {
                case 'L': {
                    cout << "\tEnter username: ";
                    cin >> username;
                    cout << "\tEnter password: ";
                    cin >> password;
                    do {
                        if (!loginUser(username, password)) {
                            cout << "\tINCORRECT USERNAME OR PASSWORD! Please try again." << endl;
                            cout << "\tEnter username: ";
                            cin >> username;
                            cout << "\tEnter password: ";
                            cin >> password;
                        }
                    }while (!loginUser(username, password));
                    cout << "\tUser " << username << " logged in successfully." << endl;
                    updateUser(getUser(username), UserDataFilePath);
                    break;
                }
                case 'S': {
                    cout << "\tEnter desired username: ";
                    cin >> username;
                    cout << "\tEnter desired password: ";
                    cin >> password;
                    cout << "\tEnter your email: ";
                    cin >> email;
                    char signUpStatus = signUpUser(username, password, email);
                    while (signUpStatus != 's'){
                        if (signUpStatus == 'u') {
                                cout << "\tUsername already taken! Please try a different username." << endl;
                                cout << "\tEnter desired username: ";
                                cin >> username;
                        } else if (signUpStatus == 'p') {
                            cout << "\tPassword does not meet criteria! Password should contain at least 8 characters, contains uppercase, lowercase, digit, and special character. " << endl;
                            cout << "\tPlease try again!" << endl;
                            cout << "\tEnter desired password: ";
                            cin >> password;
                        } else if (signUpStatus == 'e') {
                            cout << "\tInvalid email address! Please enter a valid email." << endl;
                            cout << "\tEnter your email: ";
                            cin >> email;
                        }
                    }
                    addUserToFile(users.back(), UserDataFilePath);
                    break;
                }
                case 'E':
                    exitProgram = true;
                    cout << "\tExiting program." << endl;
                    break;
                default:
                    cout << "\tInvalid choice. Exiting program." << endl;
                    return 0;
            }
        }
        if (exitProgram) break;
    }

    sf::Window window(sf::VideoMode({800, 600}), "Personal Finance Manager");
    sf::String userInput;
    const std::string fontPath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf";
    sf::Font font;
    if (!font.openFromFile(fontPath)) {
        return 1;
    }
    
    
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>()) window.close();

            // handle other events...
        }

        window.display();
    }

    
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
            return true;
        }
    }
    return false;
}
// Manages user logout
bool logoutUser(string userID){
    for(User &user : users){
        if(user.userID == userID && user.LoggedIn != 0){
            user.LoggedIn = false;
            time_t now = time(0);
            user.LoginLogoutTime = ctime(&now);
            cout << "\tUser " << user.username << " logged out successfully." << endl;
            return true;
        }
    }
    cout << "\tUser not found or not logged in." << endl;
    return false;
}
// Manages new user sign up
char signUpUser(string username, string password, string email){
    if(!isUsernameAvailable(username)){
        return 'u';
    }
    if(!isPasswordStrong(password)){
        return 'p';
    }
    if(!isEmailValid(email)){
        return 'e';
    }
    time_t now = time(0);
    User newUser;
    newUser.username = username;
    newUser.password = password;
    newUser.Email = email;
    newUser.balance = 0.0;
    newUser.MonthlyBudget = 0.0;
    newUser.SavingsGoal = 0.0;
    newUser.LoggedIn = true;
    newUser.SignUptime = ctime(&now);

    //Encrypt Password
    PasswordEncryption(newUser.password, 7); // Simple Caesar cipher with shift of 7
    // Generate unique user ID
    generateUniqueUserID(newUser);
    addUser(newUser);
    cout << "\tUser signed up successfully. User ID: " << newUser.userID << endl;
    return 's';
}
// Authenticates user credentials
bool authenticateUser(string username, string password){
    // check if username matches and password after decryption matches
    string encryptedPassword = password;
    PasswordEncryption(encryptedPassword, 7); // Encrypt entered password to compare the stored encrypted password for authentication
    for(const User &user : users){
        if(user.username == username){
            if(user.password == encryptedPassword){
                return true;
            }
        }
        return false;
    }
    return false;
}
// Check if password meets criteria
bool isPasswordStrong(string password){
    // Criteria: at least 8 characters, contains uppercase, lowercase, digit, and special character
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
    srand(static_cast<unsigned int>(time(0)));
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
    << user.LoginLogoutTime << "," << endl;
    }
    outFile.close();
    return true;
}
// Retrieves user details based on username
User getUser(const string &username){
    for(User &user : users){
        if(user.username == username){
            return user;
        }
    }
    return User(); // Return default User if not found
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
        if (line.length() == 1) continue; // Skip comma lines 
        User user;
        stringstream ss(line);
        int fieldIndex = 0;
        while(getline(ss, line, ',')){
            switch(fieldIndex){
                case 0:
                    user.userID = line;
                    break;
                case 1:
                    user.username = line;
                    break;
                case 2:
                    user.password = line;
                    break;
                case 3:
                    user.Email = line;
                    break;
                case 4:
                    user.balance = stod(line);
                    break;
                case 5:
                    user.MonthlyBudget = stod(line);
                    break;
                case 6:
                    user.SavingsGoal = stod(line);
                    break;
                case 7:
                    user.LoggedIn = stoi(line);
                    break;
                case 8:
                    user.SignUptime = line;
                    break;
                case 9:
                    user.LoginLogoutTime = line;
                    break;
            }
            fieldIndex++;
        }
        loadedUsers.push_back(user);
        UniqueUserIDs.push_back(user.userID);

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
    newTransaction.timeCreated = now;
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
    // reads each line from the TransactionsData.csv file, parses the comma-separated values, and populates the loadedTransactions vector with Transaction struct instances
    string line;
    while(getline(inFile, line)){
       Transaction trans;
        stringstream ss(line);
        int fieldIndex = 0;
        while(getline(ss, line, ',')){
            switch(fieldIndex){
                case 0:
                    trans.userID = line;
                    break;
                case 1:
                    trans.transactionNo = line;
                    break;
                case 2:
                    trans.type = line;
                    break;
                case 3:
                    trans.description = line;
                    break;
                case 4:
                    trans.amount = stod(line);
                    break;
                case 5:
                    trans.date = line;
                    break;
            }
            fieldIndex++;
        }
        loadedTransactions.push_back(trans);
    }
    inFile.close();
    return;
}
// Exports monthly report for user
void ExportMonthlyReport(const string &userID, int month, string year){
    string Months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    // filepath format: "Month_Year_UserID_MonthlyReport.csv"
    string filePath = Months[month - 1] + "_" + year + "_" + userID + "_MonthlyReport.csv";
    ofstream outFile(filePath);
    if(!outFile){
        cerr << "Error opening file for writing." << endl;
        return;
    }
    outFile << "Monthly Report for User ID: " << userID << endl;
    outFile << "----------------------------------------" << endl;
    outFile << "Transaction No, Type, Description, Amount, Date" << endl;
    for(const Transaction &trans : transactions){
        if(trans.userID == userID){
            outFile << trans.transactionNo << ", " << trans.type << ", " << trans.description << ", " << trans.amount << ", " << trans.date << endl;
        }
    }
    outFile.close();
    cout << "Monthly report exported successfully for user ID " << userID << endl;
    return;
}
