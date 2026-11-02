#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <ctime>

using namespace std;

const int MAX_NORMAL = 20;
const int MAX_EMERGENCY = 5;

const int NORMAL = 1, EMERGENCY = 2, TREATING = 3, PAUSED = 4, COMPLETED = 5, REFERRED = 6, DEFERRED = 7;

struct Patient {
    int id, age, severity, status, doctor;
    string name, problem;
    time_t arrival;
};

vector<Patient> patients;
queue<int> normalQueue;
vector<int> emergencyQueue;
int currentPatient[2] = {-1, -1};
int pausedPatient[2] = {-1, -1};

int readInt(string msg, int low, int high) {
    int x; cout << msg; cin >> x; return x;
}

int findPatient(int id) {
    for (int i = 0; i < patients.size(); i++) {
        if (patients[i].id == id) return i;
    }
    return -1;
}

string getStatus(int s) 
{
    if (s == NORMAL) return "Normal Waiting";
    if (s == EMERGENCY) return "Emergency Waiting";
    if (s == TREATING) return "In Consultation";
    if (s == PAUSED) return "Paused";
    if (s == COMPLETED) return "Completed";
    if (s == REFERRED) return "Referred";
    return "Deferred";
}

int waitingTime(int id) {
    int i = findPatient(id);
    return (i == -1) ? 0 : (int)(difftime(time(0), patients[i].arrival) / 60);
}

int getTopEmergencyIndex() {
    if (emergencyQueue.empty()) return -1;
    int bestIdx = 0;
    for (int i = 1; i < emergencyQueue.size(); i++) {
        int curr = findPatient(emergencyQueue[i]), best = findPatient(emergencyQueue[bestIdx]);
        if (patients[curr].severity > patients[best].severity || 
           (patients[curr].severity == patients[best].severity && patients[curr].id < patients[best].id)) 
           {
            bestIdx = i;
        }
    }
    return bestIdx;
}

void addToQueue(int id) {
    int i = findPatient(id);
    if (i == -1) return;

    if (patients[i].severity >= 4) {
        if (emergencyQueue.size() >= MAX_EMERGENCY) 
        {
            patients[i].status = DEFERRED;
            cout << "Emergency queue full!\n";
            return;
        }
        emergencyQueue.push_back(id);
        patients[i].status = EMERGENCY;
        cout << "Added to emergency queue.\n";
    }
     else
      {
        if (normalQueue.size() >= MAX_NORMAL)
         {
            patients[i].status = DEFERRED;
            cout << "Normal queue full.\n";
            return;
        }
        normalQueue.push(id);
        patients[i].status = NORMAL;
        cout << "Added to normal queue.\n";
    }
}

void schedule() {
    for (int d = 0; d < 2; d++) 
    {
        if (currentPatient[d] != -1) continue;
        int id = -1;

        if (!emergencyQueue.empty()) {
            int idx = getTopEmergencyIndex();
            id = emergencyQueue[idx];
            emergencyQueue.erase(emergencyQueue.begin() + idx);
        } 
        else if (!normalQueue.empty()) {
            id = normalQueue.front();
            normalQueue.pop();
        }

        if (id == -1) continue;
        int i = findPatient(id);
        patients[i].status = TREATING;
        patients[i].doctor = d + 1;
        currentPatient[d] = id;
        cout << "\nDoctor " << d + 1 << " treating " << patients[i].name << " (Wait: " << waitingTime(id) << "m)\n";
    }
}

void preempt() {
    if (emergencyQueue.empty()) return;
    int idx = getTopEmergencyIndex(), id = emergencyQueue[idx], i = findPatient(id);
    if (patients[i].severity != 5) return;
    
    cout << "\nCRITICAL PATIENT: " << patients[i].name << "\n";
    for (int d = 0; d < 2; d++) {
        if (currentPatient[d] == -1) 
        { schedule();
           return; }
    }

    cout << "Current doctors:\n";
    for (int d = 0; d < 2; d++) {
        int j = findPatient(currentPatient[d]);
        cout << d + 1 << ". " << patients[j].name << " (S" << patients[j].severity << ")\n";
    }

    int d = readInt("Doctor to interrupt (0 to cancel): ", 0, 2);
    if (d == 0) return;
    d--;

    int oldID = currentPatient[d], old = findPatient(oldID);
    if (pausedPatient[d] != -1 || patients[old].severity == 5 || readInt("Staff approves? (1/0): ", 0, 1) == 0) {
        cout << "Interruption denied/cancelled.\n";
        return;
    }

    pausedPatient[d] = oldID;
    patients[old].status = PAUSED;
    emergencyQueue.erase(emergencyQueue.begin() + idx);
    currentPatient[d] = id;
    patients[i].status = TREATING;
    patients[i].doctor = d + 1;
    cout << patients[old].name << " paused. Emergency assigned to Doc " << d + 1 << "\n";
}

void registerPatient() {
    Patient p;
    p.id = 101 + patients.size();
    p.doctor = -1;
    p.arrival = time(0);

    cout << "\n--- NEW PATIENT ---\nName: ";
    cin.ignore(); getline(cin, p.name);
    p.age = readInt("Age: ", 0, 120);
    cout << "Problem: ";
    cin.ignore(); getline(cin, p.problem);
    p.severity = readInt("Severity (1-5): ", 1, 5);
    p.status = DEFERRED;
    patients.push_back(p);
    cout << "Registered! ID: " << p.id << "\n";

    addToQueue(p.id);
    schedule();
    if (patients[findPatient(p.id)].status == EMERGENCY && p.severity == 5) preempt();
}

