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
#include <functional>
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
    double SavingsAmount;
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
User getUser(string username);
void getAllUsers(const string &filepath, vector<User> &LoadedUsers);

// User finance management functions
bool UpdateBalance(const string &userID, double amount);
bool SetMonthlyBudget(const string &userID, string budget);
bool SetSavingsGoal(const string &userID, double goal);
double GetBalance(const string &userID);
double GetMonthlyBudget(const string &userID);
double GetSavingsGoal(const string &userID);

// Transaction management functions
void generateTransactionNumber(Transaction &newTransaction);
bool CreateNewTransaction(const string &userID, const string &type, double amount, const string &description);
bool RemoveTransaction(const string &userID, const string &transactionNumber, const string &filePath);
bool SaveAllTransactions(const vector<Transaction> &userTransactions, const string &filePath);
void LoadAllTransactions(const string &filePath, vector<Transaction> &loadedTransactions);
void ExportMonthlyReport(const string &userID, int month, string year);

// All relevant file paths
const string UserDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\UserData.csv";
const string TransactionsDataFilePath = "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\TransactionsData.csv";


// GUI elements

// Set font from file 
int setFont(sf::Font& font, const string &fontPath) {
    if (!font.openFromFile(fontPath)) {
        cerr << "Error loading font\n";
        return -1;
    }
    return 0;
}

// Page state enumeration
enum class PageState {
    LOGIN_SIGNUP,
    LOGIN_PAGE,
    SIGNUP_PAGE,
    DASHBOARD_PAGE,
    ERROR,
    VIEW_ACCOUNT_DETAILS_PAGE,
    SET_NEW_BUDGET,
    ADD_TO_BALANCE,
    UPDATE_ACCOUNT_CREDENTIALS_PAGE,
    UPDATE_SAVINGS_ACCOUNT_AMOUNT,
    UPDATE_SAVINGS_GOAL,
    UPDATE_PASSWORD,
    UPDATE_EMAIL,
    VIEW_SAVINGS_GOAL_AND_PREDICTIONS,
    EXPORT_MONTHLY_REPORT,
    MANAGE_TRANSACTIONS,
    ADD_TRANSACTION,
    REMOVE_TRANSACTION,
    LOGOUT_CONFIRMATION_PAGE
};

string errorMessage = "";
char errorFlag = '.';
string currentUserName = "";
string currentUserPassword = "";
string currentUserNewPassword = "";
string currentUserConfirmNewPassword = "";
string currentUserNewEmail = "";
string currentUserEmail = "";
User currentUser;
string currentUserMonthlyBudget = "0.0";
string currentUserSavingsGoal = "0.0";
string currentUserBalance = "0.0";
string currentUserSignUpTime = currentUser.SignUptime;
string currentUserSavingsAmount = "0.0";
string currentUserSavingsGoalPrediction = "";
string currentTransactionType = "";
string currentTransactionAmount = "";
string currentTransactionDescription = "";
string currentTransactionDate = "";
vector<Transaction> currentUserTransactions;


PageState currentPage = PageState::LOGIN_SIGNUP;

struct TextField{
    sf::RectangleShape box;
    sf::Font font;
    sf::Text text{font};
    sf::Text label{font};
    std::string input;
    bool isActive;
    bool isPassword;
    sf::Clock cursorClock;
    bool showCursor;

    TextField(const string& fontPath, float x, float y, float width, float height,
              const string& labelText, bool password = false)
    : isActive(false), isPassword(password), showCursor(true) {
        
        setFont(font, fontPath);
        
        box.setPosition({x, y});
        box.setSize({width, height});
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color::Black);
        
        label.setString(labelText);
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::White);
        label.setPosition({x, y - 30});
        
        text.setString("");
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setPosition({x + 10, y + 10});
    }
    
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x),
                                 static_cast<float>(mousePressed->position.y));
            isActive = box.getGlobalBounds().contains(mousePos);
            box.setOutlineColor(isActive ? sf::Color::Blue : sf::Color::Black);
        }
        
        if (isActive) {
            if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
                if (textEntered->unicode == 8 && !input.empty()) { // Backspace
                    input.pop_back();
                } else if (textEntered->unicode >= 32 && textEntered->unicode < 128) {
                    input += static_cast<char>(textEntered->unicode);
                }
                updateText();
            }
        }
    }
    
    void update() {
        if (isActive && cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
            updateText();
        }
    }
    
    void updateText() {
        std::string displayText = input;
        if (isPassword && !input.empty()) {
            displayText = std::string(input.length(), '*');
        }
        if (isActive && showCursor) {
            displayText += "|";
        }
        text.setString(displayText);
    }
    
    void render(sf::RenderWindow& window) {
        window.draw(label);
        window.draw(box);
        window.draw(text);
    }
    
    string getValue() const {
        return input;
    }
    
    void clear() {
        input = "";
        updateText();
    }    
};

struct Button {
    sf::RectangleShape shape;
    sf::Font font;
    sf::Text text{font};
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    std::function<void()> onClick;
    
    // Constructor
    Button(const std::string& fontPath, float x, float y, float width, float height, 
           const std::string& buttonText, sf::Color idle, sf::Color hover, sf::Color active, 
           std::function<void()> clickHandler = nullptr)
    : idleColor(idle), hoverColor(hover), activeColor(active), onClick(clickHandler) {
        
        if (!font.openFromFile(fontPath)) {
            std::cerr << "Error loading font in button" << std::endl;
            return;
        }
        
        shape.setPosition({x, y});
        shape.setSize({width, height});
        shape.setFillColor(idleColor);
        
        text.setString(buttonText);
        text.setFillColor(sf::Color::Black);
        text.setCharacterSize(24);
        
        // Center text within the button
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin({textBounds.position.x + textBounds.size.x/2.0f, 
                       textBounds.position.y + textBounds.size.y/2.0f});
        text.setPosition({x + width/2.0f, y + height/2.0f});
    }
    
    // Update function to handle interaction
    void update(sf::Vector2f mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(hoverColor);
        } else {
            shape.setFillColor(idleColor);
        }
    }
    
    // Handle click event
    void handleEvent(const sf::Event& event) {
        if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
            sf::Vector2f mousePos(static_cast<float>(mouseReleased->position.x), 
                                 static_cast<float>(mouseReleased->position.y));
            if (shape.getGlobalBounds().contains(mousePos)) {
                if (onClick) {
                    onClick();
                }
            }
        }
    }
    
    // Draw function
    void render(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};
vector<Button*>RemovableTransactionButtons;
bool removeTransactionButtonsCreated = false;  // ADD THIS FLAG

