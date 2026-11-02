Clinic Management System

A menu-driven Clinic Management and Emergency Triage Simulator developed in C++ using Data Structures and Algorithms (DSA).

The system simulates patient registration, severity-based triage, waiting queue management, doctor allocation, emergency preemption, and consultation tracking.

Project Overview

The Clinic Management System is designed to demonstrate how data structures support patient scheduling and clinic resource management.

The program manages two doctors and maintains separate queues for normal and emergency patients. Patients receive a severity level assigned by a nurse, which determines their scheduling priority.

Critical patients with severity level 5 receive special handling through an emergency preemption mechanism requiring staff approval.

Features

* Register patients with automatically generated patient IDs.
* Store patient name, age, medical problem, and severity level.
* Perform nurse-assigned triage using severity levels from 1 to 5.
* Maintain separate normal and emergency waiting queues.
* Allocate waiting patients to two available doctors.
* Prioritize emergency patients over normal patients.
* Support critical emergency preemption with staff approval.
* Pause and resume interrupted consultations.
* Update patient severity levels.
* Search patient details using patient ID.
* Display current patient queues and doctor assignments.
* Mark consultations as completed or referred.
* Track elapsed time since patient registration.
* Validate numerical input.

Data Structures Used

Data Structure	C++ Implementation	Purpose
Vector	vector<Patient>	Stores registered patients
Queue	queue<int>	Maintains normal patients in FIFO order
Priority Queue / Heap	priority_queue<pair<int,int>>	Schedules emergency patients by severity
Hash Table	unordered_map<int,int>	Maps patient IDs to vector indices
Array	currentPatient[2]	Tracks patients currently assigned to doctors
Array	pausedPatient[2]	Tracks paused consultations
Pair	pair<int,int>	Stores emergency severity and patient ID
Structure	struct Patient	Groups patient information

Patient Severity Classification

Severity	Queue	Scheduling
1	Normal	FIFO
2	Normal	FIFO
3	Normal	FIFO
4	Emergency	Priority-based
5	Emergency	Highest priority with preemption support

Severity levels are assigned manually by a nurse in the simulation.

For emergency patients with equal severity, the lower patient ID receives higher priority.

Scheduling Algorithms

1. First In First Out (FIFO)

Normal patients are managed using a standard queue.

* Patients enter the normal queue in order.
* The first patient added is the first patient selected.
* Each available doctor receives the next eligible patient.

2. Priority Scheduling

Emergency patients are managed using a max-heap priority queue.

* Higher severity receives higher priority.
* Severity 5 receives priority over severity 4.
* Emergency patients are selected before normal patients.
* Patient ID breaks ties between equal severity levels.

3. Emergency Preemption

The system includes a staff-approved emergency interruption mechanism for severity 5 patients.

When a critical patient is waiting and both doctors are occupied:

1. The system displays both active consultations.
2. Staff selects a doctor to interrupt.
3. The system checks whether interruption is permitted.
4. Staff must approve the interruption.
5. The original consultation is paused.
6. The critical patient is assigned to the selected doctor.

Each doctor supports one paused patient at a time.

4. Consultation Resumption

After completing or referring an active consultation:

* The system offers the option to resume the paused patient.
* If approved, the interrupted consultation resumes.
* Staff also have a separate menu option to resume a paused patient when their doctor is available.

Clinic Capacity

Resource	Maximum Capacity
Normal waiting queue	20 patients
Emergency waiting queue	5 patients
Active doctors	2
Paused patients	1 per doctor

Queue capacities represent waiting patients only. Patients already receiving treatment are not counted in waiting queue capacity.

When a queue reaches its limit, new patients assigned to it receive a deferred status.

Patient Statuses

The program tracks the following statuses:

* Normal Waiting: Waiting in the FIFO queue.
* Emergency Waiting: Waiting in the emergency priority queue.
* In Consultation: Assigned to a doctor.
* Paused: Consultation interrupted.
* Completed: Consultation finished.
* Referred: Patient referred for additional care.
* Deferred: Patient not currently placed in a waiting queue and requires staff attention or later scheduling.

Technologies Used

* Programming Language: C++
* Standard: C++11 or later
* Interface: Command-line interface
* Libraries: iostream, vector, queue, unordered_map, string, ctime, limits

How to Run

Requirements

A C++ compiler supporting C++11 or later.

Step 1: Clone the Repository

git clone https://github.com/dogesh007/Patient-Management-System-for-Clinics

Step 2: Navigate to the Project Folder

cd Clinic-Management-System

Step 3: Compile

g++ -std=c++11 main.cpp -o clinic

Step 4: Run

On macOS or Linux:

./clinic

On Windows:

clinic.exe

Program Menu

====== CLINIC MANAGEMENT ======
1. Register and Triage Patient
2. Search Patient
3. Display Waiting Queues
4. Display Doctors
5. Update Severity
6. Complete / Refer Consultation
7. Request Emergency Preemption
8. Resume Paused Patient
9. Exit

Example Workflow

Patient Registration

--- NEW PATIENT ---
Name: Rahul
Age: 25
Main problem: Fever
Nurse performs initial triage.
Nurse-assigned severity (1-5): 2
Patient registered. ID: 101
Added to normal FIFO queue.
Doctor 1 is treating Rahul
Waiting time: 0 minutes

Emergency Patient Registration

Suppose both doctors are already occupied.

--- NEW PATIENT ---
Name: Amit
Age: 45
Main problem: Chest pain
Nurse performs initial triage.
Nurse-assigned severity (1-5): 5
Patient registered. ID: 103
Added to emergency priority queue.
CRITICAL PATIENT: Amit
Emergency staff attention required.

The system then asks staff whether an existing consultation should be interrupted.

These examples illustrate simulated inputs, not automatic medical diagnosis or clinical prioritization.

Time Tracking

The system records each patient’s arrival using:

time(0)

Elapsed time since registration is calculated using:

difftime(time(0), patients[i].arrival) / 60

The value is converted to integer minutes.

This measures elapsed time since registration, including any consultation time, rather than strictly measuring time spent in waiting queues.


Assumptions and Limitations

* The simulation manages exactly two doctors.
* Severity is entered manually.
* The application does not perform medical diagnosis.
* Queue capacities are fixed.
* Only severity 5 patients trigger the critical preemption procedure.
* Consultation durations are not automatically simulated.
* Patient records exist only during the current program session.
* No database or permanent storage is implemented.
* The displayed elapsed time is measured from patient registration.
* The system is an educational simulation, not software intended for real clinical decisions.



