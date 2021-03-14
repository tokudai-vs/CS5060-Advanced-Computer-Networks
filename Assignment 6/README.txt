L4 convertor with port NAT
========================


Build
-----------------------
Server: gcc -o server server.c -pthread
Client: gcc -o client client.c -pthread
Translator: gcc -o translator translator.c -pthread


Execution
-----------------------
Execute the server first using ./server. The program, by default, runs on localhost.
Execute the translator second using ./translator in a different terminal. The program, by default, runs on localhost.
Lastly execute multiple instances of the client using ./client. The program, by
default, runs on localhost. You will need to run each client in individual terminal.
The connection is now established.

If no errors are shown, enter the message on one of the client's terminals.

The translator will show the message received from the client and  the IP address of
the client. Each client will have a different IP address. The translator will
then attempt to send the message to the server and show appropriate messages on the terminal.

On server side, the server will show the IP address and the port from which the
message is received along with the message it has received from the client. The
server then replies with the IP, port and packet number it received.

On the client side, the client from which the message was sent will display the
message received from the server. It also shows the RTT in nanoseconds (this was
chosen as program is running on localhost). Each client will display information
only about the packet it sent. Multiple client can be connected to same NAT.

Since NAT sends its own IP address instead of client's IP address, all replies
received from server will contain the same IP address but different packet number.


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