void onLoginButtonClick() {
    cout << "Login button clicked!" << endl;
    // Add login logic here
    // Transition to Login Page
    currentPage = PageState::LOGIN_PAGE;
}
void onSignupButtonClick() {
    cout << "Sign Up button clicked!" << endl;
    // Add signup logic here
    // Transition to Sign Up Page
    currentPage = PageState::SIGNUP_PAGE;
}
void onDashboardViaLoginButtonClick() {
    cout << "Dashboard button clicked!" << endl;
    // Authenticate user login here
    cout << currentUserPassword << endl;
    if (loginUser(currentUserName, currentUserPassword)) {
        cout << "Login successful!" << endl;
        // Transition to Dashboard Page
        currentUser = getUser(currentUserName);
        updateUser(currentUser, UserDataFilePath);
        errorFlag =  'L'; // login successful
        currentPage = PageState::DASHBOARD_PAGE;
    } else {
        cout << "Login failed!" << endl;
        errorMessage = "Invalid username or password!";
        errorFlag = 'l'; // login failed
        currentPage = PageState::ERROR;
    }
}
void onDashboardViaSignupButtonClick() {
    cout << "Dashboard button clicked!" << endl;
    // Add dashboard logic here

    char signUpResult = signUpUser(currentUserName, currentUserPassword, currentUserEmail);
    switch (signUpResult) {
        case 'u':
            errorMessage = "Username already taken!";
            errorFlag = 's'; // Signup failed
            currentPage = PageState::ERROR;
            break;
        case 'p':
            errorMessage = "Password is not strong enough!";
            cout << currentUserPassword << endl;
            errorFlag = 's'; // Signup failed
            currentPage = PageState::ERROR;
            break;
        case 'e':
            errorMessage = "Invalid email format!";
            errorFlag = 's'; // Signup failed
            currentPage = PageState::ERROR;
            break;
        case 's':
            errorFlag = 'S'; // Signup successful
            currentUser = users.back();
            cout << "User signed up successfully: " << currentUser.username << endl;
            addUserToFile(currentUser, UserDataFilePath);
            currentPage = PageState::DASHBOARD_PAGE;
            break;
        default:
            errorMessage = "Unknown error during sign up!";
            errorFlag = 's'; // Signup failed
            currentPage = PageState::ERROR;
            break;
    }
    cout << "Current Username: " << currentUserName << endl;
}
void onClickViewAccountDetails(){
    cout << "View Account Details button clicked!" << endl;
    // Add view account details logic here
    currentUser = getUser(currentUser.username);
    currentPage = PageState::VIEW_ACCOUNT_DETAILS_PAGE;

}
void onClickSetNewBudget(){
    cout << "Set New Budget button clicked!" << endl;
    // Add set new budget logic here
    currentPage = PageState::SET_NEW_BUDGET;
}
void onClickUpdateBudget(){
    cout << "Update Budget button clicked!" << endl;
    // Add update budget logic here
    currentUser = getUser(currentUserName);
    currentUser.MonthlyBudget = stod(currentUserMonthlyBudget);
    updateUser(currentUser, UserDataFilePath);
    cout << "Current User New Budget: " << currentUser.MonthlyBudget << endl;
    currentPage = PageState::VIEW_ACCOUNT_DETAILS_PAGE;
}
void onClickAddToBalance(){
    cout << "Add to Balance button clicked!" << endl;
    // Add add to balance logic here
    currentPage = PageState::ADD_TO_BALANCE;
}
void onClickUpdateBalance(){
    cout << "Update Balance button clicked!" << endl;
    // Add update balance logic here
    currentUser = getUser(currentUserName);
    currentUser.balance += stod(currentUserBalance);
    updateUser(currentUser, UserDataFilePath);
    cout << "Current User New Balance: " << currentUser.balance << endl;
    currentPage = PageState::VIEW_ACCOUNT_DETAILS_PAGE;
}
void onClickUpdateAccountCredentialsPage(){
    cout << "Update Account Info button clicked!" << endl;
    // Add update account info logic here
    currentPage = PageState::UPDATE_ACCOUNT_CREDENTIALS_PAGE;
}
void onClickSetSavingsAccountAmountPage(){
    cout << "Set Savings Goal button clicked!" << endl;
    // Add set savings goal logic here
    currentPage = PageState::UPDATE_SAVINGS_ACCOUNT_AMOUNT;
}
void onClickUpdateSavingsAccountAmount(){
    cout << "Update Savings Goal button clicked!" << endl;
    // Add update savings goal logic here
    currentUser = getUser(currentUserName);
    currentUser.SavingsAmount += stod(currentUserSavingsAmount);
    updateUser(currentUser, UserDataFilePath);
    cout << "Current User New Savings Amount: " << currentUser.SavingsAmount << endl;
    currentPage = PageState::VIEW_ACCOUNT_DETAILS_PAGE;
}
void onClickSetSavingGoalPage(){
    cout << "View Savings Goal Predictions button clicked!" << endl;
    // Add view savings goal predictions logic here
    currentPage = PageState::UPDATE_SAVINGS_GOAL;
}
void onClickUpdateSavingsGoal(){
    cout << "Update Savings Goal button clicked!" << endl;
    // Add update savings goal logic here
    currentUser = getUser(currentUserName);
    currentUser.SavingsGoal = stod(currentUserSavingsGoal);
    updateUser(currentUser, UserDataFilePath);
    cout << "Current User New Savings Goal: " << currentUser.SavingsGoal << endl;
    currentPage = PageState::VIEW_ACCOUNT_DETAILS_PAGE;
}
void onClickGoToExportMonthlyReportPage(){
    cout << "Export Monthly Report button clicked!" << endl;
    // Add export monthly report logic here
    currentPage = PageState::EXPORT_MONTHLY_REPORT;
}
void onClickExportMonthlyReport(string monthName){
    cout << "Export Monthly Report for month " << monthName << " button clicked!" << endl;
    // Add export monthly report logic here
    int month;
    map<string, int> monthMap = {{"Jan",1}, {"Feb",2}, {"Mar",3}, {"Apr",4}, {"May",5}, {"Jun",6},{"Jul",7}, {"Aug",8}, {"Sep",9}, {"Oct",10}, {"Nov",11}, {"Dec",12}};
    month = monthMap[monthName];
    ExportMonthlyReport(currentUser.userID, month, "2025");
    currentPage = PageState::DASHBOARD_PAGE;
}
void onClickManageTransactions(){
    cout << "Manage Transactions button clicked!" << endl;
    // Add manage transactions logic here
    currentPage = PageState::MANAGE_TRANSACTIONS;
}
void onClickAddTransactionPage(){
    cout << "Add Transaction Page button clicked!" << endl;
    // Add add transaction page logic here
    currentPage = PageState::ADD_TRANSACTION;
}
void onClickRemoveTransactionPage(){
    cout << "Remove Transaction Page button clicked!" << endl;
    // Add remove transaction page logic here
    currentPage = PageState::REMOVE_TRANSACTION;
}
void onClickAddNewTransaction(){
    cout << "Add New Transaction button clicked!" << endl;
    // Add new transaction logic here
    currentUser = getUser(currentUserName);
    CreateNewTransaction(currentUser.userID, currentTransactionType, stod(currentTransactionAmount), currentTransactionDescription);
    SaveAllTransactions(transactions, TransactionsDataFilePath);
    LoadAllTransactions(TransactionsDataFilePath, currentUserTransactions);
    currentPage = PageState::MANAGE_TRANSACTIONS;
}
void onClickRemoveTransaction(){
    cout << "Remove Transaction button clicked!" << endl;
    // Add remove transaction logic here
    currentUser = getUser(currentUserName);
    RemoveTransaction(currentUser.userID, currentTransactionDescription, TransactionsDataFilePath);
    SaveAllTransactions(transactions, TransactionsDataFilePath);
    updateUser(currentUser, UserDataFilePath);

    // Clear buttons and reset flag
    for (auto* btn : RemovableTransactionButtons) {
        delete btn;
    }
    RemovableTransactionButtons.clear();
    removeTransactionButtonsCreated = false;  // Reset flag

    currentPage = PageState::MANAGE_TRANSACTIONS;
}
void onClicklogout(){
    cout << "Logout button clicked!" << endl;
    // logout logic here
    logoutUser(currentUser.userID);
    currentUser.LoggedIn = false;
    updateUser(currentUser, UserDataFilePath);
    // GO BACK TO LOGIN/SIGNUP PAGE
    currentPage = PageState::LOGIN_SIGNUP;
}
void onClickConfirmLogout(){
    cout << "Confirm Logout button clicked!" << endl;
    // change current page to logout confirmation page
    // user asked to confirm logout
    currentPage = PageState::LOGOUT_CONFIRMATION_PAGE;
}
void onClickCancelLogout(){
    cout << "Cancel Logout button clicked!" << endl;
    // change current page back to dashboard page
    currentPage = PageState::DASHBOARD_PAGE;
}
void onClickUpdatePasswordPage(){
    cout << "Update Password Page button clicked!" << endl;
    // Add update password logic here
    currentPage = PageState::UPDATE_PASSWORD;
}
void onClickUpdatePassword(){
    cout << "Update Password button clicked!" << endl;
    // Add update password logic here
    currentUser = getUser(currentUserName);
    if (currentUserNewPassword == currentUserConfirmNewPassword) {
        if (currentUserNewPassword == currentUser.password) {
            errorMessage = "New password cannot be the same as the old password!";
            errorFlag = 'p'; // password update failed
            currentPage = PageState::ERROR;
        } else if (!isPasswordStrong(currentUserNewPassword)) {
            errorMessage = "Password is not strong enough!";
            errorFlag = 'p'; // password update failed
            currentPage = PageState::ERROR;
        } else {
            currentUser.password = currentUserNewPassword;
            PasswordEncryption(currentUser.password, 7); // Encrypt the new password
            errorFlag = 'P'; // password update successful
            updateUser(currentUser, UserDataFilePath);
            cout << "Current User New Password: " << currentUser.password << endl;
            cout << "Logging out user after password change." << endl;
            logoutUser(currentUser.userID);
            currentUser.LoggedIn = false;
            updateUser(currentUser, UserDataFilePath);
            currentPage = PageState::LOGIN_SIGNUP;
        }
    }else {
        errorMessage = "New password and confirm password do not match!";
        errorFlag = 'p'; // password update failed
        currentPage = PageState::ERROR;
    }
}
void onClickUpdateEmailPage(){
    cout << "Update Email button clicked!" << endl;
    // Add update email logic here
    currentPage = PageState::UPDATE_EMAIL;
}
void onClickUpdateEmail(){
    cout << "Update Email button clicked!" << endl;
    // Add update email logic here
    currentUser = getUser(currentUserName);
    if (currentUserNewEmail == currentUser.Email) {
        errorMessage = "New email cannot be the same as the old email!";
        errorFlag = 'e'; // email update failed
        currentPage = PageState::ERROR;
    } else if (!isEmailValid(currentUserNewEmail)) {
        errorMessage = "Invalid email format!";
        errorFlag = 'e'; // email update failed
        currentPage = PageState::ERROR;
    } else {
        currentUser.Email = currentUserNewEmail;
        errorFlag = 'E'; // email update successful
        updateUser(currentUser, UserDataFilePath);
        cout << "Current User New Email: " << currentUser.Email << endl;
        currentPage = PageState::DASHBOARD_PAGE;
    }
}

