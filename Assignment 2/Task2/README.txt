Asynchronous non-blocking TCP chat
========================


Build
-----------------------
Server: gcc -o server task2_server.cpp
Client: gcc -pthread -o client task2_client.cpp


Execution
-----------------------
Execute the server first using ./server. The program, by default, runs on localhost. The user needs to provide the maximum number of clients that can connect simultaneously.
Execute the client using ./client. The program, by default, runs on localhost. The client program needs to be executed for every new client in a separate terminal.
The connection is now established. All the messages are written on the client-side. Once the message is sent, it will be displayed on all other clients as
    R: message
R: here describes the message received from the server, which is sent from any other client.
The message can be sent from any client at any point, and it will be displayed on all other clients.


Note
------------------------
The programs, both server, and client do not exit on their own. The programs need to be killed in the terminal to exit.


Plagarism Statement
------------------------
I certify that this assignment/report is my own work, based on my personal study and/or
research and that I have acknowledged all material and sources used in its preparation, whether
they be books, articles, reports, lecture notes, and any other kind of document, electronic or
personal communication. I also certify that this assignment/report has not previously been
submitted for assessment in any other course, except where specific permission has been
granted from all course instructors involved, or at any other time in this course, and that I have
not copied in part or whole or otherwise plagiarised the work of other students and/or persons. I
pledge to uphold the principles of honesty and responsibility at CSE@IITH. In addition, I
understand my responsibility to report honour violations by other students if I become aware of
it.
