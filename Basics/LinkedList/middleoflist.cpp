#include <iostream>

struct Node {
    int value;
    Node* next;
};

// Function to print the linked list (for demonstration purposes)
void printList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        std::cout << current->value;
        if (current->next != nullptr) {
            std::cout << " -> ";
        }
        current = current->next;
    }
    std::cout << std::endl;
}

// Function to find the middle of the linked list
Node* middleofList(Node* head) {
    Node* slow = head;
    Node* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// Function to detect a cycle in the linked list
bool hasCycle(Node* head) {
    if (head == nullptr) return false;

    Node* slow = head;
    Node* fast = head;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;            // Move slow pointer by one
        fast = fast->next->next;      // Move fast pointer by two

        if (slow == fast) return true; // Cycle detected
    }

    return false; // No cycle
}

// Function to insert a node at the end of the linked list
void insertAtEnd(Node** headRef, int value) {
    Node* newNode = new Node{value, nullptr};
    if (*headRef == nullptr) {
        *headRef = newNode;
        return;
    }
    Node* current = *headRef;
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = newNode;
}

// Function to reverse a linked list
Node* reverseList(Node* head) {
    Node* prev = nullptr;
    Node* current = head;
    Node* nextNode = nullptr;
    while (current != nullptr) {
        nextNode = current->next;
        current->next = prev;
        prev = current;
        current = nextNode;
    }
    return prev;
}

int main() {
    Node* head = nullptr; // Initialize an empty list

    // Create the list: 1 -> 2 -> 3 -> nullptr
    insertAtEnd(&head, 1);
    insertAtEnd(&head, 2);
    insertAtEnd(&head, 3);
    insertAtEnd(&head, 3);
    insertAtEnd(&head, 2);

    // Print the list
    std::cout << "Original list: ";
    printList(head);

    // Find and print the middle of the list
    Node* middle = middleofList(head);
    if (middle != nullptr) {
        std::cout << "Middle of the list is: " << middle->value << std::endl;
    } else {
        std::cout << "The list is empty." << std::endl;
    }

    // Detect cycle in the list (expecting no cycle)
    if (hasCycle(head)) {
        std::cout << "Cycle detected in the linked list." << std::endl;
    } else {
        std::cout << "No cycle detected in the linked list." << std::endl;
    }

    // Create a cycle for testing: 1 -> 2 -> 3 -> 2 (cycle)
    // Comment out for testing non-cyclic behavior
    ///head->next->next->next = head->next;
    //printList(head);

    // Detect cycle in the modified list
    if (hasCycle(head)) {
        std::cout << "Cycle detected in the modified linked list." << std::endl;
    } else {
        std::cout << "No cycle detected in the modified linked list." << std::endl;
    }

    // Free allocated memory (clean up)
    Node* current = head;
    while (current != nullptr) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }

    return 0;
}