int main() {
    // // Load existing users and transactions from files
    getAllUsers(UserDataFilePath, users);
    LoadAllTransactions(TransactionsDataFilePath, transactions);

    currentUser.LoggedIn = true;

    sf::RenderWindow window(sf::VideoMode({900, 650}), "Personal Finance Manager");
    window.setFramerateLimit(60);
    sf::Font font;
    setFont(font, "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf"); // Ensure times.ttf is in the working directory
    
    //  LOGIN_SIGNUP PAGE Buttons
    Button loginButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       250, 500, 150, 50, "LOGIN", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green, 
                        [](){ onLoginButtonClick(); });

    Button signUpButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       480, 500, 150, 50, "SIGN UP", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [](){ onSignupButtonClick(); });
                

    // ===== LOGIN_SIGNUP PAGE ELEMENTS =====
    sf::Text loginSignupPageTitle{font};
    string titleString = "WELCOME TO YOUR PERSONAL \nFINANCE MANAGER!\n\n\n\n\tPlease Login or Sign Up to Continue";
    loginSignupPageTitle.setString(titleString);
    loginSignupPageTitle.setCharacterSize(30);
    loginSignupPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect LoginSignupPageTitleBounds = loginSignupPageTitle.getLocalBounds();
    loginSignupPageTitle.setOrigin({LoginSignupPageTitleBounds.position.x + LoginSignupPageTitleBounds.size.x/2.0f, 
                        LoginSignupPageTitleBounds.position.y + LoginSignupPageTitleBounds.size.y/2.0f});
    loginSignupPageTitle.setPosition({450.0f, 250.0f});

    sf::Text UserLoggedOutText{font};
    UserLoggedOutText.setFillColor(sf::Color::Red);
    UserLoggedOutText.setCharacterSize(30);
    
    
    // ===== LOGIN PAGE ELEMENTS =====
    sf::Text LoginPageTitle{font};
    LoginPageTitle.setString("LOGIN PAGE");
    LoginPageTitle.setCharacterSize(30);
    LoginPageTitle.setFillColor(sf::Color::White);

    sf::FloatRect LoginPageTitleBounds = LoginPageTitle.getLocalBounds();
    LoginPageTitle.setOrigin({LoginPageTitleBounds.position.x + LoginPageTitleBounds.size.x/2.0f, 
                         LoginPageTitleBounds.position.y + LoginPageTitleBounds.size.y/2.0f});
    LoginPageTitle.setPosition({450.0f, 100.0f});

    TextField usernameFieldLogin("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 220, 300, 40, "Enter Username", false);
    TextField passwordFieldLogin("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 320, 300, 40, "Enter Password", true);

    Button GoToDashboardviaLoginButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       200, 400, 200, 50, "ENTER", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onDashboardViaLoginButtonClick(); });

    // ===== ERROR PAGE ELEMENTS =====
    sf::Text errorTitle{font};
    errorTitle.setString("Login Failed");
    errorTitle.setCharacterSize(36);
    errorTitle.setFillColor(sf::Color::Red);
    sf::FloatRect errorTitleBounds = errorTitle.getLocalBounds();
    errorTitle.setOrigin({errorTitleBounds.position.x + errorTitleBounds.size.x/2.0f, 
                         errorTitleBounds.position.y + errorTitleBounds.size.y/2.0f});
    errorTitle.setPosition({450.0f, 250.0f});

    
    
    sf::Text errorText{font};
    errorText.setCharacterSize(24);
    errorText.setFillColor(sf::Color::White);
    errorText.setPosition({250.0f, 320.0f});
    
    Button errorBackButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                        300, 500, 400, 50, "Try Again", 
                        sf::Color::Blue, sf::Color(100, 100, 255), 
                        sf::Color::Cyan, [](){if(errorFlag == 'l'){
                            currentPage = PageState::LOGIN_PAGE;
                        } else if (errorFlag == 's'){
                            currentPage = PageState::SIGNUP_PAGE;
                        } 
                        else if (errorFlag == 'p'){
                            currentPage = PageState::UPDATE_PASSWORD;
                        }
                        });

    // ===== SIGNUP PAGE ELEMENTS =====
    sf::Text SignupPageTitle{font};
    SignupPageTitle.setString("SIGNUP PAGE");
    SignupPageTitle.setCharacterSize(30);
    SignupPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect SignUpPageTitleBounds = SignupPageTitle.getLocalBounds();
    SignupPageTitle.setOrigin({SignUpPageTitleBounds.position.x + SignUpPageTitleBounds.size.x/2.0f, 
                         SignUpPageTitleBounds.position.y + SignUpPageTitleBounds.size.y/2.0f});
    SignupPageTitle.setPosition({450.0f, 100.0f});

    TextField usernameFieldSignup("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 180, 300, 40, "Enter Username", false);
    TextField passwordFieldSignup("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 280, 300, 40, "Enter Password", true);
    TextField emailFieldSignup("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 380, 300, 40, "Enter Email", false);
        
    Button GoToDashboardviaSignupButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       300, 500, 400, 50, "ENTER", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 

                            onDashboardViaSignupButtonClick(); });

    if (errorFlag == 'S'){
        currentUserName = usernameFieldSignup.getValue();
    }else if (errorFlag == 'L'){
        currentUserName = usernameFieldLogin.getValue();
    }


    // ===== DASHBOARD PAGE ELEMENTS =====

    // Dashboard Title
    sf::Text DashBoardPageTitle{font};
    DashBoardPageTitle.setCharacterSize(30);
    DashBoardPageTitle.setFillColor(sf::Color::White);
    

    // Dashboard : action selection prompt
    sf::Text dashboardActionPrompt{font};
    dashboardActionPrompt.setString("Select an action to proceed:");
    dashboardActionPrompt.setCharacterSize(24);
    dashboardActionPrompt.setFillColor(sf::Color::White);
    sf::FloatRect dashboardActionPromptBounds = dashboardActionPrompt.getLocalBounds();
    dashboardActionPrompt.setOrigin({dashboardActionPromptBounds.position.x + dashboardActionPromptBounds.size.x/2.0f, 
                         dashboardActionPromptBounds.position.y + dashboardActionPromptBounds.size.y/2.0f});
    dashboardActionPrompt.setPosition({450.0f, 200.0f});


    // Dashboard Buttons
    // // Go to Account Details Page Button
    Button GoToAccountDetailsPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 250, 400, 50, "VIEW ACCOUNT DETAILS", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickViewAccountDetails();});
    // Go to Update Account Credentials Page Button
    Button GoToUpdateAccountCredentialsPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 325, 400, 50, "UPDATE ACCOUNT CREDENTIALS", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickUpdateAccountCredentialsPage();});
    // Go to Manage Transactions Page Button
    Button GoToManageTransactionsPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 400, 400, 50, "MANAGE TRANSACTIONS", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickManageTransactions();});
    // Go to Logout Confirmation Page Button
    Button GoToLogoutConfirmationPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 475, 400, 50, "LOGOUT", 
                       sf::Color::Red, sf::Color::Yellow, sf::Color::Red,
                        [&](){ 
                            onClickConfirmLogout();});


    // ===== UPDATE ACCOUNT CREDENTIALS PAGE ELEMENTS =====
    sf::Text UpdateAccountCredentialsPageTitle{font};
    UpdateAccountCredentialsPageTitle.setString("UPDATE ACCOUNT CREDENTIALS");
    UpdateAccountCredentialsPageTitle.setCharacterSize(30);
    UpdateAccountCredentialsPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect UpdateAccountCredentialsPageTitleBounds = UpdateAccountCredentialsPageTitle.getLocalBounds();
    UpdateAccountCredentialsPageTitle.setOrigin({UpdateAccountCredentialsPageTitleBounds.position.x + UpdateAccountCredentialsPageTitleBounds.size.x/2.0f, 
                         UpdateAccountCredentialsPageTitleBounds.position.y + UpdateAccountCredentialsPageTitleBounds.size.y/2.0f});
    UpdateAccountCredentialsPageTitle.setPosition({450.0f, 100.0f});


    // ===== BUTTONS =====
    // Go to "Update Password" Page Button
    Button GoToUpdatePasswordPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 250, 400, 50, "UPDATE PASSWORD", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickUpdatePasswordPage();});
    // Go to "Update Email" Page Button
    Button GoToUpdateEmailPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 325, 400, 50, "UPDATE EMAIL", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickUpdateEmailPage();});
    // Go Back to Dashboard Page Button
    Button GoBackToDashboardFromUpdateAccountInfoPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 400, 400, 50, "Back to Dashboard", 
                       sf::Color::Red, sf::Color::Yellow, 
                       sf::Color::Red, [](){ 
                            currentPage = PageState::DASHBOARD_PAGE;});

    // ===== UPDATE PASSWORD PAGE ELEMENTS =====

    // Update Password Page Title
    sf::Text UpdatePasswordPageTitle{font};
    UpdatePasswordPageTitle.setString("UPDATE PASSWORD");
    UpdatePasswordPageTitle.setCharacterSize(30);
    UpdatePasswordPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect UpdatePasswordPageTitleBounds = UpdatePasswordPageTitle.getLocalBounds();
    UpdatePasswordPageTitle.setOrigin({UpdatePasswordPageTitleBounds.position.x + UpdatePasswordPageTitleBounds.size.x/2.0f, 
                         UpdatePasswordPageTitleBounds.position.y + UpdatePasswordPageTitleBounds.size.y/2.0f});
    UpdatePasswordPageTitle.setPosition({450.0f, 150.0f});

    // PASSWORD UPDATED POPUP
    sf::Text PasswordUpdatedPopup{font};
    PasswordUpdatedPopup.setCharacterSize(24);
    PasswordUpdatedPopup.setFillColor(sf::Color::Green);
    sf::FloatRect PasswordUpdatedPopupBounds = PasswordUpdatedPopup.getLocalBounds();
    PasswordUpdatedPopup.setOrigin({PasswordUpdatedPopupBounds.position.x + PasswordUpdatedPopupBounds.size.x/2.0f, 
                         PasswordUpdatedPopupBounds.position.y + PasswordUpdatedPopupBounds.size.y/2.0f});
    PasswordUpdatedPopup.setPosition({450.0f, 70.0f});

    // PASSWORD INPUT FIELD
    TextField NewPasswordInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 100, 300, 40, "Enter New Password", true);
    // CONFIRM PASSWORD INPUT FIELD
    TextField ConfirmNewPasswordInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 200, 300, 40, "Confirm New Password", true);
    // UPDATE PASSWORD BUTTON
    Button UpdatePassword("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 300, 400, 50, "UPDATE PASSWORD", 
                       sf::Color::Blue, sf::Color::Cyan, 
                       sf::Color::Blue, [](){
                            onClickUpdatePassword();});

    // ===== UPDATE EMAIL PAGE ELEMENTS =====

    // Update Email Page Title
    sf::Text UpdateEmailPageTitle{font};
    UpdateEmailPageTitle.setString("UPDATE EMAIL");
    UpdateEmailPageTitle.setCharacterSize(30);
    UpdateEmailPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect UpdateEmailPageTitleBounds = UpdateEmailPageTitle.getLocalBounds();
    UpdateEmailPageTitle.setOrigin({UpdateEmailPageTitleBounds.position.x + UpdateEmailPageTitleBounds.size.x/2.0f, 
                         UpdateEmailPageTitleBounds.position.y + UpdateEmailPageTitleBounds.size.y/2.0f});
    UpdateEmailPageTitle.setPosition({450.0f, 150.0f});

    // PASSWORD UPDATED POPUP
    sf::Text EmailUpdatedPopup{font};
    EmailUpdatedPopup.setCharacterSize(24);
    EmailUpdatedPopup.setFillColor(sf::Color::Green);
    sf::FloatRect EmailUpdatedPopupBounds = EmailUpdatedPopup.getLocalBounds();
    EmailUpdatedPopup.setOrigin({EmailUpdatedPopupBounds.position.x + EmailUpdatedPopupBounds.size.x/2.0f, 
                         EmailUpdatedPopupBounds.position.y + EmailUpdatedPopupBounds.size.y/2.0f});
    EmailUpdatedPopup.setPosition({450.0f, 70.0f});

    // EMAIL INPUT FIELD
    TextField NewEmailInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 200, 300, 40, "Enter New Email", true);
    // UPDATE EMAIL BUTTON
    Button UpdateEmail("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 300, 400, 50, "UPDATE EMAIL", 
                       sf::Color::Blue, sf::Color::Cyan, 
                       sf::Color::Blue, [](){
                            onClickUpdateEmail();});

    
    // ===== MANAGE TRANSACTIONS PAGE ELEMENTS =====
    sf::Text ManageTransactionsPageTitle{font};
    ManageTransactionsPageTitle.setString("MANAGE YOUR TRANSACTIONS");
    ManageTransactionsPageTitle.setCharacterSize(30);
    ManageTransactionsPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect ManageTransactionsPageTitleBounds = ManageTransactionsPageTitle.getLocalBounds();
    ManageTransactionsPageTitle.setOrigin({ManageTransactionsPageTitleBounds.position.x + ManageTransactionsPageTitleBounds.size.x/2.0f, 
                         ManageTransactionsPageTitleBounds.position.y + ManageTransactionsPageTitleBounds.size.y/2.0f});
    ManageTransactionsPageTitle.setPosition({450.0f, 100.0f});

    // ===== BUTTONS =====
    // Go to "Add Transaction" Page Button
    Button GoToAddTransactionPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 300, 400, 50, "ADD TRANSACTION", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickAddTransactionPage();});
    // Go to "Remove Transaction" Page Button
    Button GoToRemoveTransactionPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 400, 400, 50, "REMOVE TRANSACTION", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickRemoveTransactionPage();});
    // Go Back to Dashboard Page Button
    Button GoBackToDashboardFromManageTransactionsPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 500, 400, 50, "Back to Dashboard", 
                       sf::Color::Red, sf::Color::Yellow, 
                       sf::Color::Red, [](){ 
                            // Clear remove transaction buttons
                            for (auto* btn : RemovableTransactionButtons) {
                                delete btn;
                            }
                            RemovableTransactionButtons.clear();
                            removeTransactionButtonsCreated = false;  // Reset flag
                            currentPage = PageState::DASHBOARD_PAGE;});

    // ===== ADD TRANSACTION PAGE ELEMENTS =====
    sf::Text AddTransactionPageTitle{font};
    AddTransactionPageTitle.setString("ADD NEW TRANSACTION");
    AddTransactionPageTitle.setCharacterSize(30);
    AddTransactionPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect AddTransactionPageTitleBounds = AddTransactionPageTitle.getLocalBounds();
    AddTransactionPageTitle.setOrigin({AddTransactionPageTitleBounds.position.x + AddTransactionPageTitleBounds.size.x/2.0f, 
                         AddTransactionPageTitleBounds.position.y + AddTransactionPageTitleBounds.size.y/2.0f});
    AddTransactionPageTitle.setPosition({450.0f, 100.0f});


    // ===== INPUT FIELDS =====
    // TRANSACTION TYPE INPUT FIELD
    TextField TransactionTypeInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 180, 300, 40, "Enter Transaction Type (Income/Expense)", false);
    // TRANSACTION AMOUNT INPUT FIELD
    TextField TransactionAmountInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 280, 300, 40, "Enter Transaction Amount", false);
    // TRANSACTION DESCRIPTION INPUT FIELD
    TextField TransactionDescriptionInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 380, 300, 40, "Enter Transaction Description", false);
    // ADD TRANSACTION BUTTON
    Button AddNewTransactionButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 440, 400, 50, "ADD TRANSACTION", 
                       sf::Color::Blue, sf::Color::Cyan, 
                       sf::Color::Blue, [](){
                            onClickAddNewTransaction();});
                
    // ===== REMOVE TRANSACTION PAGE ELEMENTS =====
    sf::Text RemoveTransactionPageTitle{font};
    RemoveTransactionPageTitle.setString("REMOVE TRANSACTION");
    RemoveTransactionPageTitle.setCharacterSize(30);
    RemoveTransactionPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect RemoveTransactionPageTitleBounds = RemoveTransactionPageTitle.getLocalBounds();
    RemoveTransactionPageTitle.setOrigin({RemoveTransactionPageTitleBounds.position.x + RemoveTransactionPageTitleBounds.size.x/2.0f, 
                         RemoveTransactionPageTitleBounds.position.y + RemoveTransactionPageTitleBounds.size.y/2.0f});
    RemoveTransactionPageTitle.setPosition({450.0f, 100.0f});

    // Get current month
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string currentDate = ctime(&now);
    
    
    // ===== VIEW ACCOUT DETAILS PAGE ELEMENTS =====
    sf::Text AccountDetailsPageTitle{font};
    AccountDetailsPageTitle.setString("WHAT WOULD YOU LIKE TO VIEW?");
    AccountDetailsPageTitle.setCharacterSize(30);
    AccountDetailsPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect AccountDetailsPageTitleBounds = AccountDetailsPageTitle.getLocalBounds();
    AccountDetailsPageTitle.setOrigin({AccountDetailsPageTitleBounds.position.x + AccountDetailsPageTitleBounds.size.x/2.0f, 
                         AccountDetailsPageTitleBounds.position.y + AccountDetailsPageTitleBounds.size.y/2.0f});
    AccountDetailsPageTitle.setPosition({450.0f, 100.0f});

    // CURRENT BALANCE DISPLAY
    sf::Text CurrentUserBalance{font};
    CurrentUserBalance.setCharacterSize(24);
    CurrentUserBalance.setFillColor(sf::Color::White);

    // CURRENT MONTHLY BUDGET DISPLAY
    sf::Text CurrentUserMonthlyBugdet{font};
    CurrentUserMonthlyBugdet.setCharacterSize(24);
    CurrentUserMonthlyBugdet.setFillColor(sf::Color::White);

    // HAVE REACH SAVINGS GOAL DISPLAY
    sf::Text HaveReachedSavingsGoal{font};
    HaveReachedSavingsGoal.setCharacterSize(24);
    HaveReachedSavingsGoal.setFillColor(sf::Color::Black);

    // ===== BUTTONS =====
    // Go to "Set New Budget" Page Button
    Button GoToSetNewBugdetPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       140, 400, 300, 50, "Set New Monthly Budget", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickSetNewBudget();});
    // Go to "Add to Balance" Page Button
    Button GoToAddToBalancePage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       140, 475, 300, 50, "Add to Balance", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickAddToBalance();});
    // Go to "Update Savings Amount" Page Button
    Button GoToUpdateSavingsAmountSetPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       140, 550, 300, 50, "Update Savings Amount", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickSetSavingsAccountAmountPage();});
    // Go to "Update Savings Goal" Page Button
    Button GoToUpdateSavingsGoalPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       480, 400, 300, 50, "Update Savings Goal", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickSetSavingGoalPage();});
    // Go to "Export Monthly Report" Page Button
    Button GoToExportMonthlyReportPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       480, 475, 300, 50, "Export Monthly Report", 
                       sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                        [&](){ 
                            onClickGoToExportMonthlyReportPage();});
    // Go Back to Dashboard Page Button
    Button GoBackToDashboardFromAccountDetailsPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       480, 550, 300, 50, "Back to Dashboard", 
                       sf::Color::Red, sf::Color::Yellow, 
                       sf::Color::Red, [](){ 
                            currentPage = PageState::DASHBOARD_PAGE;});


    // ===== SET NEW BUDGET PAGE ELEMENTS =====
    sf::Text SetNewBudgetPageTitle{font};
    SetNewBudgetPageTitle.setString("SET NEW MONTHLY BUDGET PAGE");
    SetNewBudgetPageTitle.setCharacterSize(30);
    SetNewBudgetPageTitle.setFillColor(sf::Color::White);
    sf::FloatRect SetNewBudgetPageTitleBounds = SetNewBudgetPageTitle.getLocalBounds();
    SetNewBudgetPageTitle.setOrigin({SetNewBudgetPageTitleBounds.position.x + SetNewBudgetPageTitleBounds.size.x/2.0f, 
                                  SetNewBudgetPageTitleBounds.position.y + SetNewBudgetPageTitleBounds.size.y/2.0f});
    SetNewBudgetPageTitle.setPosition({450.0f, 100.0f});

    // SET NEW BUDGET INPUT FIELD
    TextField NewBudgetInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 180, 300, 40, "New Monthly Budget", false);
    
    // Update Balance Button
    Button UpdateBudget("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 300, 400, 50, "SET BUDGET", 
                       sf::Color::Blue, sf::Color::Cyan, 
                       sf::Color::Blue, [](){
                            onClickUpdateBudget();});
    
    // ===== ADD TO BALANCE PAGE ELEMENTS =====
    sf::Text AddToBalancePageTitle{font};
    AddToBalancePageTitle.setString("ADD TO BALANCE");
    AddToBalancePageTitle.setCharacterSize(30);
    AddToBalancePageTitle.setFillColor(sf::Color::White);
    sf::FloatRect AddToBalancePageTitleBounds = AddToBalancePageTitle.getLocalBounds();
    AddToBalancePageTitle.setOrigin({AddToBalancePageTitleBounds.position.x + AddToBalancePageTitleBounds.size.x/2.0f, 
                                  AddToBalancePageTitleBounds.position.y + AddToBalancePageTitleBounds.size.y/2.0f});
    AddToBalancePageTitle.setPosition({450.0f, 100.0f});

    // SET NEW BUDGET INPUT FIELD
    TextField AddToBalanceInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 180, 300, 40, "Add to Balance", false);
    
    // Update Balance Button
    Button UpdateBalance("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 300, 400, 50, "ADD TO BALANCE", 
                       sf::Color::Blue, sf::Color::Cyan, 
                       sf::Color::Blue, [](){
                            onClickUpdateBalance();});

    Button GoBackToViewAccountDetailsPage("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 400, 400, 50, "Back to Account Details", 
                       sf::Color::Red, sf::Color::Yellow, 
                       sf::Color::Red, [](){ 
                            currentPage = PageState::VIEW_ACCOUNT_DETAILS_PAGE;});
                
    // ===== UPDATE SAVING ACCOUNT AMOUNT PAGE ELEMENTS =====
    sf::Text UpdateSavingsAmountTitle{font};
    UpdateSavingsAmountTitle.setString("UPDATE SAVINGS AMOUNT");
    UpdateSavingsAmountTitle.setCharacterSize(30);
    UpdateSavingsAmountTitle.setFillColor(sf::Color::White);
    sf::FloatRect UpdateSavingsAmountTitleBounds = UpdateSavingsAmountTitle.getLocalBounds();
    UpdateSavingsAmountTitle.setOrigin({UpdateSavingsAmountTitleBounds.position.x + UpdateSavingsAmountTitleBounds.size.x/2.0f, 
                                  UpdateSavingsAmountTitleBounds.position.y + UpdateSavingsAmountTitleBounds.size.y/2.0f});
    UpdateSavingsAmountTitle.setPosition({450.0f, 100.0f});

    // ADD TO SAVINGS ACCOUNT INPUT FIELD
    TextField UpdateSavingsAmountInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 180, 300, 40, "ADD TO SAVINGS AMOUNT", false);
    
    // Update Balance Button
    Button UpdateSavingsAmount("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 300, 400, 50, "ADD TO SAVINGS AMOUNT", 
                       sf::Color::Blue, sf::Color::Cyan, 
                       sf::Color::Blue, [](){
                            onClickUpdateSavingsAccountAmount();});


    // ===== UPDATE SAVING GOALS PAGE ELEMENTS =====
    sf::Text UpdateSavingsGoalTitle{font};
    UpdateSavingsGoalTitle.setString("UPDATE SAVINGS GOAL");
    UpdateSavingsGoalTitle.setCharacterSize(30);
    UpdateSavingsGoalTitle.setFillColor(sf::Color::White);
    sf::FloatRect UpdateSavingsGoalTitleBounds = UpdateSavingsGoalTitle.getLocalBounds();
    UpdateSavingsGoalTitle.setOrigin({UpdateSavingsGoalTitleBounds.position.x + UpdateSavingsGoalTitleBounds.size.x/2.0f, 
                                  UpdateSavingsGoalTitleBounds.position.y + UpdateSavingsGoalTitleBounds.size.y/2.0f});
    UpdateSavingsGoalTitle.setPosition({450.0f, 100.0f});

    // UPDATE TO SAVINGS GOAL INPUT FIELD
    TextField UpdateSavingsGoalInputField("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 300, 180, 300, 40, "UPDATE SAVINGS GOAL", false);
    
    // Update Balance Button
    Button UpdateSavingsGoal("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       260, 300, 400, 50, "UPDATE SAVINGS GOAL", 
                       sf::Color::Blue, sf::Color::Cyan, 
                       sf::Color::Blue, [](){
                            onClickUpdateSavingsGoal();});



    // ===== EXPORT MONTHLY REPORT PAGE ELEMENTS =====

    int month = 1 + ltm->tm_mon; // tm_mon is 0-11
    map<string, int> monthMap = {{"Jan",1}, {"Feb",2}, {"Mar",3}, {"Apr",4}, {"May",5}, {"Jun",6},{"Jul",7}, {"Aug",8}, {"Sep",9}, {"Oct",10}, {"Nov",11}, {"Dec",12}};
    string currentMonthStr;
    vector<Button*> ExportMonthlyReportButtons;
    vector<string> availableMonthNames;

    for (const auto& pair : monthMap) {
        if (pair.second == month) {
            currentMonthStr = pair.first;
            break;
        }
    }

    map<string,int> availableMonths;
    for (const auto& pair : monthMap) {
        if (pair.second <= month) {
            availableMonths[pair.first] = pair.second;
            availableMonthNames.push_back(pair.first);
        }
    }

    sf::Text ExportMonthlyReportTitle{font};
    ExportMonthlyReportTitle.setString("EXPORT MONTHLY REPORT");
    ExportMonthlyReportTitle.setCharacterSize(30);
    ExportMonthlyReportTitle.setFillColor(sf::Color::White);
    sf::FloatRect ExportMonthlyReportTitleBounds = ExportMonthlyReportTitle.getLocalBounds();
    ExportMonthlyReportTitle.setOrigin({ExportMonthlyReportTitleBounds.position.x + ExportMonthlyReportTitleBounds.size.x/2.0f, 
                                  ExportMonthlyReportTitleBounds.position.y + ExportMonthlyReportTitleBounds.size.y/2.0f});
    ExportMonthlyReportTitle.setPosition({450.0f, 100.0f});

    // Create buttons for each available month
    // Arrage buttons in grid, with 3 buttons accross
    
    // Go to "Export Monthly Report" Page Button
    for (auto &monthName : availableMonthNames){
        int index = distance(availableMonthNames.begin(), find(availableMonthNames.begin(), availableMonthNames.end(), monthName));
        int x = 200 + (index % 3) * 220; // 3 buttons per row
        int y = 200 + (index / 3) * 80;  // New row every 3 buttons

        Button* monthButton = new Button("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                           x, y, 200, 50, monthName, 
                           sf::Color::Green, sf::Color::Cyan, sf::Color::Green,
                            [monthName](){ 
                                onClickExportMonthlyReport(monthName);});
                                ExportMonthlyReportButtons.push_back(monthButton);
    }

    // ===== LOGOUT CONFIRMATION PAGE ELEMENTS =====
    sf::Text LogoutConfirmationTitle{font};
    LogoutConfirmationTitle.setString("ARE YOU SURE YOU WANT TO LOGOUT?");
    LogoutConfirmationTitle.setCharacterSize(30);
    LogoutConfirmationTitle.setFillColor(sf::Color::White);
    sf::FloatRect LogoutConfirmationTitleBounds = LogoutConfirmationTitle.getLocalBounds();
    LogoutConfirmationTitle.setOrigin({LogoutConfirmationTitleBounds.position.x + LogoutConfirmationTitleBounds.size.x/2.0f, 
                                  LogoutConfirmationTitleBounds.position.y + LogoutConfirmationTitleBounds.size.y/2.0f});
    LogoutConfirmationTitle.setPosition({450.0f, 200.0f});

    // Confirm Logout Button
    Button ConfirmLogoutButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       250, 300, 150, 50, "YES", 
                       sf::Color::Red, sf::Color::Yellow, 
                       sf::Color::Red, [](){
                            onClicklogout();});
    // Cancel Logout Button
    Button CancelLogoutButton("D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                       500, 300, 150, 50, "NO", 
                       sf::Color::Green, sf::Color::Cyan, 
                       sf::Color::Green, [](){
                            onClickCancelLogout();});
    


    // ===== MAIN LOOP =====

    while (window.isOpen()){
        while (const optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                cout << "Closing window..." << endl;
                window.close();
            }
            // Handle events based on current page
            if (currentPage == PageState::LOGIN_SIGNUP) {
                loginButton.handleEvent(*event);
                signUpButton.handleEvent(*event);
            } else if (currentPage == PageState::LOGIN_PAGE) {
                usernameFieldLogin.handleEvent(*event, window);
                passwordFieldLogin.handleEvent(*event, window);
                GoToDashboardviaLoginButton.handleEvent(*event);
            } else if (currentPage == PageState::SIGNUP_PAGE) {
                usernameFieldSignup.handleEvent(*event, window);
                passwordFieldSignup.handleEvent(*event, window);
                emailFieldSignup.handleEvent(*event, window);
                GoToDashboardviaSignupButton.handleEvent(*event);
            }else if (currentPage == PageState::ERROR) {
                errorBackButton.handleEvent(*event);
            }else if (currentPage == PageState::DASHBOARD_PAGE) {
                GoToAccountDetailsPage.handleEvent(*event);
                GoToUpdateAccountCredentialsPage.handleEvent(*event);
                GoToManageTransactionsPage.handleEvent(*event);
                GoToLogoutConfirmationPage.handleEvent(*event);
            }else if (currentPage == PageState::VIEW_ACCOUNT_DETAILS_PAGE){
                GoBackToDashboardFromAccountDetailsPage.handleEvent(*event);
                GoToSetNewBugdetPage.handleEvent(*event);
                GoToAddToBalancePage.handleEvent(*event);
                GoToUpdateSavingsAmountSetPage.handleEvent(*event);
                GoToUpdateSavingsGoalPage.handleEvent(*event);
                GoToExportMonthlyReportPage.handleEvent(*event);
            }else if (currentPage == PageState::UPDATE_ACCOUNT_CREDENTIALS_PAGE){
                GoToUpdatePasswordPage.handleEvent(*event);
                GoToUpdateEmailPage.handleEvent(*event);
                GoBackToDashboardFromAccountDetailsPage.handleEvent(*event);
            }else if (currentPage == PageState::SET_NEW_BUDGET){
                NewBudgetInputField.handleEvent(*event, window);
                UpdateBudget.handleEvent(*event);
                GoBackToViewAccountDetailsPage.handleEvent(*event);
            }else if (currentPage == PageState::ADD_TO_BALANCE){
                AddToBalanceInputField.handleEvent(*event, window);
                UpdateBalance.handleEvent(*event);
                GoBackToViewAccountDetailsPage.handleEvent(*event);
            }else if (currentPage == PageState::UPDATE_SAVINGS_ACCOUNT_AMOUNT){
                UpdateSavingsAmountInputField.handleEvent(*event, window);
                UpdateSavingsAmount.handleEvent(*event);
                GoBackToViewAccountDetailsPage.handleEvent(*event);
            }else if (currentPage == PageState::EXPORT_MONTHLY_REPORT){
                for (auto& button : ExportMonthlyReportButtons) {
                    button->handleEvent(*event);
                }
            }else if (currentPage == PageState::UPDATE_SAVINGS_GOAL){
                UpdateSavingsGoalInputField.handleEvent(*event, window);
                UpdateSavingsGoal.handleEvent(*event);
                GoBackToViewAccountDetailsPage.handleEvent(*event);
            }else if (currentPage == PageState::LOGOUT_CONFIRMATION_PAGE){
                ConfirmLogoutButton.handleEvent(*event);
                CancelLogoutButton.handleEvent(*event);
            }else if (currentPage == PageState::UPDATE_PASSWORD){
                NewPasswordInputField.handleEvent(*event, window);
                ConfirmNewPasswordInputField.handleEvent(*event, window);
                UpdatePassword.handleEvent(*event);
            }else if (currentPage == PageState::UPDATE_EMAIL){
                NewEmailInputField.handleEvent(*event, window);
                UpdateEmail.handleEvent(*event);
            }else if (currentPage == PageState::MANAGE_TRANSACTIONS){
                GoToAddTransactionPage.handleEvent(*event);
                GoToRemoveTransactionPage.handleEvent(*event);
                GoBackToDashboardFromManageTransactionsPage.handleEvent(*event);
            }else if (currentPage == PageState::ADD_TRANSACTION){
                TransactionTypeInputField.handleEvent(*event, window);
                TransactionAmountInputField.handleEvent(*event, window);
                TransactionDescriptionInputField.handleEvent(*event, window);
                AddNewTransactionButton.handleEvent(*event);
            }else if (currentPage == PageState::REMOVE_TRANSACTION){
                for (auto& button : RemovableTransactionButtons) {
                    button->handleEvent(*event);
                }
                GoBackToDashboardFromManageTransactionsPage.handleEvent(*event);
            }
        }

        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos(static_cast<float>(mousePixelPos.x), 
                             static_cast<float>(mousePixelPos.y));

        if (currentPage == PageState::LOGIN_SIGNUP){
            // Display User logged out message if applicable
            string UserLoggedOutString = "";
            if (currentUser.LoggedIn == 0){
                UserLoggedOutString = currentUserName + " Has been logged out successfully!";
            }
            UserLoggedOutText.setString(UserLoggedOutString);
            sf::FloatRect UserLoggedOutTextBounds = UserLoggedOutText.getLocalBounds();
            UserLoggedOutText.setOrigin({UserLoggedOutTextBounds.position.x + UserLoggedOutTextBounds.size.x/2.0f, 
            UserLoggedOutTextBounds.position.y + UserLoggedOutTextBounds.size.y/2.0f});
            UserLoggedOutText.setPosition({450.0f, 100.0f});
            loginButton.update(mousePos);
            signUpButton.update(mousePos);
        } else if (currentPage == PageState::LOGIN_PAGE) {
            currentUserName = usernameFieldLogin.getValue();
            currentUserPassword = passwordFieldLogin.getValue();
            usernameFieldLogin.update();
            passwordFieldLogin.update();
            GoToDashboardviaLoginButton.update(mousePos);
        } else if (currentPage == PageState::SIGNUP_PAGE) {
            currentUserName = usernameFieldSignup.getValue();
            currentUserPassword = passwordFieldSignup.getValue();
            currentUserEmail = emailFieldSignup.getValue();
            usernameFieldSignup.update();
            passwordFieldSignup.update();
            emailFieldSignup.update();
            GoToDashboardviaSignupButton.update(mousePos);
        } else if (currentPage == PageState::ERROR) {
            errorBackButton.update(mousePos);
        } else if (currentPage == PageState::DASHBOARD_PAGE) {
            string DashBoardTitleString = "WELCOME TO YOUR DASHBOARD!" + ("\n\nLogged in as: " + currentUser.username);
            DashBoardPageTitle.setString(DashBoardTitleString);
            sf::FloatRect DashBoardPageTitleBounds = DashBoardPageTitle.getLocalBounds();
            DashBoardPageTitle.setOrigin({DashBoardPageTitleBounds.position.x + DashBoardPageTitleBounds.size.x/2.0f, 
            DashBoardPageTitleBounds.position.y + DashBoardPageTitleBounds.size.y/2.0f});
            DashBoardPageTitle.setPosition({450.0f, 100.0f});
            GoToAccountDetailsPage.update(mousePos);
            GoToUpdateAccountCredentialsPage.update(mousePos); 
            GoToManageTransactionsPage.update(mousePos);
            GoToLogoutConfirmationPage.update(mousePos);
        }else if (currentPage == PageState::VIEW_ACCOUNT_DETAILS_PAGE){
            // Display Current Balance
            string CurrentUserBalanceString = "Current Balance: $" + to_string(trunc(currentUser.balance));
            CurrentUserBalance.setString(CurrentUserBalanceString);
            sf::FloatRect CurrentUserBalanceStringBounds = CurrentUserBalance.getLocalBounds();
            CurrentUserBalance.setOrigin({CurrentUserBalanceStringBounds.position.x + CurrentUserBalanceStringBounds.size.x/2.0f, 
            CurrentUserBalanceStringBounds.position.y + CurrentUserBalanceStringBounds.size.y/2.0f});
            CurrentUserBalance.setPosition({180.0f,150.0f});
            // Display Current Monthly Budget
            string CurrentUserMonthlyBudgetString = "Current Monthly Budget: $" + to_string(trunc(currentUser.MonthlyBudget));
            CurrentUserMonthlyBugdet.setString(CurrentUserMonthlyBudgetString);
            sf::FloatRect CurrentUserMonthlyBugdetBounds = CurrentUserMonthlyBugdet.getLocalBounds();
            CurrentUserMonthlyBugdet.setOrigin({CurrentUserMonthlyBugdetBounds.position.x + CurrentUserMonthlyBugdetBounds.size.x/2.0f, 
            CurrentUserMonthlyBugdetBounds.position.y + CurrentUserMonthlyBugdetBounds.size.y/2.0f});
            CurrentUserMonthlyBugdet.setPosition({200.0f, 200.0f});
            // Display Have Reached Savings Goal
            string HaveReachedSavingsGoalString = "";
            if (currentUser.SavingsGoal > 0){
                if (currentUser.SavingsAmount >= currentUser.SavingsGoal){
                    HaveReachedSavingsGoalString = "Congratulations! You have reached your savings goal of $" + to_string(trunc(currentUser.SavingsGoal)) + ".";
                }
            }
            HaveReachedSavingsGoal.setString(HaveReachedSavingsGoalString);
            HaveReachedSavingsGoal.setFillColor(sf::Color::Green);
            sf::FloatRect HaveReachedSavingsGoalBounds = HaveReachedSavingsGoal.getLocalBounds();
            HaveReachedSavingsGoal.setOrigin({HaveReachedSavingsGoalBounds.position.x + HaveReachedSavingsGoalBounds.size.x/2.0f, 
            HaveReachedSavingsGoalBounds.position.y + HaveReachedSavingsGoalBounds.size.y/2.0f});
            HaveReachedSavingsGoal.setPosition({250.0f, 250.0f});
            // Update Mouse position for buttons
            GoBackToDashboardFromAccountDetailsPage.update(mousePos);
            GoToSetNewBugdetPage.update(mousePos);
            GoToAddToBalancePage.update(mousePos);
            GoToUpdateSavingsAmountSetPage.update(mousePos);
            GoToUpdateSavingsGoalPage.update(mousePos);
            GoToExportMonthlyReportPage.update(mousePos);
        }else if (currentPage == PageState::UPDATE_ACCOUNT_CREDENTIALS_PAGE){
            if (errorFlag == 'E' ){
                EmailUpdatedPopup.setString("EMAIL UPDATED SUCCESSFULLY!");
            }else if  (errorFlag == 'P' ){
                PasswordUpdatedPopup.setString("PASSWORD UPDATED SUCCESSFULLY!");
            }else {
                EmailUpdatedPopup.setString("");
                PasswordUpdatedPopup.setString("");
                errorFlag = '\0';
            }
            GoToUpdatePasswordPage.update(mousePos);
            GoToUpdateEmailPage.update(mousePos);
            GoBackToDashboardFromAccountDetailsPage.update(mousePos);
        }else if (currentPage == PageState::SET_NEW_BUDGET){
            currentUserMonthlyBudget = NewBudgetInputField.getValue();
            NewBudgetInputField.update();
            UpdateBudget.update(mousePos);
            GoBackToViewAccountDetailsPage.update(mousePos);
        }else if (currentPage == PageState::ADD_TO_BALANCE){
            currentUserBalance = AddToBalanceInputField.getValue();
            AddToBalanceInputField.update();
            UpdateBalance.update(mousePos);
            GoBackToViewAccountDetailsPage.update(mousePos);
        }else if (currentPage == PageState::UPDATE_SAVINGS_ACCOUNT_AMOUNT){
            currentUserSavingsAmount = UpdateSavingsAmountInputField.getValue();
            UpdateSavingsAmountInputField.update();
            UpdateSavingsAmount.update(mousePos);
        }else if (currentPage == PageState::EXPORT_MONTHLY_REPORT){
            for (auto& button : ExportMonthlyReportButtons) {
                button->update(mousePos);
            }
        }else if (currentPage == PageState::UPDATE_SAVINGS_GOAL){
            currentUserSavingsGoal = UpdateSavingsGoalInputField.getValue();
            UpdateSavingsGoalInputField.update();
            UpdateSavingsGoal.update(mousePos);
            GoBackToViewAccountDetailsPage.update(mousePos);
        }else if (currentPage == PageState::LOGOUT_CONFIRMATION_PAGE){
            ConfirmLogoutButton.update(mousePos);
            CancelLogoutButton.update(mousePos);
        }else if (currentPage == PageState::UPDATE_PASSWORD){
            currentUserNewPassword = NewPasswordInputField.getValue();
            currentUserConfirmNewPassword = ConfirmNewPasswordInputField.getValue();
            NewPasswordInputField.update();
            ConfirmNewPasswordInputField.update();
            UpdatePassword.update(mousePos);
        }else if (currentPage == PageState::UPDATE_EMAIL){
            currentUserNewEmail = NewEmailInputField.getValue();
            NewEmailInputField.update();
            UpdateEmail.update(mousePos);
        }else if (currentPage == PageState::MANAGE_TRANSACTIONS){
            GoToAddTransactionPage.update(mousePos);
            GoToRemoveTransactionPage.update(mousePos);
            GoBackToDashboardFromManageTransactionsPage.update(mousePos);
        }else if (currentPage == PageState::ADD_TRANSACTION){
            currentTransactionType = TransactionTypeInputField.getValue();
            currentTransactionAmount = TransactionAmountInputField.getValue();
            currentTransactionDescription = TransactionDescriptionInputField.getValue();
            TransactionTypeInputField.update();
            TransactionAmountInputField.update();
            TransactionDescriptionInputField.update();
            AddNewTransactionButton.update(mousePos);
        }else if (currentPage == PageState::REMOVE_TRANSACTION){
            // Only create buttons once when entering the page
            if (!removeTransactionButtonsCreated) {
                // Load current user's transactions
                currentUserTransactions.clear();
                LoadAllTransactions(TransactionsDataFilePath, currentUserTransactions);
                
                // Filter transactions for current user only
                vector<Transaction> userTransactions;
                for (const auto& trans : currentUserTransactions) {
                    if (trans.userID == currentUser.userID) {
                        userTransactions.push_back(trans);
                    }
                }
                
                // Show last 10 transactions
                size_t numToShow = min(static_cast<size_t>(10), userTransactions.size());
                size_t startIdx = userTransactions.size() > 10 ? userTransactions.size() - 10 : 0;
                
                // Clear old buttons
                for (auto* btn : RemovableTransactionButtons) {
                    delete btn;
                }
                RemovableTransactionButtons.clear();
                
                // Create new buttons for last 10 transactions
                for (size_t i = 0; i < numToShow; ++i) {
                    size_t transIdx = startIdx + i;
                    const Transaction& trans = userTransactions[transIdx];
                    
                    string buttonText = trans.description + " - $" + to_string(trans.amount);
                    float y = 180.0f + (i * 45.0f);
                    
                    Button* removeButton = new Button(
                        "D:\\LUMS\\Semester 1\\CS 100\\CS100_Semester_Project_Personal_Finance_Manager\\times.ttf", 
                        200, y, 500, 40, buttonText, 
                        sf::Color::Red, sf::Color(255, 100, 100), sf::Color::Yellow,
                        [trans]() { 
                            currentTransactionDescription = trans.transactionNo;
                            onClickRemoveTransaction();
                        });
                    RemovableTransactionButtons.push_back(removeButton);
                }
                
                removeTransactionButtonsCreated = true;  // Mark as created
            }
            
            // Update button hover states every frame
            for (auto* btn : RemovableTransactionButtons) {
                btn->update(mousePos);
            }
            GoBackToDashboardFromManageTransactionsPage.update(mousePos);
        }

        // Render based on current page
        window.clear(sf::Color::Black);
        if (currentPage == PageState::LOGIN_SIGNUP){
            window.draw(loginSignupPageTitle);
            window.draw(UserLoggedOutText);
            loginButton.render(window);
            signUpButton.render(window);
        } else if (currentPage == PageState::LOGIN_PAGE) {
            window.draw(LoginPageTitle);
            usernameFieldLogin.render(window);
            passwordFieldLogin.render(window);
            GoToDashboardviaLoginButton.render(window);
        } else if (currentPage == PageState::SIGNUP_PAGE) {
            window.draw(SignupPageTitle);
            usernameFieldSignup.render(window);
            passwordFieldSignup.render(window);
            emailFieldSignup.render(window);
            GoToDashboardviaSignupButton.render(window);
        } else if (currentPage == PageState::ERROR) {
            window.draw(errorTitle);
            errorText.setString(errorMessage);
            window.draw(errorText);
            errorBackButton.render(window);
        } else if (currentPage == PageState::DASHBOARD_PAGE) {
            window.draw(DashBoardPageTitle);
            window.draw(dashboardActionPrompt);
            GoToAccountDetailsPage.render(window);
            GoToUpdateAccountCredentialsPage.render(window);
            GoToManageTransactionsPage.render(window);
            GoToLogoutConfirmationPage.render(window);
            // Add button for Logout, View Account Details, Update Account Details, Manage Transactions
            // Render dashboard page elements here
        }else if (currentPage == PageState::VIEW_ACCOUNT_DETAILS_PAGE){
            window.draw(AccountDetailsPageTitle);
            window.draw(CurrentUserBalance);
            window.draw(CurrentUserMonthlyBugdet);
            window.draw(HaveReachedSavingsGoal);
            GoBackToDashboardFromAccountDetailsPage.render(window);
            GoToSetNewBugdetPage.render(window);
            GoToAddToBalancePage.render(window);
            GoToUpdateSavingsAmountSetPage.render(window);
            GoToUpdateSavingsGoalPage.render(window);
            GoToExportMonthlyReportPage.render(window);
        }else if (currentPage == PageState::UPDATE_ACCOUNT_CREDENTIALS_PAGE){
            window.draw(UpdateAccountCredentialsPageTitle);
            GoToUpdatePasswordPage.render(window);
            GoToUpdateEmailPage.render(window);
            GoBackToDashboardFromAccountDetailsPage.render(window);
        }else if (currentPage == PageState::SET_NEW_BUDGET){
            NewBudgetInputField.render(window);
            UpdateBudget.render(window);
            GoBackToViewAccountDetailsPage.render(window);
        }else if (currentPage == PageState::ADD_TO_BALANCE){
            AddToBalanceInputField.render(window);
            UpdateBalance.render(window);
            GoBackToViewAccountDetailsPage.render(window);
        }else if (currentPage == PageState::UPDATE_SAVINGS_ACCOUNT_AMOUNT){
            UpdateSavingsAmountInputField.render(window);
            UpdateSavingsAmount.render(window);
            GoBackToViewAccountDetailsPage.render(window);
        }else if (currentPage == PageState::EXPORT_MONTHLY_REPORT){
            window.draw(ExportMonthlyReportTitle);
            for (auto& button : ExportMonthlyReportButtons) {
                button->render(window);
            }
        }else if (currentPage == PageState::UPDATE_SAVINGS_GOAL){
            UpdateSavingsGoalInputField.render(window);
            UpdateSavingsGoal.render(window);
            GoBackToViewAccountDetailsPage.render(window);
        }else if (currentPage == PageState::LOGOUT_CONFIRMATION_PAGE){
            window.draw(LogoutConfirmationTitle);
            ConfirmLogoutButton.render(window);
            CancelLogoutButton.render(window);
        }else if (currentPage == PageState::UPDATE_PASSWORD){
            NewPasswordInputField.render(window);
            ConfirmNewPasswordInputField.render(window);
            UpdatePassword.render(window);
        }else if (currentPage == PageState::UPDATE_EMAIL){
            NewEmailInputField.render(window);
            UpdateEmail.render(window);
        }else if (currentPage == PageState::MANAGE_TRANSACTIONS){
            window.draw(ManageTransactionsPageTitle);
            GoToAddTransactionPage.render(window);
            GoToRemoveTransactionPage.render(window);
            GoBackToDashboardFromManageTransactionsPage.render(window);
        }else if (currentPage == PageState::ADD_TRANSACTION){
            TransactionTypeInputField.render(window);
            TransactionAmountInputField.render(window);
            TransactionDescriptionInputField.render(window);
            AddNewTransactionButton.render(window);
        }else if (currentPage == PageState::REMOVE_TRANSACTION){
            window.draw(RemoveTransactionPageTitle);
            for (auto& button : RemovableTransactionButtons) {
                button->render(window);
            }
            GoBackToDashboardFromManageTransactionsPage.render(window);
        }
        window.display();
    }
    for (auto* btn : RemovableTransactionButtons) {
        delete btn;
    }
    RemovableTransactionButtons.clear();
        
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
    newUser.SavingsAmount = 0.0;
    newUser.LoggedIn = true;
    newUser.SignUptime = ctime(&now);

    //Encrypt Password
    PasswordEncryption(newUser.password, 7); // Simple Caesar cipher with shift of 7
    // Generate unique user ID
    generateUniqueUserID(newUser);
    addUser(newUser);
    cout << "\tUser " << username << " signed up successfully. User ID: " << newUser.userID << endl;
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
    << newUser.balance << "," << newUser.MonthlyBudget << "," << newUser.SavingsAmount << "," << newUser.SavingsGoal << "," << newUser.LoggedIn<< "," << newUser.SignUptime<< "," 
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
            cout << user.MonthlyBudget << endl;
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
    << user.balance << "," << user.MonthlyBudget << "," << user.SavingsAmount << "," << user.SavingsGoal << "," << user.LoggedIn<< "," << user.SignUptime<< "," 
    << user.LoginLogoutTime << "," << endl;
    }
    outFile.close();
    return true;
}
// Retrieves user details based on username
User getUser(string username){
    for(User &user : users){
        if(user.username == username){
            cout << "\tUser " << username << " found." << endl;
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
                    user.SavingsAmount = stod(line);
                    break;
                case 8:
                    user.LoggedIn = stoi(line);
                    break;
                case 9:
                    user.SignUptime = line;
                    break;
                case 10:
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
bool SetMonthlyBudget(const string &userID, string budget){
    for(User &user : users){
        if(user.userID == userID){
            user.MonthlyBudget = stod(budget);
            cout << "Monthly budget updated to " << user.MonthlyBudget << " successfully for user " << user.username << endl;
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
    if(type == "Income"){
        UpdateBalance(userID, amount);
    } else if(type == "Expense"){
        if (amount > GetBalance(userID)){
            cout << "Failed to create transaction due to insufficient balance." << endl;
            return false;
        }else{
            UpdateBalance(userID, -amount);
        }
    } else{
        cout << "Invalid transaction type." << endl;
        return false;
    }
    for (User &user : users){
        if(user.userID == userID){
            updateUser(user,UserDataFilePath);
        }
    }
    transactions.push_back(newTransaction);
    return true;
}
// Removes a transaction from the user's transaction file and transaction vector and updates the user's balance accordingly
bool RemoveTransaction(const string &userID, const string &transactionNumber, const string &filePath){
    // Find transaction from transactions vector, remove it
    // Re-write the Transactions file with new, updated transactions vector
    auto it = remove_if(transactions.begin(), transactions.end(), [&userID, &transactionNumber](
        const Transaction &trans){
            return trans.userID == userID && trans.transactionNo == transactionNumber;
        });
    if(it != transactions.end()){
        Transaction transToRemove = *it;
        // Update user balance accordingly
        if(transToRemove.type == "Income"){
            UpdateBalance(userID, -transToRemove.amount);
        } else if(transToRemove.type == "Expense"){
            UpdateBalance(userID, transToRemove.amount);
        }
        transactions.erase(it, transactions.end());
        // Rewrite the Transactions file
        SaveAllTransactions(transactions, filePath);
        cout << "Transaction " << transactionNumber << " removed successfully for user ID " << userID << endl;
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
