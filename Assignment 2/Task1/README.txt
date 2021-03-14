DNS query
========================


Build
-----------------------
gcc -o task1 task1.c


Execution
-----------------------
Execute the program using ./task1.
The program will ask for a hostname whose IP address is to be found using a DNS resolver.
The program will, by default, use the IITH DNS server, which might not be accessible if not connected to IITH VPN. 
DNS_SERVER_1 and DNS_SERVER_2 define this though only DNS_SERVER_1 is used in this program.
In case IITH VPN is not accessible, the program can be used by changing DNS_SERVER_1 to Google DNS (8.8.8.8) or any other DNS resolver.
This program will display the IPv4 address of the provided hostname.


Note
------------------------
The address parsing will fail if the DNS resolver sends multiple DNS addresses for a single hostname. In such cases, the program will display an appropriate error message.
In such cases, please try another hostname, such as google.com.


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
