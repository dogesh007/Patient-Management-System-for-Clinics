
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <ctime>
#include <limits>
using namespace std;

const int MAX_NORMAL = 20;
const int MAX_EMERGENCY = 5;

const int NORMAL = 1;
const int EMERGENCY = 2;
const int TREATING = 3;
const int PAUSED = 4;
const int COMPLETED = 5;
const int REFERRED = 6;
const int DEFERRED = 7;

struct Patient 
{
    int id, age, severity, status, doctor;
    string name, problem;
    time_t arrival;
};

vector<Patient> patients;
unordered_map<int, int> patientIndex;
queue<int> normalQueue;
priority_queue<pair<int, int> > emergencyQueue;

int currentPatient[2] = {-1, -1};
int pausedPatient[2] = {-1, -1};

int readInt(string message, int low, int high) 
{
    int x;
    while (true)
     {
        cout << message;
        if (cin >> x && x >= low && x <= high)
         {
            cin.ignore(10000, '\n');
            return x;
        }
        cout << "Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

int findPatient(int id)
 {
    if (patientIndex.find(id) == patientIndex.end())
        return -1;
    return patientIndex[id];
}

string getStatus(int s) 
{
    if (s == NORMAL) return "Normal Waiting";
    if (s == EMERGENCY) return "Emergency Waiting";
    if (s == TREATING) return "In Consultation";
    if (s == PAUSED) return "Paused";
    if (s == COMPLETED) return "Completed";
    if (s == REFERRED) return "Referred";
    return "Deferred / Needs Staff Attention";
}

int waitingTime(int id)
{
    int i = findPatient(id);
    if (i == -1) return 0;
    return (int)(difftime(time(0), patients[i].arrival) / 60);
}

void addToQueue(int id) 
{
    int i = findPatient(id);
    if (i == -1) return;

    if (patients[i].severity >= 4)
     {
        if ((int)emergencyQueue.size() >= MAX_EMERGENCY) 
        {
            patients[i].status = DEFERRED;
            cout << "EMERGENCY CAPACITY FULL!\n";
            cout << "Alert staff for immediate assessment/referral.\n";
            return;
        }
        emergencyQueue.push(make_pair(patients[i].severity, -id));
        patients[i].status = EMERGENCY;
        cout << "Added to emergency priority queue.\n";
    }

    else 
    {
        if ((int)normalQueue.size() >= MAX_NORMAL) 
        {
            patients[i].status = DEFERRED;
            cout << "Normal queue full. Offer later appointment.\n";
            return;
        }
        normalQueue.push(id);
        patients[i].status = NORMAL;
        cout << "Added to normal FIFO queue.\n";
    }
}

void schedule() 
{
    for (int d = 0; d < 2; d++) 
    {
        if (currentPatient[d] != -1) continue;

        int id = -1;

        if (!emergencyQueue.empty()) 
        {
            id = -emergencyQueue.top().second;
            emergencyQueue.pop();
        }
        else if (!normalQueue.empty())
     {
            id = normalQueue.front();
            normalQueue.pop();
        }

        if (id == -1) continue;

        int i = findPatient(id);
        patients[i].status = TREATING;
        patients[i].doctor = d + 1;
        currentPatient[d] = id;

        cout << "\nDoctor " << d + 1;
        cout << " is treating " << patients[i].name << "\n";
        cout << "Waiting time: " << waitingTime(id)
             << " minutes\n";
    }
}

void preempt() 
{
    if (emergencyQueue.empty()) return;

    int id = -emergencyQueue.top().second;
    int i = findPatient(id);

    if (patients[i].severity != 5) return;

    cout << "\nCRITICAL PATIENT: " << patients[i].name << "\n";
    cout << "Emergency staff attention required.\n";

    for (int d = 0; d < 2; d++)
    {
        if (currentPatient[d] == -1) 
        {
            schedule();
            return;
        }
    }

    cout << "\nCurrent doctors:\n";
    for (int d = 0; d < 2; d++) 
    {
        int j = findPatient(currentPatient[d]);
        cout << d + 1 << ". " << patients[j].name;
        cout << " (Severity " << patients[j].severity << ")";
        if (pausedPatient[d] != -1)
            cout << " [Already has paused patient]";
        cout << "\n";
    }

    int d = readInt("Doctor to interrupt (0 to cancel): ", 0, 2);
    if (d == 0) return;
    d--;

    int oldID = currentPatient[d];
    int old = findPatient(oldID);

    if (pausedPatient[d] != -1 || patients[old].severity == 5) 
    {
        cout << "This doctor cannot be interrupted.\n";
        return;
    }

    int approval = readInt("Staff approves interruption? (1 Yes, 0 No): ", 0, 1);
    if (approval == 0) 
    {
        cout << "Interruption declined. Escalate emergency.\n";
        return;
    }

    pausedPatient[d] = oldID;
    patients[old].status = PAUSED;

    emergencyQueue.pop();
    currentPatient[d] = id;
    patients[i].status = TREATING;
    patients[i].doctor = d + 1;

    cout << patients[old].name << " paused.\n";
    cout << "Doctor " << d + 1 << " now treating "
         << patients[i].name << "\n";
}

void registerPatient() 
{
    Patient p;
    p.id = 101 + (int)patients.size();
    p.doctor = -1;
    p.arrival = time(0);

    cout << "\n--- NEW PATIENT ---\n";
    cout << "Name: ";
    getline(cin, p.name);

    p.age = readInt("Age: ", 0, 120);

    cout << "Main problem: ";
    getline(cin, p.problem);

    cout << "\nNurse performs initial triage.\n";
    p.severity = readInt("Nurse-assigned severity (1-5): ", 1, 5);

    p.status = DEFERRED;
    patientIndex[p.id] = (int)patients.size();
    patients.push_back(p);

    cout << "Patient registered. ID: " << p.id << "\n";

    addToQueue(p.id);
    schedule();

    if (patients[findPatient(p.id)].status == EMERGENCY && p.severity == 5)
        preempt();
}

void removeFromQueue(int id, int status) 
{
    if (status == NORMAL) 
    {
        queue<int> temp;
        while (!normalQueue.empty())
        {
            int x = normalQueue.front();
            normalQueue.pop();
            if (x != id) temp.push(x);
        }
        normalQueue = temp;
    }
    else if (status == EMERGENCY) 
    {
        priority_queue<pair<int, int> > temp;
        while (!emergencyQueue.empty()) 
        {
            pair<int, int> x = emergencyQueue.top();
            emergencyQueue.pop();
            if (-x.second != id) temp.push(x);
        }
        emergencyQueue = temp;
    }
}

void updateSeverity() 
{
    int id = readInt("Patient ID: ", 1, 1000000);
    int i = findPatient(id);

    if (i == -1)
 {
        cout << "Patient not found.\n";
        return;
    }

    if (patients[i].status == COMPLETED || patients[i].status == REFERRED) {
        cout << "Consultation already closed.\n";
        return;
    }

    int severity = readInt("New nurse-assigned severity: ", 1, 5);
    int oldStatus = patients[i].status;

    if (oldStatus == NORMAL || oldStatus == EMERGENCY)
        removeFromQueue(id, oldStatus);

    patients[i].severity = severity;

    if (oldStatus == NORMAL || oldStatus == EMERGENCY) 
    {
        addToQueue(id);
        schedule();

        if (patients[i].status == EMERGENCY && severity == 5)
            preempt();
    }
    else if (oldStatus == TREATING || oldStatus == PAUSED)
    {
        cout << "Severity updated. Notify treating doctor.\n";
        cout << "No automatic queue transfer.\n";
    }
    else 
    {
        addToQueue(id);
        schedule();
    }
}

void finishConsultation() 
{
    int d = readInt("Doctor number (1-2): ", 1, 2) - 1;

    if (currentPatient[d] == -1)
    {
        cout << "Doctor is available.\n";
        return;
    }

    int id = currentPatient[d];
    int i = findPatient(id);

    cout << "Patient: " << patients[i].name << "\n";
    int choice = readInt("1. Complete  2. Refer: ", 1, 2);

    if (choice == 1) patients[i].status = COMPLETED;
    else patients[i].status = REFERRED;

    patients[i].doctor = -1;
    currentPatient[d] = -1;

    cout << "Consultation closed.\n";

    if (pausedPatient[d] != -1) 
    {
        int approval = readInt("Resume paused patient? (1 Yes, 0 No): ", 0, 1);
        if (approval == 1)
     {
            int oldID = pausedPatient[d];
            int old = findPatient(oldID);

            currentPatient[d] = oldID;
            pausedPatient[d] = -1;
            patients[old].status = TREATING;

            cout << "Resumed " << patients[old].name << "\n";
        }
    }

    schedule();
}

void resumePatient() 
{
    int d = readInt("Doctor number (1-2): ", 1, 2) - 1;

    if (pausedPatient[d] == -1) 
    {
        cout << "No paused patient.\n";
        return;
    }

    if (currentPatient[d] != -1) 
    {
        cout << "Doctor is busy.\n";
        return;
    }

    int id = pausedPatient[d];
    int i = findPatient(id);

    currentPatient[d] = id;
    pausedPatient[d] = -1;
    patients[i].status = TREATING;

    cout << "Resumed " << patients[i].name << "\n";
}

void searchPatient() 
{
    int id = readInt("Patient ID: ", 1, 1000000);
    int i = findPatient(id);

    if (i == -1) 
    {
        cout << "Patient not found.\n";
        return;
    }

    Patient p = patients[i];

    cout << "\nID: " << p.id;
    cout << "\nName: " << p.name;
    cout << "\nAge: " << p.age;
    cout << "\nProblem: " << p.problem;
    cout << "\nSeverity: " << p.severity;
    cout << "\nStatus: " << getStatus(p.status);
    cout << "\nDoctor: " << p.doctor;
    cout << "\nElapsed time since arrival: "
         << waitingTime(id) << " minutes\n";
}

void displayQueues() 
{
    cout << "\n--- NORMAL QUEUE ---\n";
    queue<int> nq = normalQueue;

    while (!nq.empty()) {
        int id = nq.front();
        nq.pop();
        int i = findPatient(id);

        cout << id << " " << patients[i].name;
        cout << " | S" << patients[i].severity << "\n";
    }

    cout << "\n--- EMERGENCY QUEUE ---\n";
    priority_queue<pair<int, int> > eq = emergencyQueue;

    while (!eq.empty()) 
    {
        int id = -eq.top().second;
        eq.pop();
        int i = findPatient(id);

        cout << id << " " << patients[i].name;
        cout << " | S" << patients[i].severity << "\n";
    }

    cout << "\nCapacity: Normal " << normalQueue.size();
    cout << "/" << MAX_NORMAL;
    cout << ", Emergency " << emergencyQueue.size();
    cout << "/" << MAX_EMERGENCY << "\n";
}

void displayDoctors() 
{
    for (int d = 0; d < 2; d++) 
    {
        cout << "\nDoctor " << d + 1 << ": ";

        if (currentPatient[d] == -1)
            cout << "Available";
        else 
        {
            int i = findPatient(currentPatient[d]);
            cout << "Treating " << patients[i].name;
        }

        if (pausedPatient[d] != -1)
     {
            int i = findPatient(pausedPatient[d]);
            cout << " | Paused: " << patients[i].name;
        }
        cout << "\n";
    }
}

int main() 
{
    int choice;

    while (true)
 {
        cout << "\n====== CLINIC MANAGEMENT ======\n";
        cout << "1. Register and Triage Patient\n";
        cout << "2. Search Patient\n";
        cout << "3. Display Waiting Queues\n";
        cout << "4. Display Doctors\n";
        cout << "5. Update Severity\n";
        cout << "6. Complete / Refer Consultation\n";
        cout << "7. Request Emergency Preemption\n";
        cout << "8. Resume Paused Patient\n";
        cout << "9. Exit\n";

        choice = readInt("Enter choice: ", 1, 9);

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