void removeFromQueue(int id, int status) {
    if (status == NORMAL) {
        queue<int> temp;
        while (!normalQueue.empty()) {
            int x = normalQueue.front(); normalQueue.pop();
            if (x != id) temp.push(x);
        }
        normalQueue = temp;
    } else if (status == EMERGENCY) {
        for (auto it = emergencyQueue.begin(); it != emergencyQueue.end(); ++it) {
            if (*it == id) { emergencyQueue.erase(it); break; }
        }
    }
}

void updateSeverity() {
    int id = readInt("Patient ID: ", 1, 1000000);
    int i = findPatient(id);
    if (i == -1 || patients[i].status >= COMPLETED) { cout << "Invalid or closed patient.\n"; return; }

    int oldStatus = patients[i].status;
    if (oldStatus == NORMAL || oldStatus == EMERGENCY) removeFromQueue(id, oldStatus);
    
    patients[i].severity = readInt("New severity (1-5): ", 1, 5);

    // Simplified condition: if treating/paused, don't re-queue automatically. Otherwise re-queue.
    if (oldStatus == TREATING || oldStatus == PAUSED) {
        cout << "Severity updated. Inform doctor manually.\n";
    } else {
        addToQueue(id);
        schedule();
        if (patients[i].status == EMERGENCY && patients[i].severity == 5) preempt();
    }
}

void finishConsultation() {
    int d = readInt("Doctor number (1-2): ", 1, 2) - 1;
    if (currentPatient[d] == -1) { cout << "Doctor is free.\n"; return; }

    int id = currentPatient[d], i = findPatient(id);
    int choice = readInt("1. Complete  2. Refer: ", 1, 2);
    patients[i].status = (choice == 1) ? COMPLETED : REFERRED;
    patients[i].doctor = -1;
    currentPatient[d] = -1;

    if (pausedPatient[d] != -1 && readInt("Resume paused patient? (1/0): ", 0, 1) == 1) {
        int old = findPatient(pausedPatient[d]);
        currentPatient[d] = pausedPatient[d];
        pausedPatient[d] = -1;
        patients[old].status = TREATING;
        cout << "Resumed " << patients[old].name << "\n";
    }
    schedule();
}

void resumePatient() {
    int d = readInt("Doctor number (1-2): ", 1, 2) - 1;
    if (pausedPatient[d] == -1 || currentPatient[d] != -1) { cout << "Cannot resume.\n"; return; }
    
    int i = findPatient(pausedPatient[d]);
    currentPatient[d] = pausedPatient[d];
    pausedPatient[d] = -1;
    patients[i].status = TREATING;
    cout << "Resumed " << patients[i].name << "\n";
}

void searchPatient() {
    int id = readInt("Patient ID: ", 1, 1000000);
    int i = findPatient(id);
    if (i == -1) { cout << "Not found.\n"; return; }

    Patient p = patients[i];
    cout << "\nID: " << p.id << "\nName: " << p.name << "\nAge: " << p.age 
         << "\nProblem: " << p.problem << "\nSeverity: " << p.severity 
         << "\nStatus: " << getStatus(p.status) << "\nDoctor: " << p.doctor 
         << "\nWait time: " << waitingTime(id) << " mins\n";
}

void displayQueues() {
    cout << "\n--- NORMAL QUEUE ---\n";
    queue<int> nq = normalQueue;
    while (!nq.empty()) {
        int id = nq.front(); nq.pop(); int i = findPatient(id);
        cout << id << ". " << patients[i].name << " (S" << patients[i].severity << ")\n";
    }

    cout << "\n--- EMERGENCY QUEUE ---\n";
    for (int id : emergencyQueue) {
        int i = findPatient(id);
        cout << id << ". " << patients[i].name << " (S" << patients[i].severity << ")\n";
    }
    cout << "Capacity -> Normal: " << normalQueue.size() << "/" << MAX_NORMAL 
         << ", Emergency: " << emergencyQueue.size() << "/" << MAX_EMERGENCY << "\n";
}

void displayDoctors() {
    for (int d = 0; d < 2; d++) {
        cout << "Doctor " << d + 1 << ": ";
        if (currentPatient[d] == -1) cout << "Free";
        else cout << "Treating " << patients[findPatient(currentPatient[d])].name;
        if (pausedPatient[d] != -1) cout << " [Paused: " << patients[findPatient(pausedPatient[d])].name << "]";
        cout << "\n";
    }
}

int main() {
    while (true) {
        cout << "\n=== CLINIC MENU ===\n"
             << "1. Register\n2. Search\n3. Queues\n4. Doctors\n"
             << "5. Update Severity\n6. Finish\n7. Preempt\n8. Resume\n9. Exit\n";
        int choice = readInt("Choice: ", 1, 9);
        if (choice == 1) registerPatient();
        else if (choice == 2) searchPatient();
        else if (choice == 3) displayQueues();
        else if (choice == 4) displayDoctors();
        else if (choice == 5) updateSeverity();
        else if (choice == 6) finishConsultation();
        else if (choice == 7) preempt();
        else if (choice == 8) resumePatient();
        else if (choice == 9) break;
    }
    return 0;
}