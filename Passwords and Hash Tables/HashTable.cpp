/*
* ChatGPT (GPT 6) used for FNV-1a setting basis
*/
#include "HashTable.h"
#include <iostream>
#include <utility>

using namespace std;

// Educational hash function (chatgpt)
uint64_t demoHash(const string& s) {
    uint64_t h = 1469598103934665603ULL;

    for (unsigned char c : s) {
        h = (h ^ c) * 1099511628211ULL;
    }

    return h;
}

// Constructor
HashTable::HashTable(CollisionStrategy mode, size_t initialCapacity) {
    strategy = mode;
    capacity = initialCapacity < 5 ? 5: initialCapacity; // what?

    // Find an odd prime capacity
    auto isPrime = [](size_t n) {
        if (n < 2) return false;

        for (size_t i = 2; i <= n / i; i++) {
            if(n % i == 0) return false;
        }

        return true;
    };

    while (!isPrime(capacity)) {
        capacity++;
    }

    count = 0;
    rehashes = 0;

    if (strategy == CollisionStrategy::Chaining) {
        buckets.resize(capacity, nullptr);
    }
    else {
        slots.resize(capacity);
    }
}

// Destructor
HashTable::~HashTable() {
    clearNodes();
}

// Delete All linked-list nodes
void HashTable::clearNodes() {
    for (size_t i = 0; i < buckets.size(); i++) {
        Node* current = buckets[i];

        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }

        buckets[i] = nullptr;
    }
}

// Calculate index for collision strategy
size_t HashTable::indexFor(const string& key, size_t attempt) const {
    uint64_t hash = demoHash(key);
    size_t index = hash % capacity;

    if (strategy == CollisionStrategy::Chaining) {
        return index;
    }

    else if (strategy == CollisionStrategy::Linear) {
        return (index + attempt) % capacity;
    }

    else if (strategy == CollisionStrategy::Quadratic) {
        return (index + attempt * attempt) % capacity;
    }
    else {
        size_t step = 1 + (hash % (capacity - 1));
        return (index + attempt * step) % capacity;
    }
}

// Insert without resize
bool HashTable::insertWithoutResize(const User& user) {
    if (strategy == CollisionStrategy::Chaining) {
        size_t index = indexFor(user.username, 0);
        Node* current = buckets[index];

        while (current != nullptr) {
            if (current->user.username == user.username) {
                return false;
            }

            current = current->next;
        }

        buckets[index] = new Node(user, buckets[index]);
        count++;
        return true;
    }

    size_t firstDeleted = capacity;

    for(size_t attempt = 0; attempt < capacity; attempt++) {
        size_t index = indexFor(user.username, attempt);

        if (slots[index].occupied) {
            if (slots[index].user.username == user.username) {
                return false;
            }
        }
        else if (slots[index].deleted) {
            if (firstDeleted == capacity) {
                firstDeleted = index;
            }
        }

        else {
            if (firstDeleted != capacity) {
                index = firstDeleted;
            }

            slots[index].user = user;
            slots[index].occupied = true;
            slots[index].deleted = false;
            count++;
            return true;
        }
    }

    if (firstDeleted != capacity) {
        slots[firstDeleted].user = user;
        slots[firstDeleted].occupied = true;
        slots[firstDeleted].deleted = false;
        count++;
        return true;
    }

    return false;
}

// Insert a user
bool HashTable::insert(const User& user) {
    if (find(user.username) != nullptr) {
        return false;
    }

    if ((count + 1.0) / capacity > 0.5) {
        resizeTable();
    }

    return insertWithoutResize(user);
}

// Find a user by username
const User* HashTable::find(const string& username) const {
    if (strategy == CollisionStrategy::Chaining) {
        size_t index = indexFor(username, 0);
         Node* current = buckets[index];

        while (current != nullptr) {
            if (current->user.username == username) {
                return &current->user;
            }

            current = current -> next;
        }

        return nullptr;
    }

    for (size_t attempt = 0; attempt < capacity; attempt++) {
        size_t index = indexFor(username, attempt);

        if(!slots[index].occupied && !slots[index].deleted) {
            return nullptr;
        }

        if (slots[index].occupied && slots[index].user.username == username) {
            return &slots[index].user;
        }
    }

    return nullptr;
}

// Remove a user
bool HashTable::remove(const string& username) {
    if (strategy == CollisionStrategy::Chaining) {
        size_t index = indexFor(username, 0);
        Node* current = buckets[index];
        Node* previous = nullptr;

        while (current != nullptr) {
            if (current->user.username == username) {
                if (previous == nullptr) {
                    buckets[index]= current-> next;
                }

                else {
                    previous-> next = current->next;
                }

                delete current;
                count--;
                return true;
            }

            previous = current;
            current = current->next;
        }
            
        return false;
    }

    for (size_t attempt = 0; attempt < capacity; attempt++) {
        size_t index = indexFor(username, attempt);

        if (!slots[index].occupied && !slots[index].deleted) {
            return false;
        }

        if (slots[index].occupied && slots[index].user.username == username) {
            slots[index].occupied = false;
            slots[index].deleted = true;
            count--;
            return true;
        }
    }

    return false;
}

// Calculate load factor
double HashTable::loadFactor() const {
    return static_cast<double>(count) / capacity;
}

// Resize and rehash all users
void HashTable::resizeTable() {
    size_t newCapacity = capacity * 2 + 1;

    auto isPrime = [](size_t n) {
        if (n < 2) return false;
        
        for (size_t i= 2; i <= n / i; i++) {
            if (n % i == 0) return false;
        }

        return true;
    };

    while (!isPrime(newCapacity)) {
        newCapacity++;
    }

    vector<User> users;

    if (strategy == CollisionStrategy::Chaining) {
        for (size_t i = 0; i < buckets.size(); i++) {
            Node* current = buckets[i];

            while (current != nullptr) {
                users.push_back(current->user);
                current = current->next;
            }
        }

        clearNodes();
        buckets.clear();
    }
    else {
        for (size_t i = 0; i< slots.size(); i++) {
            if (slots[i].occupied) {
                users.push_back(slots[i].user);
            }
        }

        slots.clear();
    }

     capacity = newCapacity;
     count = 0;
     rehashes++;

     if(strategy == CollisionStrategy::Chaining) {
        buckets.resize(capacity, nullptr);
     }

     else {
        slots.resize(capacity);
     }

     for (size_t i=0; i < users.size(); i++) {
        insertWithoutResize(users[i]);
     }
}