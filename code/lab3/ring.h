
class slot {
public:
    slot(int id, int number);

    slot() {
        thread_id = 0;
        value = 0;
    };
    int thread_id;
    int value;
};


class Ring {
public:
    Ring(int sz);    // Constructor:  initialize variables, allocate space.
    ~Ring();         // Destructor:   deallocate space allocated above.
    void Put(slot *message); // Put a message the next empty slot.
    void Get(slot *message); // Get a message from the next  full slot.
    int Full();       // Returns non-0 if the ring is full, 0 otherwise.
    int Empty();      // Returns non-0 if the ring is empty, 0 otherwise.
private:
    int size;         // The size of the ring buffer.
    int in, out;      // Index of
    bool empty = true;
    bool full = false;
    slot *buffer;       // A pointer to an array for the ring buffer.
};


