# Railways Ticket Reservation System
Team Members:
| S.No | Name | Roll Number |
|------|------|------------|
|1.|Gideon Daniel Giftson T |211111|
|2.|Renganathan M |211129|
## Problem Statement
Creating an application which is a mock version of an actual railways ticket reservation system and allows the system administrator to add and delete trains.
## UML
![UML1](https://user-images.githubusercontent.com/55153800/206425137-e87f954b-f4e0-430b-bdf4-de5985e4273d.JPG)
![UML2](https://user-images.githubusercontent.com/55153800/206425140-90a57f63-7418-4060-a2c4-285f6d9f034e.JPG)
![UML3](https://user-images.githubusercontent.com/55153800/206425129-5561af91-93aa-4f20-8894-a46ae796cd32.JPG)
![UML4](https://user-images.githubusercontent.com/55153800/206425136-fdbddca3-2dff-4831-8e10-9f2104d9399a.JPG)
## Assumptions / Constraints
1. The database only contains trains upto 90 days from the date on which the databases are populated.
2. The program is only able to handle a fixed number of cities. Changing the cities requires recompilation.
3. The duration of each journey is randomized. It may not make sense when the real world distances are taken into account.
4. The trains are from point to point. The intermediate stations are not considered.
5. The prices of the train tickets are also randomized. They may not reflect the reality of the prices.
## Methodology
1. The application uses the MySQL Connector C++ API to query the databases run using a MySQL server.
2. The databases are created and populated methodically in the app itself, and do not require manual insertion of data.
3. The application wields the OOP paradigm to implement the the frontend and backend subsystems for the user and administrator.
## Prerequisites
1. MySQL Server - A database should already be created with the name "railways".
2. MySQL Connector API C++
3. MinGW - For recompiling the program if required.
## Sample IO
![upload1](https://user-images.githubusercontent.com/55153800/206648280-b1015ff6-dec2-4930-8860-7cc7cbee160b.JPG)
![upload2](https://user-images.githubusercontent.com/55153800/206648298-8d179513-7eda-4fff-8184-d4bf143c3683.JPG)
![upload3](https://user-images.githubusercontent.com/55153800/206648307-08cce0f1-adef-42b1-b179-d19d8d977ccf.JPG)
## Sample Databases
### Table: booked_tickets
Stores the data of the booked tickets
![Table: booked_tickets](https://user-images.githubusercontent.com/55153800/206419793-90cebafa-ed36-41dd-9276-b10051f69ffd.JPG)
### Table : schedule
Contains the schedule of the trains
![Table: schedule](https://user-images.githubusercontent.com/55153800/206419803-3a275b8a-30b9-4412-98a8-3ab99088b9cc.JPG)
### Table: userinfo
Stores the user data needed for logging in.
![Table: userinfo](https://user-images.githubusercontent.com/55153800/206419838-877a60f7-101d-44fe-8eb4-3e1f2bbf80f3.JPG)
## License
This application uses the GNU GPL License v3.0.
