#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <set>
#include <string>
#include <ctime>

using namespace std;

// stats
const int NORMAL = 1, EMERGENCY = 2, TREATING = 3, PAUSED = 4, COMPLETED = 5, REFERRED = 6, DEFERRED = 7;

struct Patient 
{
    int id, age, severity, status, doctor;
    string name, problem;
    time_t arrival;
};

unordered_map<int, Patient> patientsDB;
queue<int> normalQueue;

struct EmgItem 
{
    int id;
    int severity;
    time_t arrival;
    
   
    bool operator<(const EmgItem& other) const {
        if (severity != other.severity) return severity > other.severity; 
        if (arrival != other.arrival) return arrival < other.arrival;
        return id < other.id;
    }
};
set<EmgItem> emergencyQueue;

vector<stack<int>> pausedPatients;
vector<int> currentPatient; 

int numDoctors = 2;
int maxNormal = 20;
int maxEmergency = 5;
int patientIdCounter = 101;

int readInt(string msg, int low, int high) 
{
    int x; 
    while (true) 
    {
        cout << msg;
        if (cin >> x && x >= low && x <= high) break;
        cout << "Invalid input. Please enter a number between " << low << " and " << high << ".\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    return x;
}

string getStatus(int s)
 {
    switch(s)
    {
        case NORMAL: return "Normal Waiting";
        case EMERGENCY: return "Emergency Waiting";
        case TREATING: return "In Consultation";
        case PAUSED: return "Paused";
        case COMPLETED: return "Completed";
        case REFERRED: return "Referred";
        default: return "Deferred";
    }
}

int waitingTime(int id)
{
    if (patientsDB.find(id) == patientsDB.end()) return 0;
    return (int)(difftime(time(0), patientsDB[id].arrival) / 60);
}

void schedule() 
{
    for (int d = 0; d < numDoctors; d++) 
    {
        if (currentPatient[d] != -1) continue;   
        
        int id = -1;
        if (!emergencyQueue.empty()) 
        {
            auto topEmg = emergencyQueue.begin();
            id = topEmg->id;
            emergencyQueue.erase(topEmg);
        } 
        else if (!normalQueue.empty()) 
        {
            id = normalQueue.front();
            normalQueue.pop();
        }

        if (id == -1) continue;

        patientsDB[id].status = TREATING;
        patientsDB[id].doctor = d + 1;
        currentPatient[d] = id;
        cout << "\n[SCHEDULE] Doctor " << d + 1 << " is now treating " << patientsDB[id].name 
             << " (Wait: " << waitingTime(id) << "m)\n";
    }
}

void preempt(int id) 
{
    int severity = patientsDB[id].severity;
    cout << "\n!!! CRITICAL PREEMPTION PROTOCOL INITIATED for " << patientsDB[id].name << " !!!\n";
    
    // checking doctor is free or not
    for (int d = 0; d < numDoctors; d++) 
    {
        if (currentPatient[d] == -1) 
        { 
            schedule(); 
            return; 
        }
    }

   
    int targetDoc = -1;
    int lowestSeverity = 10;
    
    for (int d = 0; d < numDoctors; d++) 
    {
        int currentTreatedId = currentPatient[d];
        int currentSev = patientsDB[currentTreatedId].severity;
        
        if (currentSev < lowestSeverity && currentSev < severity) 
        {
            lowestSeverity = currentSev;
            targetDoc = d;
        }
    }

    if (targetDoc == -1) 
    {
        cout << "All doctors are handling equally or more critical cases. " << patientsDB[id].name << " remains queued.\n";
        return;
    }

    int oldID = currentPatient[targetDoc];
    pausedPatients[targetDoc].push(oldID); 
    patientsDB[oldID].status = PAUSED;
    
    // remove critical pat.
    EmgItem targetItem = {id, severity, patientsDB[id].arrival};
    emergencyQueue.erase(targetItem);
    
    // new pat.
    currentPatient[targetDoc] = id;
    patientsDB[id].status = TREATING;
    patientsDB[id].doctor = targetDoc + 1;
    
    cout << ">> " << patientsDB[oldID].name << " paused! Doctor " << targetDoc + 1 
         << " taken over by " << patientsDB[id].name << "\n";
}

void addToQueue(int id) 
{
    if (patientsDB.find(id) == patientsDB.end()) return;
    Patient& p = patientsDB[id];

    if (p.severity >= 4) 
    {
        if (p.severity < 6 && emergencyQueue.size() >= maxEmergency) 
        {
            p.status = DEFERRED;
            cout << "Emergency queue full! Patient deferred.\n";
            return;
        }
        emergencyQueue.insert({id, p.severity, p.arrival}); 
        p.status = EMERGENCY;
        cout << "Added to Emergency Queue.\n";
    } 
    else 
    {
        if (normalQueue.size() >= maxNormal) 
        {
            p.status = DEFERRED;
            cout << "Normal queue full. Patient deferred.\n";
            return;
        }
        normalQueue.push(id); 
        p.status = NORMAL;
        cout << "Added to Normal Queue.\n";
    }
}

void registerPatient() 
{
    Patient p;
    p.id = patientIdCounter++;
    p.doctor = -1;
    p.arrival = time(0);

    cout << "\n--- NEW PATIENT ---\nName: ";
    cin.ignore(); getline(cin, p.name);
    p.age = readInt("Age: ", 0, 120);
    cout << "Problem: ";
    cin.ignore(); getline(cin, p.problem);
    
    cout << "Severity Scale:\n1-3: Normal\n4-5: Emergency\n6: CODE RED (Heart Attack, Stroke, etc.)\n";
    p.severity = readInt("Severity (1-6): ", 1, 6);
    p.status = DEFERRED;
    
    patientsDB[p.id] = p; 
    cout << "Registered! ID: " << p.id << "\n";

    addToQueue(p.id);
    schedule();
    
    if ((p.severity == 5 || p.severity == 6) && patientsDB[p.id].status == EMERGENCY) preempt(p.id);
    
}

void updateSeverity() 
{
    int id = readInt("Patient ID: ", 101, patientIdCounter - 1);
    if (patientsDB.find(id) == patientsDB.end() || patientsDB[id].status >= COMPLETED) 
    { 
        cout << "Invalid or closed patient.\n"; 
        return; 
    }

    Patient& p = patientsDB[id];
    int oldStatus = p.status;
    
    if (oldStatus == NORMAL) 
    {
        queue<int> temp;
        while (!normalQueue.empty()) 
        {
            if (normalQueue.front() != id) temp.push(normalQueue.front());
            normalQueue.pop();
        }
        normalQueue = temp;
    } else if (oldStatus == EMERGENCY) 
    {
        emergencyQueue.erase({id, p.severity, p.arrival}); 
    }
    
    p.severity = readInt("New severity (1-6): ", 1, 6);
    if (oldStatus == TREATING || oldStatus == PAUSED) 
    {
        cout << "Severity updated for patient currently in consultation/paused.\n";
    } 
    else 
    {
        addToQueue(id);
        schedule();
        if ((p.severity == 5 || p.severity == 6) && p.status == EMERGENCY) preempt(id);
    }
}

void finishConsultation() 
{
    int d = readInt("Doctor number: ", 1, numDoctors) - 1;
    if (currentPatient[d] == -1) 
    { 
        cout << "Doctor is free.\n"; return; 
    }

    int id = currentPatient[d];
    int choice = readInt("1. Complete  2. Refer: ", 1, 2);
    patientsDB[id].status = (choice == 1) ? COMPLETED : REFERRED;
    patientsDB[id].doctor = -1;
    currentPatient[d] = -1;

    if (!pausedPatients[d].empty()) 
    {
        int old = pausedPatients[d].top();
        pausedPatients[d].pop(); 
        currentPatient[d] = old;
        patientsDB[old].status = TREATING;
        cout << ">> Doctor " << d + 1 << " automatically resumed " << patientsDB[old].name << "\n";
    }
    schedule();
}

void configureClinic() 
{
    cout << "\n=== DYNAMIC CLINIC CONFIGURATION ===\n";
    numDoctors = readInt("Enter number of doctors: ", 1, 50);
    maxNormal = readInt("Enter normal queue capacity: ", 1, 1000);
    maxEmergency = readInt("Enter emergency queue capacity: ", 1, 1000);
    
    currentPatient.assign(numDoctors, -1);
    pausedPatients.assign(numDoctors, stack<int>());
    cout << "Clinic dynamically reconfigured!\n";
}

void searchPatient() 
{
    int id = readInt("Patient ID: ", 101, patientIdCounter - 1);
    if (patientsDB.find(id) == patientsDB.end()) 
    { 
        cout << "Not found.\n"; 
        return; 
    }

    Patient p = patientsDB[id];
    cout << "\nID: " << p.id << "\nName: " << p.name << "\nSeverity: " << p.severity 
         << "\nStatus: " << getStatus(p.status) << "\nWait time: " << waitingTime(id) << " mins\n";
}

void displayDashboard() 
{
    cout << "\n=== CLINIC DASHBOARD ===\n";
    for (int d = 0; d < numDoctors; d++) 
    {
        cout << "Doctor " << d + 1 << ": ";
        if (currentPatient[d] == -1) cout << "Free";
        else cout << "Treating " << patientsDB[currentPatient[d]].name;
        
        if (!pausedPatients[d].empty()) cout << " [" << pausedPatients[d].size() << " patient(s) on pause stack]";
        cout << "\n";
    }
    cout << "\nEmergency Queue (" << emergencyQueue.size() << "/" << maxEmergency << "):\n";
    for (auto it = emergencyQueue.begin(); it != emergencyQueue.end(); ++it) 
    {
        cout << it->id << ". " << patientsDB[it->id].name << " (S" << it->severity << ")\n";
    }
}

int main() 
{
    configureClinic(); // imp. for changeing values dynm.
    
    while (true) 
    {
        cout << "\n=== CLINIC MENU ===\n"
             << "1. Register Patient\n2. View Dashboard\n3. Update Severity\n"
             << "4. Finish Consultation\n5. Search Patient\n6. System Settings\n7. Exit\n";
        int choice = readInt("Choice: ", 1, 7);
        if (choice == 1) registerPatient();
        else if (choice == 2) displayDashboard();
        else if (choice == 3) updateSeverity();
        else if (choice == 4) finishConsultation();
        else if (choice == 5) searchPatient();
        else if (choice == 6) configureClinic();
        else if (choice == 7) break;
    }
    return 0;
}
