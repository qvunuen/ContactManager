#include <iostream>
#include <string>
#include <limits>
#include "ContactList.h"
#include "Contact.h"
#include <iomanip>

using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void showMenu() {
    cout << "\n======================================" << endl;
    cout << "\n--- PhoneBook Manager 1.0 ---" << endl;
    cout << "======================================" << endl;
    cout << "1. List display" << endl;
    cout << "2. Add Contact" << endl;
    cout << "3. Search" << endl;
    cout << "4. Update Contacts" << endl;
    cout << "5. Delete Contact" << endl;
    cout << "0. Save and Exit" << endl;
    cout << "--------------------------------------" << endl;
    cout << "Your choice (0 - 5): ";
}

int main() {
    ContactList myDirectory("../data/contacts.txt");
    int choice;

    do {
        showMenu();
        cin >> choice;
        clearInput();
        switch (choice) {
            case 1:
                cout << "\n--- Contact List ---" << endl;
                myDirectory.showAllContacts();
                break;
            case 2: { // Add Contact
                string n, p, e;
                cout << "Full Name: "; getline(cin, n);
                cout << "Phone Number: "; getline(cin, p);
                cout << "Email: "; getline(cin, e);
                myDirectory.addContact(Contact(n, p, e));
                cout << "Contact added successfully!" << endl;
                break;
            }
            case 3: { 
                string keyword;
                cout << "\n======================================" << endl;
                cout << "             SEARCH CONTACT             " << endl;
                cout << "========================================" << endl;
                cout << "Name or Number to search (Enter to Cancel): ";
                getline(cin, keyword);

                if (keyword.empty()) {
                    cout << "=> Abandon Search." << endl;
                    break;
                }

                // Gọi hàm search mới (trả về danh sách các vị trí tìm thấy)
                vector<int> results = myDirectory.searchContactsAdvanced(keyword);

                if (results.empty()) {
                    cout << "=> Not Found: \"" << keyword << "\"" << endl;
                } else {
                    cout << "\n[ Found " << results.size() << " Results ]" << endl;
                    cout << string(65, '-') << endl;
                    cout << left << setw(5)  << "Order" 
                                 << setw(20) << "Full Name" 
                                 << setw(15) << "Phone Number" 
                                 << setw(25) << "E-Mail" << endl;
                    cout << string(65, '-') << endl;

                    // In danh sách kết quả
                    for (int i = 0; i < results.size(); i++) {
                        cout << left << setw(5) << (i + 1); // In số thứ tự 1, 2, 3...
                        myDirectory.showContactAtIndex(results[i]); 
                    }
                    cout << string(65, '-') << endl;

                    // Tích hợp Menu phụ (Sub-menu) thao tác nhanh
                    int subChoice;
                    cout << "\nThao tac nhanh voi ket qua tim kiem:" << endl;
                    cout << "1. Edit    2. Delete    0. Return to main menu" << endl;
                    cout << "Choice: ";
                    
                    if (cin >> subChoice && subChoice != 0) {
                        clearInput(); // Xóa bộ nhớ đệm
                        int targetNum;
                        cout << "Enter the Order of the Contact (1 - " << results.size() << "): ";
                        cin >> targetNum;
                        clearInput();

                        if (targetNum > 0 && targetNum <= results.size()) {
                            int realIndex = results[targetNum - 1]; // Chuyển STT về index thật trong vector gốc

                            if (subChoice == 1) {
                                string n, p, e;
                                cout << "\n--- Edit Information ---" << endl;
                                cout << "New Name: "; getline(cin, n);
                                cout << "New Number: "; getline(cin, p);
                                cout << "New E-Mail: "; getline(cin, e);
                                myDirectory.updateContact(realIndex, Contact(n, p, e));
                                cout << "=> Update Complete!" << endl;
                            } 
                            else if (subChoice == 2) {
                                char confirm;
                                cout << "Delete? (y/n): "; cin >> confirm;
                                if (confirm == 'y' || confirm == 'Y') {
                                    myDirectory.deleteContact(realIndex);
                                    cout << "=> Deleted!" << endl;
                                }
                            }
                        } else {
                            cout << "=> Order invalid!" << endl;
                        }
                    } else {
                        clearInput();
                    }
                }
                break;
            }

            case 4: { // Update
                string keyword;
                cout << "Enter the Name/Number you want to edit: "; getline(cin, keyword);
                int index = myDirectory.searchContact(keyword);

                if (index != -1) {
                    string n, p, e;
                    cout << "Enter new information:" << endl;
                    cout << "New name: "; getline(cin, n);
                    cout << "New number: "; getline(cin, p);
                    cout << "New E-Mail: "; getline(cin, e);
                    
                    if (myDirectory.updateContact(index, Contact(n, p, e))) {
                        cout << "Update Complete!" << endl;
                    }
                } else {
                    cout << "Not found!" << endl;
                }
                break;
            }

            case 5: { // Xóa
                string keyword;
                cout << "Enter the Name/Number you want to delete: "; getline(cin, keyword);
                int index = myDirectory.searchContact(keyword);

                if (index != -1) {
                    char confirm;
                    cout << "Do you want to delete (y/n): "; cin >> confirm;
                    if (confirm == 'y' || confirm == 'Y') {
                        myDirectory.deleteContact(index);
                        cout << "Deleted!" << endl;
                    }
                } else {
                    cout << "Not found!" << endl;
                }
                break;
            }
            case 0:
                myDirectory.saveToFile();
                break;
            default:
                cout << "Please only enter the numbers which were listed on the menu!" << endl;
        }
    } while (choice != 0);

    return 0;
}