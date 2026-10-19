/*
* Use of ChatGPT (GPT 6) for uint64_t usage cases
*/
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

uint64_t demoHash(const string& s);

struct User {
    string username;
    string salt;
    uint64_t passwordHash;
};

enum class CollisionStrategy {
    Chaining,
    Linear,
    Quadratic,
    DoubleHashing
};

class HashTable {
    private:
        struct Node {
            User user;
            Node* next;

            Node(const User& u, Node* n = nullptr)
                : user(u), next(n) {}
        };

        struct Slot {
            User user{};
            bool occupied = false;
            bool deleted = false;
        };

        CollisionStrategy strategy;
        vector<Node*> buckets;
        vector<Slot> slots;
        size_t count;
        size_t capacity;
        size_t rehashes;

        size_t indexFor(const string& key, size_t attempt) const;
        void resizeTable();
        bool insertWithoutResize(const User& user);
        void clearNodes();

    public:
        explicit HashTable(CollisionStrategy mode, size_t initialCapacity = 5);
        ~HashTable();

        HashTable(const HashTable&) = delete;
        HashTable& operator=(const HashTable&) = delete;

        bool insert(const User& user);
        const User* find(const string& username) const;
        bool remove(const string& username);
        double loadFactor() const;

        size_t size() const {return count; }
        size_t bucketCount() const { return capacity; }
        size_t rehashCount() const { return rehashes; }

};

#endif //HASHTABLE_H
