
/*
* main.cpp
* Use of ChatGPT (GPT 6) for testing hash table
* collision strategies and login verification.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include "HashTable.h"

using namespace std;

// Pepper shared by all users
const string PEPPER = "ClassroomSecretPepper";

// Create a user with a salt and hashed password
User createUser(const string& username,
                const string& password,
                const string& salt) {

    User user;

    user.username = username;
    user.salt = salt;
    user.passwordHash = demoHash(salt + password + PEPPER);

    return user;
}

// Verify username and password
void login(const HashTable& table,
           const string& username,
           const string& password) {

    const User* user = table.find(username);

    if (user == nullptr) {
        cout << "Login " << username
             << ": Unknown user!" << endl;
        return;
    }

    uint64_t submittedHash =
        demoHash(user->salt + password + PEPPER);

    if (submittedHash == user->passwordHash) {
        cout << "Login " << username
             << ": Successful!" << endl;
    }
    else {
        cout << "Login " << username
             << ": Incorrect password!" << endl;
    }
}

// Display the selected collision strategy
string strategyName(CollisionStrategy strategy) {
    switch (strategy) {
        case CollisionStrategy::Chaining:
            return "Separate Chaining";

        case CollisionStrategy::Linear:
            return "Linear Probing";

        case CollisionStrategy::Quadratic:
            return "Quadratic Probing";

        case CollisionStrategy::DoubleHashing:
            return "Double Hashing";
    }

    return "Unknown";
}

// Test a collision strategy
void testStrategy(CollisionStrategy strategy) {
    cout << "\n====================================" << endl;
    cout << strategyName(strategy) << endl;
    cout << "====================================" << endl;

    HashTable table(strategy, 5);

    // Create five users
    User user1 = createUser("alice", "pass123", "saltA");
    User user2 = createUser("bob", "hello456", "saltB");
    User user3 = createUser("carol", "secure789", "saltC");
    User user4 = createUser("dave", "password321", "saltD");
    User user5 = createUser("erin", "welcome123", "saltE");

    // Insert users
    cout << "\n----- INSERT USERS -----" << endl;

    User users[] = {user1, user2, user3, user4, user5};

    for (int i = 0; i < 5; i++) {
        size_t previousRehashes = table.rehashCount();

        bool inserted = table.insert(users[i]);

        cout << "Insert " << users[i].username
             << ": " << (inserted ? "Success" : "Failed")
             << endl;

        cout << "Size: " << table.size()
             << " | Capacity: " << table.bucketCount()
             << " | Load Factor: " << fixed
             << setprecision(4) << table.loadFactor()
             << endl;

        if (table.rehashCount() > previousRehashes) {
            cout << "*** AUTOMATIC REHASH OCCURRED ***"
                 << endl;
        }

        cout << endl;
    }

    // Demonstrate login attempts
    cout << "\n----- LOGIN TESTS -----" << endl;

    login(table, "alice", "pass123");
    login(table, "alice", "wrongpassword");
    login(table, "unknown", "password");

    // Demonstrate find
    cout << "\n----- FIND USER -----" << endl;

    const User* found = table.find("carol");

    if (found != nullptr) {
        cout << "Found user: " << found->username << endl;
        cout << "Salt: " << found->salt << endl;
        cout << "Password Hash: "
             << found->passwordHash << endl;
    }
    else {
        cout << "User not found!" << endl;
    }

    // Demonstrate removal
    cout << "\n----- REMOVE USER -----" << endl;

    if (table.remove("bob")) {
        cout << "Bob removed successfully!" << endl;
    }
    else {
        cout << "Bob not found!" << endl;
    }

    if (table.find("bob") == nullptr) {
        cout << "Bob no longer exists in the table."
             << endl;
    }

    // Display final table statistics
    cout << "\n----- FINAL STATISTICS -----" << endl;

    cout << "Size: " << table.size() << endl;
    cout << "Capacity: " << table.bucketCount() << endl;
    cout << "Load Factor: " << fixed
         << setprecision(4) << table.loadFactor()
         << endl;
    cout << "Total Rehashes: "
         << table.rehashCount() << endl;
}

int main() {
    cout << "===== HASH TABLE USER SYSTEM ====="
         << endl;

    testStrategy(CollisionStrategy::Chaining);
    testStrategy(CollisionStrategy::Linear);
    testStrategy(CollisionStrategy::Quadratic);
    testStrategy(CollisionStrategy::DoubleHashing);

    cout << "\n===== ALL TESTS COMPLETED ====="
         << endl;

    return 0;
}
