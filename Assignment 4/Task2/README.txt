Go Back N ARQ
========================


Build -----------------------
Server: gcc -o server task2_server.c -pthread -lm
Client: gcc -o client task2_client.c -lm


Execution
-----------------------
Execute the server first using ./server. The program, by default, runs on localhost.Execute the client using ./client.
The program, by default, runs on localhost. The user needs to specify the file name to be transferred to the server.
The connection is now established.

File transfer will automatically start. The file will be transferred from server to client. You can see the file
transfer progress on the terminal.

Once transfer is complete the programs will exit on their own. To confirm if the file is completely transferred,
check the file size of both sent and received file. If the transfer completed correctly, the file sizes should be
same. File size can be checked using any file explorer.

Effect of delay and loss
-----------------------------
Since our program waits for a specified time defined by TIMEOUT before retransmitting the file. On adding random delay
and loss some of the packets do not receive their acknowledgement in specified time. Such packets are retransmitted after
specified TIMEOUT time. Our file takes longer to transmit and number of not acknowledged packets increases as loss is
increased but our full file is transmitted successfully.


Plagarism Statement
------------------------
I certify that this assignment/report is my own work, based on my personal study and/or research and that I have
acknowledged all material and sources used in its preparation, whether they be books, articles, reports, lecture notes,
and any other kind of document, electronic or personal communication. I also certify that this assignment/report has not
previously been submitted for assessment in any other course, except where specific permission has been granted from all
course instructors involved, or at any other time in this course, and that I have not copied in part or whole or otherwise
plagiarised the work of other students and/or persons. I pledge to uphold the principles of honesty and responsibility
at CSE@IITH. In addition, I understand my responsibility to report honour violations by other students if I become aware of it.
